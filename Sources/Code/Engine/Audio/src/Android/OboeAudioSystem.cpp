#include "Android/OboeAudioSystem.hpp"
#include "Platform/ETDevice.hpp"
#include "Nodes/ETSoundNodeManager.hpp"
#include "Nodes/ETSoundNode.hpp"

#include <cassert>

namespace {

const int FADE_IN_SAMPLES = 256;
const int FADE_OUT_SAMPLES = 256;

} // namespace

OboeAudioSystem::OboeAudioSystem() :
    oboeStream(nullptr),
    canMix(false) {
}

OboeAudioSystem::~OboeAudioSystem() {
}

bool OboeAudioSystem::initMixer() {
    const DeviceAudioConfig* deviceConfig = nullptr;
    ET_SendEventReturn(deviceConfig, &ETDevice::ET_getAudioConfig);
    if(!deviceConfig) {
        LogError("[OboeAudioSystem::initMixer] Can't get config of audio device");
        return false;
    }

    MixConfig mixConfig;
    mixConfig.outSampleRate = deviceConfig->frameRate;
    mixConfig.samplesPerBurst = deviceConfig->framesPerBurst;

    if(!mixGraph.init(mixConfig)) {
        LogError("[OboeAudioSystem::initMixer] Can't init mix graph");
        return false;
    }

    mixBuffer.reset(new float[mixConfig.samplesPerBuffer * mixConfig.channels]);
    if(!mixBuffer) {
        LogWarning("[OboeAudioSystem::initAlSource] Can't allocate mix buffer");
        return false;
    }

    bufferQueue.init(mixConfig.buffersCount);
    return true;
}

bool OboeAudioSystem::initOboeStream() {
    auto& mixConfig = mixGraph.getMixConfig();

    oboe::AudioStreamBuilder builder;
    builder.setDirection(oboe::Direction::Output);
    builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
    builder.setSharingMode(oboe::SharingMode::Exclusive);
    builder.setCallback(this);
    builder.setSampleRate(mixConfig.outSampleRate);
    builder.setChannelCount(mixConfig.channels);
    builder.setFormat(oboe::AudioFormat::I16);
    builder.setSampleRateConversionQuality(oboe::SampleRateConversionQuality::Fastest);
    builder.setUsage(oboe::Usage::Game);

    auto res = builder.openStream(&oboeStream);
    if(res != oboe::Result::OK){
        LogError("[OboeAudioSystem::initOboeStream] Can't open oboe stream (Error: %s)", oboe::convertToText(res));
        return false;
    }
    res = oboeStream->setBufferSizeInFrames(mixConfig.samplesPerBurst);
    if(res != oboe::Result::OK){
        LogWarning("[OboeAudioSystem::initOboeStream] Can't change stream's buffer size (Error: %s)", oboe::convertToText(res));
    }

    assert(oboeStream->getFormat() == oboe::AudioFormat::I16);
    return true;
}

void OboeAudioSystem::ET_onActivityEvent(ActivityEventType eventType) {
    if(eventType == ActivityEventType::OnWindowFocusGet) {
        auto res = oboeStream->requestStart();
        if(res != oboe::Result::OK) {
            LogError("[OboeAudioSystem::ET_onActivityEvent] Can't start oboe stream (Error: %s)", oboe::convertToText(res));
        } else {
            canMix.store(true);
            LogInfo("[OboeAudioSystem::ET_onActivityEvent] Resume oboe stream");
        }
    } else if(eventType == ActivityEventType::OnWindowFocusLost) {
        canMix.store(false);
        auto res = oboeStream->requestPause();
        if(res != oboe::Result::OK) {
            LogError("[OboeAudioSystem::ET_onActivityEvent] Can't pause oboe stream (Error: %s)",
                oboe::convertToText(res));
        } else {
            LogInfo("[OboeAudioSystem::ET_onActivityEvent] Pause oboe stream");
        }
    }
}

bool OboeAudioSystem::init() {
    if(!initMixer()) {
        return false;
    }
    if(!initOboeStream()) {
        return false;
    }

    ETNode<ETSoundUpdateTask>::connect(getEntityId());
    ETNode<ETAudioSystem>::connect(getEntityId());
    ETNode<ETSoundPlayManager>::connect(getEntityId());
    ETNode<ETAndroidActivityEvents>::connect(getEntityId());

    return true;
}

void OboeAudioSystem::ET_updateSound() {
    ET_PollAllEvents<ETAudioSystem>();
    ET_PollAllEvents<ETSoundNodeManager>();
    ET_PollAllEvents<ETSoundNode>();
    ET_PollAllEvents<ETSoundEventNode>();

    if(!canMix.load()) {
        return;
    }

    auto& mixConfig = mixGraph.getMixConfig();
    auto buffers = bufferQueue.peekWrites();

    if(buffers.size() == mixConfig.buffersCount) {
        LogError("[OboeAudioSystem::ET_updateSound] Very slow mixing");
    }

    for(auto& buff : buffers) {
        mixGraph.mixBufferAndConvert(mixBuffer.get());
        buff->write(mixBuffer.get(), mixConfig.samplesPerBuffer * mixConfig.channels * sizeof(int16_t));
    }
    bufferQueue.submitWrites(buffers);
}

void OboeAudioSystem::ET_setEqualizer(ESoundGroup soundGroup, const EqualizerSetup& eqSetup) {
    mixGraph.setEqualizer(soundGroup, eqSetup);
}

bool OboeAudioSystem::ET_play(SoundStream* soundStream) {
    return mixGraph.playSound(soundStream);
}

void OboeAudioSystem::ET_setMasterVolume(float newVolume) {
    mixGraph.setMasterVolume(newVolume);
}

float OboeAudioSystem::ET_getMasterVolume() const {
    return mixGraph.getMasterVolume();
}

void OboeAudioSystem::stopOboeStream() {
    if(!oboeStream) {
        return;
    }
    oboeStream->stop();
}

void OboeAudioSystem::deinit() {
    stopOboeStream();
    return;
}

void OboeAudioSystem::onErrorAfterClose(oboe::AudioStream* stream, oboe::Result res) {
    LogError("[OboeAudioSystem::onErrorAfterClose] Error: %s", oboe::convertToText(res));
}

oboe::DataCallbackResult OboeAudioSystem::onAudioReady(oboe::AudioStream* outStream, void* audioData, int32_t numFrames) {
    auto& mixConfig = mixGraph.getMixConfig();

    assert(outStream->getChannelCount() == mixConfig.channels && "Invalid channles count");

    int totalBytes = numFrames * mixConfig.channels * sizeof(int16_t);
    int bytesLeft = totalBytes;
    char* out = static_cast<char*>(audioData);
    int offset = 0;
    while(bytesLeft != 0) {
        auto buff = bufferQueue.peekRead();
        if(!buff) {
            break;
        }
        bytesLeft -= buff->read(out + offset, bytesLeft);
        offset = totalBytes - bytesLeft;
        bufferQueue.tryPopRead();
    }

    if(bytesLeft > 0) {
        std::fill_n(reinterpret_cast<int16_t*>(out + offset), bytesLeft / sizeof(int16_t), 0);
        if(canMix.load()) {
            LogError("[OboeAudioSystem::onAudioReady] Not enough data: %d", bytesLeft);
        }
    }

    if(canMix.load()) {
        if(!fader.isFadeInState()) {
            fader.setFadeIn(std::min(mixConfig.samplesPerBurst, FADE_IN_SAMPLES));
        }
    }

    fader.exclusiveTransformInt16(static_cast<int16_t*>(audioData),
        mixConfig.channels, numFrames);

    return oboe::DataCallbackResult::Continue;
}