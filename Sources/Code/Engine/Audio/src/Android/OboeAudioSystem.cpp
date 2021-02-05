#include "Android/OboeAudioSystem.hpp"
#include "Platform/ETDevice.hpp"
#include "Core/ETLogger.hpp"
#include "Nodes/ETSoundNodeManager.hpp"
#include "Nodes/ETSoundNode.hpp"

#include <cassert>

OboeAudioSystem::OboeAudioSystem() :
    oboeStream(nullptr) {
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

    if(!mixGrap.init(mixConfig)) {
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
    auto& mixConfig = mixGrap.getMixConfig();

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
            LogInfo("[OboeAudioSystem::ET_onActivityEvent] Resume oboe stream");
        }
    } else if(eventType == ActivityEventType::OnWindowFocusLost) {
        auto res = oboeStream->requestPause();
        if(res != oboe::Result::OK) {
            LogError("[OboeAudioSystem::ET_onActivityEvent] Can't pause oboe stream (Error: %s)", oboe::convertToText(res));
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

    auto& mixConfig = mixGrap.getMixConfig();
    auto buffers = bufferQueue.peekWrites();
    for(auto& buff : buffers) {
        mixGrap.mixBufferAndConvert(mixBuffer.get());
        buff->write(mixBuffer.get(), mixConfig.samplesPerBuffer * mixConfig.channels * sizeof(int16_t));
    }
    bufferQueue.submitWrites(buffers);
}

void OboeAudioSystem::ET_setEqualizer(ESoundGroup soundGroup, const EqualizerSetup& eqSetup) {
    mixGrap.setEqualizer(soundGroup, eqSetup);
}

bool OboeAudioSystem::ET_play(SoundStream* soundStream) {
    return mixGrap.playSound(soundStream);
}

void OboeAudioSystem::ET_setMasterVolume(float newVolume) {
    mixGrap.setMasterVolume(newVolume);
}

float OboeAudioSystem::ET_getMasterVolume() const {
    return mixGrap.getMasterVolume();
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
    auto& mixConfig = mixGrap.getMixConfig();
    int totalSize = numFrames * mixConfig.channels * sizeof(int16_t);
    int currSize = totalSize;
    char* out = static_cast<char*>(audioData);
    int offset = 0;
    while(currSize > 0) {
        auto buff = bufferQueue.peekRead();
        if(buff) {
            currSize -= buff->read(out + offset, currSize);
            offset = totalSize - currSize;
            bufferQueue.tryPopRead();
        } else {
            char* ptr = reinterpret_cast<char*>(audioData) + offset;
            std::fill_n(reinterpret_cast<int16_t*>(ptr), currSize / 2, 0);
            break;
        }
    }
    return oboe::DataCallbackResult::Continue;
}