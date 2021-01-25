#include "Android/OboeAudioSystem.hpp"
#include "Platform/ETDevice.hpp"
#include "Core/ETLogger.hpp"

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
    mixConfig.samplesPerBuffer = (mixConfig.outSampleRate * mixConfig.buffersTime) / mixConfig.buffersCount;

    if(!mixGrap.init(mixConfig)) {
        LogError("[ALAudioSystem::initMixer] Can't init mix graph");
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
        LogError("[OboeAudioSystem::initOboeStream] Can't open oboe stream. Error: %s", oboe::convertToText(res));
        return false;
    }
    res = oboeStream->setBufferSizeInFrames(mixConfig.samplesPerBurst);
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
    if(!initMixer()) {
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
    auto& mixConfig = mixGrap.getMixConfig();
    auto buffers = bufferQueue.getNextWrites();
    for(auto& buff : buffers) {
        int buffReqSize = mixConfig.samplesPerBuffer * mixConfig.channels * sizeof(float);
        buff->resize(buffReqSize);
        mixGrap.mixBufferAndConvert(static_cast<float*>(buff->getPtr()));
        buff->setReadSize(buffReqSize / 2);
    }
    bufferQueue.putWritesDone(buffers);
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
    auto& mixConfig = mixGrap.getMixConfig();
    int totalSizeToCopy = numFrames * mixConfig.channels * sizeof(int16_t);
    int sizeToCopy = totalSizeToCopy;
    while(sizeToCopy > 0) {
        auto readBuff = bufferQueue.getNextRead();
        if(readBuff) {
            int canCopySize = std::min(readBuff->getAvaibleSizeForRead(), sizeToCopy);
            memcpy(audioData, readBuff->getPtr(), canCopySize);

            readBuff->setReadDone(canCopySize);
            bufferQueue.putReadDone(readBuff);
            sizeToCopy -= canCopySize;
        } else {
            int zerosCount = sizeToCopy / 2;
            int offset = totalSizeToCopy - sizeToCopy;
            int16_t* ptr = reinterpret_cast<int16_t*>(
                reinterpret_cast<char*>(audioData) + offset);
            std::fill_n(ptr, zerosCount, 0);
            break;
        }
    }
    return oboe::DataCallbackResult::Continue;
}