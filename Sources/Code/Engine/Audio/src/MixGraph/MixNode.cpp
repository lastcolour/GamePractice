#include "MixGraph/MixGraph.hpp"

#include <cassert>

MixNode::MixNode(MixGraph* mixGraph) :
    graph(mixGraph),
    parent(nullptr) {

    assert(mixGraph && "Invalid mix graph");
}

MixNode::~MixNode() {
}

CombineNode* MixNode::getParent() {
    return parent;
}

void MixNode::setParent(CombineNode* newParent) {
    parent = newParent;
}

MixGraph* MixNode::getMixGraph() {
    return graph;
}