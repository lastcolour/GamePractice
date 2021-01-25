#include "MixGraph/CombineNode.hpp"
#include "MixGraph/MixGraph.hpp"

#include <algorithm>
#include <cassert>

CombineNode::CombineNode(MixGraph* mixGraph) :
    MixNode(mixGraph),
    equalizer(&mixGraph->getMixConfig()),
    isMixing(false) {
}

CombineNode::~CombineNode() {
}

void CombineNode::addChild(MixNode* node) {
    assert(node && "invalid child node");
    node->setParent(this);
    children.push_back(node);
}

void CombineNode::setEqualizerSetup(const EqualizerSetup& eqSetup) {
    equalizer.setSetup(eqSetup);
}

void CombineNode::additiveMixTo(float* out, int channels, int samples) {
    auto& comineBuff = getMixGraph()->getCombineBuffer();

    auto comineBuffPtr = static_cast<float*>(comineBuff.getWriteData());
    std::fill_n(comineBuffPtr, channels * samples, 0.f);

    isMixing = true;
    for(auto node : children) {
        node->additiveMixTo(comineBuffPtr, channels, samples);
    }
    isMixing = false;

    equalizer.exclusiveTransform(comineBuffPtr, channels, samples);

    for(int i = 0; i < samples * channels; ++i) {
        out[i] += comineBuffPtr[i];
    }

    removeNullChildren();
}

void CombineNode::removeChild(MixNode* node) {
    assert(node && "Invalid child");
    auto it = std::find(children.begin(), children.end(), node);
    if(it == children.end()) {
        return;
    }
    (*it)->setParent(nullptr);
    if(isMixing) {
        *it = nullptr;
    } else {
        children.erase(it);
    }
}

void CombineNode::removeNullChildren() {
    children.erase(std::remove(children.begin(), children.end(), nullptr),
        children.end());
}