#include "Logics/SoundPlayLogic.hpp"
#include "Reflect/ReflectContext.hpp"

void SoundPlayLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<SoundPlayLogic>("SoundPlayer")) {
        classInfo->addField("looped", &SoundPlayLogic::looped);
        classInfo->addField("volume", &SoundPlayLogic::volume);
        classInfo->addResourceField("sound", &SoundPlayLogic::setSound);
        classInfo->addField("autoStart", &SoundPlayLogic::autoStart);
    }
}

SoundPlayLogic::SoundPlayLogic() :
    volume(1.f),
    looped(false),
    autoStart(false) {
}

SoundPlayLogic::~SoundPlayLogic() {
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

void SoundPlayLogic::ET_play() {
    if(sound) {
        sound->setLooped(looped);
        sound->setVolume(volume);
        sound->play();
    }
}

void SoundPlayLogic::ET_stop() {
    if(sound) {
        sound->stop();
    }
}

void SoundPlayLogic::setSound(const char* soundName) {
    ET_SendEventReturn(sound, &ETSoundManager::ET_createSound, soundName);
}