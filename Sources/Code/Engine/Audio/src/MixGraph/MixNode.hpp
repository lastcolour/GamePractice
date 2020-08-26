#ifndef __MIX_NODE_HPP__
#define __MIX_NODE_HPP__

class CombineNode;
class MixGraph;
struct SourceNode;

class MixNode {
public:

    MixNode();
    virtual ~MixNode();

    virtual void additiveMixTo(float* out, int channels, int samples) = 0;

    CombineNode* getParent();
    void setParent(CombineNode* newParent);
    void setMixGraph(MixGraph* mixGraph);
    MixGraph* getMixGraph();

private:

    MixGraph* graph;
    CombineNode* parent;
};

#endif /* __MIX_NODE_HPP__ */