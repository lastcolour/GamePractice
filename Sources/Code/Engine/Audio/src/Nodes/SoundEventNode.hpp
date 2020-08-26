#ifndef __SOUND_EVENT_NODE_HPP__
#define __SOUND_EVENT_NODE_HPP__

#include "Core/ETPrimitives.hpp"
#include "Nodes/ETSoundNode.hpp"
#include "Core/TimePoint.hpp"
#include "Core/Buffer.hpp"
#include "SoundStream.hpp"

class SoundEventNode : public SoundStream,
    public ETNode<ETSoundEventNode> {
public:

    SoundEventNode(EntityId soundNodeId, const std::string& soundFileName, float eventVolue, float eventDelay);
    virtual ~SoundEventNode();

    EntityId getNodeId() const;
    const std::string& getSoundName() const;
    void setSoundData(Buffer& buffer);

    // SoundStream
    unsigned int getSamplesOffset() const override;
    float getMixVolume() const override;
    bool isEvent() const override;
    bool isMixLooped() const override;
    Buffer& getData() override;
    void onAttachToMixNode(SourceNode* node) override;
    void onDetachFromMixNode(int newSampleOffset) override;

    // ETSoundEventNode
    void ET_emit() override;

private:

    EntityId nodeId;
    std::string soundName;
    Buffer soundData;
    float volume;
    float nextDelay;
    TimePoint lastPlayTime;
};

#endif /* __SOUND_EVENT_NODE_HPP__ */