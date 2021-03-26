#ifndef __SOUND_EVENT_HPP__
#define __SOUND_EVENT_HPP__

class SoundEvent {
public:

    static void Reflect(ReflectContext& ctx);

public:

    SoundEvent();
    SoundEvent(const SoundEvent& other);
    SoundEvent& operator=(const SoundEvent& other);
    ~SoundEvent();

    void emit();

private:

    std::string eventName;
};

#endif /* __SOUND_EVENT_HPP__ */