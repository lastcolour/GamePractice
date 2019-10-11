#include "Audio/Android/OboeAudioSystem.hpp"

OboeAudioSystem::OboeAudioSystem() {
}

OboeAudioSystem::~OboeAudioSystem() {
}

bool OboeAudioSystem::init() {
    ETNode<ETSoundSourceManager>::connect(getEntityId());
   return true;
}

void OboeAudioSystem::deinit() {
    return;
}

SoundSource* OboeAudioSystem::ET_getFreeSource() {
    return nullptr;
}

void OboeAudioSystem::ET_returnSoundSource(SoundSource* soundSoruce) {
}