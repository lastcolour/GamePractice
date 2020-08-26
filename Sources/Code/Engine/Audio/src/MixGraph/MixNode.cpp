#include "MixGraph/MixGraph.hpp"

#include <cassert>

MixNode::MixNode() :
    graph(nullptr),
    parent(nullptr) {
}

MixNode::~MixNode() {
}

CombineNode* MixNode::getParent() {
    return parent;
}

void MixNode::setParent(CombineNode* newParent) {
    parent = newParent;
}

void MixNode::setMixGraph(MixGraph* mixGraph) {
    assert(mixGraph && "Invalid mix graph");
    graph = mixGraph;
}

MixGraph* MixNode::getMixGraph() {
    return graph;
}