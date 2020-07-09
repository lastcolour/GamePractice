#ifndef __MIX_NODE_HPP__
#define __MIX_NODE_HPP__

class CombineNode;
class MixGraph;
class Resampler;
class Buffer;
struct MixConfig;

class MixNode {
public:

    MixNode();
    virtual ~MixNode();

    virtual void additiveMixTo(float* out, int channels, int samples) = 0;

    CombineNode* getParent();
    void setParent(CombineNode* newParent);
    void setMixGraph(MixGraph* mixGraph);
    const MixConfig& getMixConfig() const;
    Resampler& getResampler();
    Buffer& getTempBuffer();

private:

    MixGraph* graph;
    CombineNode* parent;
};

#endif /* __MIX_NODE_HPP__ */