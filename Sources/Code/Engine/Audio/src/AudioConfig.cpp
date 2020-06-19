#include "AudioConfig.hpp"
#include "Reflect/ReflectContext.hpp"

AudioConfig::AudioConfig() :
    maxSoundSources(32) {
}

AudioConfig::~AudioConfig() {
}

void AudioConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<AudioConfig>("AudioConfig")) {
        classInfo->addField("max_sound_sources", &AudioConfig::maxSoundSources);
    }
}