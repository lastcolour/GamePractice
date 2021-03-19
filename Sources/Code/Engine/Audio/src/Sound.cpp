#include "Audio/Sound.hpp"
#include "Audio/ETSoundManagers.hpp"
#include "Reflect/EnumInfo.hpp"
#include "SoundDataManager.hpp"

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
    //assert(proxy && "Can't create sound proxy");
}

Sound::Sound(Sound&& other) :
    proxy(other.proxy),
    data(std::move(other.data)),
    volume(other.volume),
    group(other.group),
    looped(other.looped) {

    other.proxy = nullptr;
}

Sound& Sound::operator=(Sound&& other) {
    if(this != &other) {
        stop();
        volume = other.volume;
        looped = other.looped;
        group = other.group;
        data = std::move(other.data);
        ET_SendEvent(&ETSoundDataManager::ET_removeSoundProxy, proxy);
        proxy = other.proxy;
        other.proxy = nullptr;
    }
    return *this;
}

Sound::~Sound() {
    if(proxy) {
        ET_SendEvent(&ETSoundDataManager::ET_removeSoundProxy, proxy);
        proxy = nullptr;
    }
    stop();
}

void Sound::play() {
    if(!data) {
        return;
    }
}

void Sound::stop() {
}

void Sound::pause() {
}

void Sound::resume() {
}

void Sound::setLooped(bool flag) {
    looped = flag;
}

bool Sound::isLooped() const {
    return looped;
}

void Sound::setVolume(float newVolume) {
    volume = newVolume;
}

float Sound::getVolume() const {
    return volume;
}

bool Sound::isPlaying() const {
    return false;
}

bool Sound::isPaused() const {
    return false;
}

bool Sound::isValid() const {
    return false;
}

ESoundGroup Sound::getGroup() const {
    return group;
}

void Sound::setGroup(ESoundGroup newGroup) {
    if(group == newGroup) {
        return;
    }
    group = newGroup;
    if(isPlaying()) {
        pause();
        resume();
    }
}

void Sound::setFile(const char* fileName) {
    ET_SendEventReturn(data, &ETSoundDataManager::ET_loadSoundData, fileName);
}

const char* Sound::getFile() const {
    if(data) {
        return data->fileName;
    }
    return nullptr;
}