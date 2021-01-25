#ifndef __MIX_NODE_HPP__
#define __MIX_NODE_HPP__

class CombineNode;
class MixGraph;

class MixNode {
public:

    MixNode(MixGraph* mixGraph);
    virtual ~MixNode();

    virtual void additiveMixTo(float* out, int channels, int samples) = 0;

    CombineNode* getParent();
    void setParent(CombineNode* newParent);
    MixGraph* getMixGraph();

private:

    MixGraph* graph;
    CombineNode* parent;
};

#endif /* __MIX_NODE_HPP__ */