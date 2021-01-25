#ifndef __COMBINE_NODE_HPP__
#define __COMBINE_NODE_HPP__

#include "MixGraph/MixNode.hpp"
#include "Filters/Equalizer.hpp"

#include <vector>

class CombineNode : public MixNode {
public:

    CombineNode(MixGraph* mixGraph);
    virtual ~CombineNode();

    void addChild(MixNode* node);
    void removeChild(MixNode* node);
    void setEqualizerSetup(const EqualizerSetup& eqSetup);

    // MixNode
    void additiveMixTo(float* out, int channels, int samples) override;

private:

    void removeNullChildren();

private:

    std::vector<MixNode*> children;
    Equalizer equalizer;
    bool isMixing;
};

#endif /* __COMBINE_NODE_HPP__ */