#ifndef __SOUND_NODE_HPP__
#define __SOUND_NODE_HPP__

#include "Nodes/ETSoundNode.hpp"
#include "SoundStream.hpp"

class SoundNode : public ETNode<ETSoundNode>,
    public SoundStream {
public:

    SoundNode(EntityId soundNodeId);
    ~SoundNode();

    void setSoundData(Buffer& buffer);
    EntityId getNodeId() const;

    // SoundStream
    unsigned int getSamplesOffset() const override;
    bool isMixLooped() const override;
    float getMixVolume() const override;
    bool isEvent() const override;
    Buffer& getData() override;
    void onAttachToMixNode(SourceNode* node) override;
    void onDetachFromMixNode(int newSampleOffset) override;
    ESoundGroup getGroup() const override;

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
    bool dataIsSet;
};

#endif /* __SOUND_NODE_HPP__ */