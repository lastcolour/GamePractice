#ifndef __COMBINE_NODE_HPP__
#define __COMBINE_NODE_HPP__

#include "MixGraph/MixNode.hpp"

#include <vector>

class CombineNode : public MixNode {
public:

    CombineNode();
    virtual ~CombineNode();

    void addChild(MixNode* node);
    void removeChild(MixNode* node);

    // MixNode
    void additiveMixTo(float* out, int channels, int samples) override;

private:

    std::vector<MixNode*> children;
};

#endif /* __COMBINE_NODE_HPP__ */