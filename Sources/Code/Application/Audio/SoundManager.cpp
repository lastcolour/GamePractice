#include "Audio/SoundManager.hpp"
#include "Audio/Sound.hpp"
#include "Audio/OggDataStream.hpp"
#include "ETApplicationInterfaces.hpp"

#include <AL/al.h>
#include <AL/alc.h>

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

std::shared_ptr<Sound> SoundManager::loadSound(const char* soundName) {
    return nullptr;
}

std::unique_ptr<Sound> SoundManager::ET_createSound(const char* soundName) {
    if(!soundName || !soundName[0]) {
        LogError("[SoundManager::ET_createSound] Can't load empty sound");
        return nullptr;
    }
    auto it = sounds.find(soundName);
    if(it != sounds.end()) {
        return nullptr;
        //return it->second;
    }
    Buffer buff;
    std::string reqSoundName = StringFormat("%s/%s", SOUNDS_ROOT_DIR, soundName);
    ET_SendEventReturn(buff, &ETAssets::ET_loadAsset, reqSoundName.c_str());
    if(!buff) {
        return nullptr;
    }

    std::shared_ptr<OggDataStream> dataStream(new OggDataStream(buff));
    if(!dataStream->isOpened()) {
        LogError("[SoundManager::ET_createSound] Can't make OGG stream from: %s", soundName);
        return nullptr;
    }

    if(dataStream->channels > 2) {
        LogError("[SoundManager::ET_createSound] Too many channels %d in OGG stream: %s",  dataStream->channels, soundName);
        return nullptr;
    }

    std::unique_ptr<Sound> sound(new Sound);
    sound->dataStream = dataStream;
    sound->soundSource = nullptr;
    return sound;
}