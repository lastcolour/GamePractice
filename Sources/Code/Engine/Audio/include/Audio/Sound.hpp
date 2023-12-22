#ifndef __SOUND_HPP__
#define __SOUND_HPP__

#include "Reflect/Resource.hpp"

class SoundData;
class SoundProxy;

enum class ESoundGroup {
    Music = 0,
    Game,
    UI
};

class Sound {
public:

    struct SoundResourceDescriptor {
        using StorageType = std::string;
        using RuntimeType = SoundProxy*;
        const char* TypeName = "Sound";

        static void Convert(const std::string& fileName, SoundProxy*& proxy);
        static void Convert(SoundProxy* const& proxy, std::string& fileName);
    };

    class SoundResource : private Reflect::Resource<SoundResourceDescriptor> {
    public:

        using SpecType = SoundResourceDescriptor;

    public:

        SoundResource() {
            setAndReset(nullptr);
        }

        SoundResource(SoundResource&& other) {
            set(other.get());
            other.set(nullptr);
        }

        SoundResource& operator=(SoundResource&& other) {
            if(this != &other) {
                detach();
                set(other.get());
                other.setAndReset(nullptr);
            }
            return *this;
        }

        ~SoundResource();

        void detach();
        void setFile(const std::string& filePath);

        SoundProxy* operator->() { return get(); }
        const SoundProxy* operator->() const { return get(); }
        SoundProxy* getProxy() { return get(); }
        operator bool() const { return isSet() && get(); }
    };

public:

    static void Reflect(ReflectContext& ctx);

public:

    Sound();
    Sound(Sound&& other);
    Sound& operator=(Sound&& other);
    ~Sound();

    void fadeInPlay(float duration);
    void fadeOutStop(float duration);
    void play();
    void stop();
    void pause();
    void resume();
    void setLooped(bool flag);
    bool getLooped() const;
    void setVolume(float newVolume);
    float getVolume() const;
    bool isPlaying() const;
    bool isLooped() const;
    ESoundGroup getGroup() const;
    void setGroup(ESoundGroup newGroup);
    void setFile(const char* fileName);
    const char* getFile() const;
    void setKeepLoaded(bool flag);
    SoundProxy* getProxy();

private:

    Sound(const Sound&) = delete;
    Sound& operator=(const Sound& other) = delete;

private:

    SoundResource resource;
    float volume;
    ESoundGroup group;
    bool looped;
};

#endif /* __SOUND_HPP__ */