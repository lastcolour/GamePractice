#ifndef __SOUND_NODE_HPP__
#define __SOUND_NODE_HPP__

#include "Core/ETPrimitives.hpp"
#include "Nodes/ETSoundNode.hpp"
#include "Core/Buffer.hpp"
#include "SoundStream.hpp"

class SoundNode : public ETNode<ETSoundNode>,
    public SoundStream {
public:

    SoundNode(EntityId soundNodeId);
    ~SoundNode();

    void setData(Buffer& data);
    EntityId getNodeId() const;

    // SoundStream
    unsigned int getSamplesOffset() const override;
    void setMixNode(SourceNode* node) override;
    bool isMixLooped() const override;
    float getMixVolume() const override;
    bool isEvent() const override;
    Buffer& getData() override;

    // ETSoundNode
    void ET_play() override;
    void ET_stop() override;
    void ET_pause() override;
    void ET_resume() override;
    void ET_setLooped(bool flag) override;
    void ET_setVolume(float newVolume) override;
    bool ET_isPlaying() const override;
    bool ET_isPaused() const override;

private:

    EntityId nodeId;
    Buffer soundData;
    SourceNode* mixNode;
    float volume;
    unsigned int samplesOffset;
    bool looped;
};

#endif /* __SOUND_NODE_HPP__ */