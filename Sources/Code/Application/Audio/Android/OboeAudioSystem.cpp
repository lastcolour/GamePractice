#include "Audio/Android/OboeAudioSystem.hpp"
#include "Audio/AudioConfig.hpp"
#include "Platforms/Android/ETAndroidInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"

namespace {

const int BUFFERS_SIZE_PER_BURST = 2;

} // namespace

OboeAudioSystem::OboeAudioSystem() :
    oboeStream(nullptr) {
}

OboeAudioSystem::~OboeAudioSystem() {
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
    return true;
}

bool OboeAudioSystem::initSoundSources() {
    const int maxSoundSources = ET_getConfig<AudioConfig>()->maxSoundSources;
    for(int i = 0; i < maxSoundSources; ++i) {
        sources.emplace_back();
        sourceStateMap.emplace_back(ESourceState::Free);
    }
    return true;
}

bool OboeAudioSystem::init() {
    if(!initOboeStream()) {
        return false;
    }
    if(!initSoundSources()) {
        return false;
    }
    ETNode<ETSoundSourceManager>::connect(getEntityId());
    return true;
}

void OboeAudioSystem::deinit() {
    return;
}

SoundSource* OboeAudioSystem::ET_getFreeSource() {
    return nullptr;
}

void OboeAudioSystem::ET_returnSoundSource(SoundSource* soundSoruce) {
}

oboe::DataCallbackResult OboeAudioSystem::onAudioReady(oboe::AudioStream *outStream, void *audioData, int32_t numFrames) {
    auto outChannels = outStream->getChannelCount();
    auto outFormat = outStream->getFormat();
    mixer.startMixing(outChannels, outFormat, audioData, numFrames);
    for(int i = 0, sz = sources.size(); i < sz; ++i) {
        auto& source = sources[i];
        if(sourceStateMap[i] != ESourceState::Free) {
            mixer.addSource(source);
        }
    }
    mixer.endMixing();
    return oboe::DataCallbackResult::Continue;
}