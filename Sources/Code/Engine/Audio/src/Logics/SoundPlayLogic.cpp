#include "Logics/SoundPlayLogic.hpp"
#include "Reflect/ReflectContext.hpp"

void SoundPlayLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<SoundPlayLogic>("SoundPlayer")) {
        classInfo->addField("looped", &SoundPlayLogic::looped);
        classInfo->addField("volume", &SoundPlayLogic::volume);
        classInfo->addResourceField("sound", ResourceType::Sound, &SoundPlayLogic::setSound);
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

void SoundPlayLogic::init() {
    ETNode<ETSoundPlayer>::connect(getEntityId());
    if(autoStart) {
        ET_play();
    }
}

void SoundPlayLogic::deinit() {
    ET_stop();
}

void SoundPlayLogic::ET_play() {
    sound.setLooped(looped);
    sound.setVolume(volume);
    sound.play();
}

void SoundPlayLogic::ET_stop() {
    sound.stop();
}

void SoundPlayLogic::setSound(const char* soundName) {
    ET_SendEventReturn(sound, &ETSoundManager::ET_createSound, soundName);
}