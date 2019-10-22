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

bool OboeAudioSystem::initOboeSources() {
    const int maxSources = ET_getConfig<AudioConfig>()->maxSoundSources;
    for(int i = 0; i < maxSources; ++i) {
        oboeSources.emplace_back(new OboeSoundSource);
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
    if(!initOboeSources()) {
        return false;
    }
    if(!initOboeStream()) {
        return false;
    }
    ETNode<ETAudioSystem>::connect(getEntityId());
    return true;
}

void OboeAudioSystem::deinit() {
    ETNode<ETAudioSystem>::disconnect();
    return;
}

std::vector<SoundSource*> OboeAudioSystem::ET_getSourcesToManage() {
    std::vector<SoundSource*> sources;
    for(int i = 0, sz = oboeSources.size(); i < sz; ++i) {
        sources.emplace_back(oboeSources[i].get());
    }
    return sources;
}

void OboeAudioSystem::onErrorAfterClose(oboe::AudioStream* stream, oboe::Result res) {
    LogError("[OboeAudioSystem::onErrorAfterClose] Oboe stream error: %s", oboe::convertToText(res));
}

oboe::DataCallbackResult OboeAudioSystem::onAudioReady(oboe::AudioStream *outStream, void *audioData, int32_t numFrames) {
    auto outChannels = outStream->getChannelCount();
    auto outFormat = outStream->getFormat();
    mixer.startMixing(outChannels, outFormat, audioData, numFrames);
    for(int i = 0, sz = oboeSources.size(); i < sz; ++i) {
        auto& source = oboeSources[i];
        mixer.addSource(*source);
    }
    mixer.endMixing();
    return oboe::DataCallbackResult::Continue;
}