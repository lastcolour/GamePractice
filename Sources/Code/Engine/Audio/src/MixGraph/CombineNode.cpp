#include "MixGraph/CombineNode.hpp"

#include <algorithm>
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

void CombineNode::additiveMixTo(float* out, int channels, int samples) {
    for(auto node : children) {
        node->additiveMixTo(out, channels, samples);
    }
    children.erase(std::remove_if(children.begin(), children.end(), [](MixNode* node){
        return node->getParent() == nullptr;
    }), children.end());
}