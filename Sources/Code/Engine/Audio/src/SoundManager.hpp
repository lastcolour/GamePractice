#ifndef __SOUND_MANAGER_HPP__
#define __SOUND_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Audio/ETSound.hpp"
#include "Core/ETPrimitives.hpp"
#include "Core/Buffer.hpp"
#include "Nodes/ETSoundNodeManager.hpp"

#include <unordered_map>
#include <vector>

class SoundEventNode;
class SoundNode;

class SoundManager : public SystemLogic,
    public ETNode<ETSoundManager>,
    public ETNode<ETSoundNodeManager> {
public:

    SoundManager();
    virtual ~SoundManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETSoundManager
    Sound ET_createSound(const char* soundName) override;
    SoundEvent ET_createEvent(const char* soundName) override;

    // ETSoundNodeManager
    void ET_initSoundNode(SoundNode* node, std::string soundName) override;
    void ET_removeSoundNode(EntityId nodeId) override;
    Buffer ET_loadSoundBuffer(const char* name) override;
    void ET_loadSoundEventsBuffers() override;

private:

    void loadSoundEventsTable();

private:

    std::unordered_map<std::string, Buffer> buffers;
    std::unordered_map<std::string, std::unique_ptr<SoundEventNode>> eventMap;
    std::unordered_map<EntityId, std::unique_ptr<SoundNode>> soundNodes;
};

#endif /* __SOUND_MANAGER_HPP__ */