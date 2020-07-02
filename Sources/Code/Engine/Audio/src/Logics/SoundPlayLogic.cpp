#include "Logics/SoundPlayLogic.hpp"
#include "Reflect/ReflectContext.hpp"

SoundPlayLogic::SoundPlayLogic() :
    volume(1.f),
    looped(false),
    autoStart(false) {
}

SoundPlayLogic::~SoundPlayLogic() {
}

void SoundPlayLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<SoundPlayLogic>("SoundPlayer")) {
        classInfo->addResourceField("sound", &SoundPlayLogic::ET_setSound);
        classInfo->addField("looped", &SoundPlayLogic::looped);
        classInfo->addField("volume", &SoundPlayLogic::volume);
        classInfo->addField("autoStart", &SoundPlayLogic::autoStart);
    }
}

bool SoundPlayLogic::init() {
    ETNode<ETSoundPlayer>::connect(getEntityId());
    if(autoStart) {
        ET_play();
    }
    return true;
}

void SoundPlayLogic::deinit() {
    ET_stop();
}

void SoundPlayLogic::ET_setSound(const char* newSoundName) {
    if(!newSoundName || !newSoundName[0]) {
        sound.reset();
    } else {
        ET_SendEventReturn(sound, &ETSoundManager::ET_createSound, newSoundName);
    }
}

void SoundPlayLogic::ET_play() {
    if(!sound) {
        return;
    }
    sound->play(looped);
    sound->setVolume(volume);
}

void SoundPlayLogic::ET_pause() {
    if(!sound) {
        return;
    }
    sound->pause();
}

void SoundPlayLogic::ET_resume() {
    if(!sound) {
        return;
    }
    sound->resume();
}

void SoundPlayLogic::ET_stop() {
    if(!sound) {
        return;
    }
    sound->stop();
}