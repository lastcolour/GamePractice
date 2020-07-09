#include "Audio/Android/OboeAudioSystem.hpp"
#include "Audio/AudioConfig.hpp"
#include "Audio/Android/OboeSoundSource.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Platforms/Android/ETAndroidInterfaces.hpp"

namespace {

const int BUFFERS_SIZE_PER_BURST = 2;

} // namespace

OboeAudioSystem::OboeAudioSystem() :
    oboeStream(nullptr) {
}

OboeAudioSystem::~OboeAudioSystem() {
}

bool OboeAudioSystem::initOboeMixer() {
    const AndroidAudioDeviceConfig* deviceConfig = nullptr;
    ET_SendEventReturn(deviceConfig, &ETAndroidAudioDevice::ET_getAudioConfig);
    if(!deviceConfig) {
        LogError("[OboeAudioSystem::initOboeMixer] Can't get config of audio device");
        return false;
    }
    return true;
}

bool OboeAudioSystem::initOboeStream() {
    oboe::AudioStreamBuilder builder;
    builder.setDirection(oboe::Direction::Output);
    builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
    builder.setSharingMode(oboe::SharingMode::Exclusive);
    builder.setCallback(this);
    auto res = builder.openStream(&oboeStream);
    if(res != oboe::Result::OK){
        LogError("[OboeAudioSystem::init] Can't open oboe stream. Error: %s", oboe::convertToText(res));
        return false;
    }
    res = oboeStream->setBufferSizeInFrames(oboeStream->getFramesPerBurst() * BUFFERS_SIZE_PER_BURST);
    if(res != oboe::Result::OK){
        LogWarning("[OboeAudioSystem::init] Can't change stream's buffer size. Error: %s", oboe::convertToText(res));
    }
    res = oboeStream->start();
    if(res != oboe::Result::OK) {
        LogWarning("[OboeAudioSystem::init] Can't start oboe stream. Error: %s", oboe::convertToText(res));
    }
    return true;
}

bool OboeAudioSystem::init() {
    if(!initOboeMixer()) {
        return false;
    }
    if(!initOboeSources()) {
        return false;
    }
    if(!initOboeStream()) {
        return false;
    }
    return true;
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
    return oboe::DataCallbackResult::Continue;
}