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