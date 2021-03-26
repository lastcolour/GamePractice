#include "MixGraph/SoundSource.hpp"
#include "SoundProxy.hpp"

SoundSource::SoundSource() :
    proxy(nullptr),
    isEvent(false),
    isRestOffset(false) {
}

SoundSource::SoundSource(SoundProxy& soundProxy, bool isEventFlag) :
    proxy(&soundProxy),
    isEvent(isEventFlag),
    isRestOffset(false) {
}

SoundSource::SoundSource(SoundSource&& other) :
    proxy(other.proxy),
    isEvent(other.isEvent),
    isRestOffset(other.isRestOffset) {

    other.proxy = nullptr;
    other.isRestOffset = false;
}

SoundSource& SoundSource::operator=(SoundSource&& other) {
    if(this != &other) {
        proxy = other.proxy;
        isEvent = other.isEvent;
        isRestOffset = other.isRestOffset;
        other.proxy = nullptr;
        other.isRestOffset = false;
    }
    return *this;
}

void SoundSource::setMixNode(MixNode* mixNode) {
    if(!isEvent) {
        proxy->setMixNode(mixNode);
    }
}

MixNode* SoundSource::getMixNode() {
    if(isEvent) {
        return nullptr;
    }
    return proxy->getMixNode();
}

float SoundSource::getVolume() const {
    return proxy->readVolume();
}

bool SoundSource::getLooped() const {
    if(isEvent) {
        return false;
    }
    return proxy->readLooped();
}

int SoundSource::getOffset() const {
    if(isEvent) {
        return 0;
    }
    return proxy->getOffset();
}

void SoundSource::setOffset(int newOffset) {
    if(isEvent) {
        return;
    }
    proxy->setOffset(newOffset);
}

bool SoundSource::isNull() const {
    return proxy == nullptr;
}

void SoundSource::reset() {
    if(isEvent) {
        return;
    }
    proxy->setMixNode(nullptr);
    if(isRestOffset) {
        isRestOffset = false;
        proxy->setOffset(0);
    }
}

void SoundSource::setResetOffset(bool flag) {
    if(isEvent) {
        return;
    }
    isRestOffset = flag;
}

std::shared_ptr<SoundData>& SoundSource::getData() {
    return proxy->getData();
}