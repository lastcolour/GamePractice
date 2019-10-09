#include "Audio/Logics/SoundPlayLogic.hpp"
#include "Core/JSONNode.hpp"
#include "ETApplicationInterfaces.hpp"

SoundPlayLogic::SoundPlayLogic() :
    looped(false) {
}

SoundPlayLogic::~SoundPlayLogic() {
}

bool SoundPlayLogic::serialize(const JSONNode& node) {
    std::string soundName;
    node.read("sound", soundName);
    node.read("looped", looped);
    if(!soundName.empty()) {
        ET_SendEventReturn(sound, &ETSoundManager::ET_createSound, soundName.c_str());
        if(!sound) {
            return false;
        }
    } else {
        LogWarning("[SoundPlayLogic::serialize] Empty sound name");
        return false;
    }
    return true;
}

bool SoundPlayLogic::init() {
    ETNode<ETSoundPlayer>::connect(getEntityId());
    return true;
}

void SoundPlayLogic::ET_setSound(const char* newSoundName) {
    ET_SendEventReturn(sound, &ETSoundManager::ET_createSound, newSoundName);
}

void SoundPlayLogic::ET_play() {
    sound->play();
}

void SoundPlayLogic::ET_pause() {
    sound->pause();
}

void SoundPlayLogic::ET_resume() {
    sound->resume();
}

void SoundPlayLogic::ET_stop() {
    sound->stop();
}