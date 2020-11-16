#ifndef __SOUND_NODE_MANAGER_HPP__
#define __SOUND_NODE_MANAGER_HPP__

#include "Core/Buffer.hpp"

class SoundNode;

struct ETSoundNodeManager {
    virtual ~ETSoundNodeManager() = default;
    virtual void ET_loadSoundEventsBuffers() = 0;
    virtual void ET_initSoundNode(SoundNode* node, std::string soundName) = 0;
    virtual void ET_removeSoundNode(EntityId nodeId) = 0;
};

#endif /* __SOUND_NODE_MANAGER_HPP__ */