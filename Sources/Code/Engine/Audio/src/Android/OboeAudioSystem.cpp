#include "Android/OboeAudioSystem.hpp"
#include "Platform/ETDevice.hpp"
#include "Core/ETLogger.hpp"

namespace {

const int BUFFERS_SIZE_PER_BURST = 2;

} // namespace

OboeAudioSystem::OboeAudioSystem() :
    oboeStream(nullptr) {
}

OboeAudioSystem::~OboeAudioSystem() {
}

bool OboeAudioSystem::initOboeStream() {
    const DeviceAudioConfig* deviceConfig = nullptr;
    ET_SendEventReturn(deviceConfig, &ETDevice::ET_getAudioConfig);
    if(!deviceConfig) {
        LogError("[OboeAudioSystem::initOboeStream] Can't get config of audio device");
        return false;
    }

    oboe::AudioStreamBuilder builder;
    builder.setDirection(oboe::Direction::Output);
    builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
    builder.setSharingMode(oboe::SharingMode::Exclusive);
    builder.setCallback(this);
    builder.setFormat(oboe::AudioFormat::Float);
    builder.setSampleRate(deviceConfig->frameRate);
    builder.setChannelCount(2);
    builder.setSampleRateConversionQuality(oboe::SampleRateConversionQuality::Fastest);
    builder.setUsage(oboe::Usage::Game);

    auto res = builder.openStream(&oboeStream);
    if(res != oboe::Result::OK){
        LogError("[OboeAudioSystem::initOboeStream] Can't open oboe stream. Error: %s", oboe::convertToText(res));
        return false;
    }
    res = oboeStream->setBufferSizeInFrames(deviceConfig->framesPerBurst);
    if(res != oboe::Result::OK){
        LogWarning("[OboeAudioSystem::initOboeStream] Can't change stream's buffer size. Error: %s", oboe::convertToText(res));
    }
    res = oboeStream->start();
    if(res != oboe::Result::OK) {
        LogWarning("[OboeAudioSystem::initOboeStream] Can't start oboe stream. Error: %s", oboe::convertToText(res));
    }
    return true;
}

bool OboeAudioSystem::init() {
    if(!mixGrap.init()) {
        LogError("[ALAudioSystem::init] Can't init mix graph");
        return false;
    }
    if(!initOboeStream()) {
        return false;
    }

    ETNode<ETSoundPlayManager>::connect(getEntityId());
    ETNode<ETSoundUpdateTask>::connect(getEntityId());
    ETNode<ETAudioSystem>::connect(getEntityId());

    return true;
}

void OboeAudioSystem::ET_updateSound() {
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
    LogError("[OboeAudioSystem::onErrorAfterClose] Oboe stream error: %s", oboe::convertToText(res));
}

oboe::DataCallbackResult OboeAudioSystem::onAudioReady(oboe::AudioStream* outStream, void* audioData, int32_t numFrames) {
    float* out = static_cast<float*>(audioData);
    for(int i = 0; i < numFrames; ++i) {
        out[2 * i] = 0.f;
        out[2 * i + 1] = 0.f;
    }
    return oboe::DataCallbackResult::Continue;
}