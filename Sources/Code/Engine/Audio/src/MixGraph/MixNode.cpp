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

const MixConfig& MixNode::getMixConfig() const {
    assert(graph && "Invalid mix graph");
    return graph->getMixConfig();
}

Resampler& MixNode::getResampler() {
    assert(graph && "Invalid mix graph");
    return graph->getResampler();
}

Buffer& MixNode::getTempBuffer() {
    return graph->getTempBuffer();
}