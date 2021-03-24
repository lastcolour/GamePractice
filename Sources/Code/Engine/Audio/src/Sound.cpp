#include "Audio/Sound.hpp"
#include "Audio/ETSoundManagers.hpp"
#include "Reflect/EnumInfo.hpp"
#include "SoundDataManager.hpp"
#include "SoundProxy.hpp"

#include <cassert>

void Sound::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<ESoundGroup>("ESoundGroup")) {
        enumInfo->addValues<ESoundGroup>({
            {"Game", ESoundGroup::Game},
            {"Music", ESoundGroup::Music},
            {"UI", ESoundGroup::UI}
        });
    }
    if(auto classInfo = ctx.classInfo<Sound>("Sound")) {
        classInfo->addResourceField("file", ResourceType::Sound, &Sound::setFile);
        classInfo->addField("volume", &Sound::volume);
        classInfo->addField("loop", &Sound::looped);
        classInfo->addField("group", &Sound::group);
    }
}

Sound::Sound() :
    proxy(nullptr),
    volume(1.f),
    group(ESoundGroup::Music),
    looped(false) {

    ET_SendEventReturn(proxy, &ETSoundDataManager::ET_createSoundProxy);
    assert(proxy && "Can't create sound proxy");
}

Sound::Sound(Sound&& other) :
    proxy(other.proxy),
    volume(other.volume),
    group(other.group),
    looped(other.looped) {

    other.proxy = nullptr;
}

Sound& Sound::operator=(Sound&& other) {
    if(this != &other) {
        volume = other.volume;
        looped = other.looped;
        group = other.group;
        proxy->setNoSound();
        proxy = other.proxy;
        other.proxy = nullptr;
    }
    return *this;
}

Sound::~Sound() {
    if(proxy) {
        proxy->setNoSound();
        proxy = nullptr;
    }
}

void Sound::fadeInPlay(float duration) {
    proxy->fadeInPlay(*this, duration);
}

void Sound::fadeOutStop(float duration) {
    proxy->fadeOutStop(duration);
}

void Sound::play() {
    proxy->play(*this);
}

void Sound::stop() {
    proxy->stop();
}

void Sound::pause() {
    proxy->pause();
}

void Sound::resume() {
    proxy->resume(*this);
}

void Sound::setLooped(bool flag) {
    looped = flag;
    proxy->writeLooped(flag);
}

bool Sound::isLooped() const {
    return looped;
}

void Sound::setVolume(float newVolume) {
    volume = newVolume;
    proxy->writeVolume(volume);
}

float Sound::getVolume() const {
    return volume;
}

bool Sound::isPlaying() const {
    return proxy->isPlaying();
}

bool Sound::isValid() const {
    return proxy->isValid();
}

ESoundGroup Sound::getGroup() const {
    return group;
}

void Sound::setGroup(ESoundGroup newGroup) {
    if(group == newGroup) {
        return;
    }
    group = newGroup;
    proxy->writeGroup(group);
}

void Sound::setFile(const char* fileName) {
    proxy->writeFile(fileName);
}

const char* Sound::getFile() const {
    return proxy->readFile();
}