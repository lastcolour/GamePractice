#include "SoundProxy.hpp"

SoundProxy::SoundProxy() {
}

SoundProxy::~SoundProxy() {
}

ESoundGroup SoundProxy::readGroup() const {
    return ESoundGroup::Game;
}

SoundData* SoundProxy::readData() {
    return nullptr;
}

float SoundProxy::readVolume() const {
    return 0.f;
}

bool SoundProxy::readLooped() const {
    return false;
}

int SoundProxy::readOffset() const {
    return 0;
}

void SoundProxy::writeOffset(int newOffset) {
}