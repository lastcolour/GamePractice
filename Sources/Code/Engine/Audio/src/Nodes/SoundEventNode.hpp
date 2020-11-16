#ifndef __SOUND_EVENT_NODE_HPP__
#define __SOUND_EVENT_NODE_HPP__

#include "Core/ETPrimitives.hpp"
#include "Nodes/ETSoundNode.hpp"
#include "Core/TimePoint.hpp"
#include "Core/Buffer.hpp"
#include "SoundStream.hpp"

class SoundEventInfo;

class SoundEventNode : public SoundStream,
    public ETNode<ETSoundEventNode> {
public:

    SoundEventNode(EntityId soundNodeId, SoundEventInfo* eventInfo);
    virtual ~SoundEventNode();

    EntityId getNodeId() const;

    // SoundStream
    unsigned int getSamplesOffset() const override;
    float getMixVolume() const override;
    bool isEvent() const override;
    bool isMixLooped() const override;
    Buffer& getData() override;
    void onAttachToMixNode(SourceNode* node) override;
    void onDetachFromMixNode(int newSampleOffset) override;
    ESoundGroup getGroup() const override;

    // ETSoundEventNode
    void ET_emit() override;

private:

    EntityId nodeId;
    SoundEventInfo* info;
    TimePoint lastPlayTime;
};

#endif /* __SOUND_EVENT_NODE_HPP__ */