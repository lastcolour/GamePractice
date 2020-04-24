#include "Audio/AudioConfig.hpp"
#include "Core/JSONNode.hpp"

AudioConfig::AudioConfig() : maxSoundSources(32) {
}

AudioConfig::~AudioConfig() {
}

void AudioConfig::serialize(const JSONNode& node) {
    node.read("max_sound_sources", maxSoundSources);
}