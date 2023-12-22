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
        classInfo->addField("file", &Sound::resource);
        classInfo->addField("volume", &Sound::volume);
        classInfo->addField("loop", &Sound::looped);
        classInfo->addField("group", &Sound::group);
    }
}

void Sound::SoundResourceDescriptor::Convert(const std::string& fileName, SoundProxy*& proxy) {
    if(fileName.empty()) {
        if(proxy) {
            proxy->setNoSound();
            proxy = nullptr;
        }
    } else {
        if(!proxy) {
            ET_SendEventReturn(proxy, &ETSoundDataManager::ET_createSoundProxy);
        }
        if(proxy) {
            proxy->setFile(fileName.c_str());
        }
    }
}

void Sound::SoundResourceDescriptor::Convert(SoundProxy* const& proxy, std::string& fileName) {
    if(proxy) {
        fileName = proxy->getFile();
    }
}

Sound::SoundResource::~SoundResource() {
    detach();
}

void Sound::SoundResource::detach() {
    SoundProxy* proxy = get();
    if(proxy) {
        proxy->setNoSound();
        setAndReset(nullptr);
    }
}

void Sound::SoundResource::setFile(const std::string& filePath) {
    SoundResourceDescriptor::Convert(filePath, get());
    if(get()) {
        set(get());
    }
}

Sound::Sound() :
    volume(1.f),
    group(ESoundGroup::Music),
    looped(false) {
}

Sound::Sound(Sound&& other) :
    resource(std::move(other.resource)),
    volume(other.volume),
    group(other.group),
    looped(other.looped) {
}

Sound& Sound::operator=(Sound&& other) {
    if(this != &other) {
        volume = other.volume;
        looped = other.looped;
        group = other.group;
        resource = std::move(other.resource);
    }
    return *this;
}

Sound::~Sound() {
}

void Sound::fadeInPlay(float duration) {
    if(!resource) {
        return;
    }
    resource->fadeInPlay(*this, duration);
}

void Sound::fadeOutStop(float duration) {
    if(!resource) {
        return;
    }
    resource->fadeOutStop(duration);
}

void Sound::play() {
    if(!resource) {
        return;
    }
    resource->play(*this);
}

void Sound::stop() {
    if(!resource) {
        return;
    }
    resource->stop();
}

void Sound::pause() {
    if(!resource) {
        return;
    }
    resource->pause();
}

void Sound::resume() {
    if(!resource) {
        return;
    }
    resource->resume(*this);
}

void Sound::setLooped(bool flag) {
    looped = flag;
    if(!resource) {
        return;
    }
    resource->writeLooped(flag);
}

bool Sound::isLooped() const {
    return looped;
}

void Sound::setVolume(float newVolume) {
    volume = newVolume;
    if(!resource) {
        return;
    }
    resource->writeVolume(volume);
}

float Sound::getVolume() const {
    return volume;
}

bool Sound::isPlaying() const {
    if(!resource) {
        return false;
    }
    return resource->isPlaying();
}

ESoundGroup Sound::getGroup() const {
    return group;
}

void Sound::setGroup(ESoundGroup newGroup) {
    if(group == newGroup) {
        return;
    }
    group = newGroup;
    if(!resource) {
        return;
    }
    resource->writeGroup(group);
}

void Sound::setFile(const char* fileName) {
    resource.setFile(fileName);
}

const char* Sound::getFile() const {
    if(!resource) {
        return "";
    }
    return resource->getFile();
}

void Sound::setKeepLoaded(bool flag) {
    if(!resource) {
        return;
    }
    resource->setKeepLoaded(flag);
}

SoundProxy* Sound::getProxy() {
    return resource.getProxy();
}