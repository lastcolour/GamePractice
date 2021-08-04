#ifndef __BRON_KERBOSCH_HPP__
#define __BRON_KERBOSCH_HPP__

#include "Core/TimePoint.hpp"

class AdjacencyMatrix {
public:

    AdjacencyMatrix();
    AdjacencyMatrix(int numVertecies);

    ~AdjacencyMatrix();

    void addEdge(int a, int b);
    bool hasEdge(int a, int b) const;

    int getVertexCost(int a) const;
    void setVertexCost(int a, int cost);
    int vertexCount() const;
    int edgeCount() const;

    void reset(int numVertecies);

private:

    AdjacencyMatrix(const AdjacencyMatrix&) = delete;
    AdjacencyMatrix& operator=(const AdjacencyMatrix&) = delete;

private:

    std::vector<int> data;
    int vertNum;
    int edgeNum;
};

struct BronKerboschCache;

struct BronKerboschRequest {
public:

    BronKerboschRequest();
    ~BronKerboschRequest();

public:

    AdjacencyMatrix adjMat;
    std::vector<int> result;
    BronKerboschCache* cache;
    int currStopIterCount;
    int stopIterCount;
    int maxCost;
    int iterCount;
    int maxDepth;
    float duration;
    bool stopBecauseLimit;

private:

    BronKerboschRequest(const BronKerboschRequest&) = delete;
    BronKerboschRequest& operator=(const BronKerboschRequest&) = delete;
};

void BronKerbosch(BronKerboschRequest& req);

#endif /* __BRON_KERBOSCH_HPP__ */