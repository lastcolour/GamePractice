#include "MixGraph/CombineNode.hpp"

#include <cassert>

CombineNode::CombineNode() {
}

CombineNode::~CombineNode() {
}

void CombineNode::addChild(MixNode* node) {
    assert(node && "invalid child node");
    node->setParent(this);
    children.push_back(node);
}

void CombineNode::removeChild(MixNode* node) {
    auto it = std::find(children.begin(), children.end(), node);
    if(it != children.end()) {
        (*it)->setParent(nullptr);
        children.erase(it);
    } else {
        assert(false && "Can't find child to remove");
    }
}

void CombineNode::additiveMixTo(float* out, int channels, int samples) {
    for(auto node : children) {
        node->additiveMixTo(out, channels, samples);
    }
}