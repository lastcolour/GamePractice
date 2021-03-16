#ifndef __SOUND_EVENT_HPP__
#define __SOUND_EVENT_HPP__

class SoundEvent {
public:

    SoundEvent();
    SoundEvent(EntityId soundNodeId);
    SoundEvent(const SoundEvent& other);
    SoundEvent& operator=(const SoundEvent& other);
    ~SoundEvent();

    // Sound
    void emit();
    bool isValid() const;

private:

    EntityId nodeId;
};

#endif /* __SOUND_EVENT_HPP__ */