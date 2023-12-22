#ifndef __SOUND_EVENT_HPP__
#define __SOUND_EVENT_HPP__

#include "Reflect/Resource.hpp"

class SoundEvent {
public:

    static void Reflect(ReflectContext& ctx);

public:

    struct ResourceDescriptor {
        using RuntimeType = std::string;
        using StorageType = std::string;
        const char* TypeName = "SoundEvent";
    };

public:

    SoundEvent();
    SoundEvent(const SoundEvent& other);
    SoundEvent& operator=(const SoundEvent& other);
    ~SoundEvent();

    void emit();

private:

    Reflect::Resource<ResourceDescriptor> event;
};

#endif /* __SOUND_EVENT_HPP__ */