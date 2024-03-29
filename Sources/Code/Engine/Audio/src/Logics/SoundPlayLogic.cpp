#include "Logics/SoundPlayLogic.hpp"

void SoundPlayLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<SoundPlayLogic>("SoundPlayer")) {
        classInfo->addField("sound", &SoundPlayLogic::sound);
        classInfo->addField("autoStart", &SoundPlayLogic::autoStart);
    }
}

SoundPlayLogic::SoundPlayLogic() :
    autoStart(false) {
}

SoundPlayLogic::~SoundPlayLogic() {
}

void SoundPlayLogic::init() {
    ETNode<ETSoundPlayer>::connect(getEntityId());
}

void SoundPlayLogic::onLoaded() {
    if(autoStart) {
        ET_play();
    }
}

void SoundPlayLogic::deinit() {
    ET_stop();
}

void SoundPlayLogic::ET_play() {
    sound.play();
}

void SoundPlayLogic::ET_stop() {
    sound.stop();
}