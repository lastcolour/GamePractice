#include "Audio/Logics/SoundPlayLogic.hpp"
#include "Core/JSONNode.hpp"
#include "ETApplicationInterfaces.hpp"

SoundPlayLogic::SoundPlayLogic() :
    looped(false),
    volume(1.f),
    autoStart(false) {
}

SoundPlayLogic::~SoundPlayLogic() {
}

bool SoundPlayLogic::serialize(const JSONNode& node) {
    std::string soundName;
    node.read("sound", soundName);
    node.read("loop", looped);
    node.read("volume", volume);
    node.read("autoStart", autoStart);
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
    if(autoStart) {
        ET_play();
    }
    return true;
}

void SoundPlayLogic::ET_setSound(const char* newSoundName) {
    ET_SendEventReturn(sound, &ETSoundManager::ET_createSound, newSoundName);
}

void SoundPlayLogic::ET_play() {
    sound->play(looped);
    sound->setVolume(volume);
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