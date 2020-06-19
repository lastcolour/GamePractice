#include "SoundManager.hpp"
#include "Sound.hpp"
#include "OggDataStream.hpp"
#include "Core/ETLogger.hpp"
#include "Core/ETAssets.hpp"

namespace {

const char* SOUNDS_ROOT_DIR = "Sounds";

} // namespace

SoundManager::SoundManager() {
}

SoundManager::~SoundManager() {
}

bool SoundManager::init() {
    ETNode<ETSoundManager>::connect(getEntityId());
    return true;
}

void SoundManager::deinit() {
}

Buffer SoundManager::loadSoundBuffer(const char* soundName) {
    if(!soundName || !soundName[0]) {
        LogError("[SoundManager::loadSoundBuffer] Can't load empty sound");
        return Buffer();
    }
    auto it = buffers.find(soundName);
    if(it != buffers.end()) {
        return it->second;
    }

    Buffer buff;
    std::string reqSoundName = StringFormat("%s/%s", SOUNDS_ROOT_DIR, soundName);
    ET_SendEventReturn(buff, &ETAssets::ET_loadAsset, reqSoundName.c_str());

    if(buff) {
        buffers[soundName] = buff;
    }

    return buff;
}

std::unique_ptr<Sound> SoundManager::ET_createSound(const char* soundName) {
    auto buff = loadSoundBuffer(soundName);
    if(!buff) {
        return nullptr;
    }
    std::unique_ptr<OggDataStream> dataStream(new OggDataStream(buff));
    if(!dataStream->isOpened()) {
        LogError("[SoundManager::ET_createSound] Can't make OGG stream from: '%s'", soundName);
        return nullptr;
    }

    if(dataStream->channels > 2) {
        LogError("[SoundManager::ET_createSound] Too many channels %d in OGG stream: '%s'",  dataStream->channels, soundName);
        return nullptr;
    }
    std::unique_ptr<Sound> sound(new Sound(std::move(dataStream)));
    return sound;
}