#include "Core/ETSystem.hpp"

#include <cassert>

ETSystem* ET_SYSTEM = nullptr;

ETSystem::ETSystem() :
    entityIdGen() {
    if(!ET_SYSTEM) {
        ET_SYSTEM = this;
    } else {
        assert(false && "Already exist other ETSystem");
    }
}

ETSystem::~ETSystem() {
    if(this == ET_SYSTEM) {
        ET_SYSTEM = nullptr;
    }
}

ETSystem* GetETSystem() {
    return ET_SYSTEM;
}

bool ETSystem::isRouteSafe(TypeId etId) const {
    for(auto elem : activeRoute) {
        if (elem == etId) {
            return false;
        }
    }
    return true;
}

void ETSystem::updateConnections(const ETConnectionRequest& connReq) {
    if(connReq.isDisconnect) {
        auto it = activeConnection.find(connReq.etId);
        if(it == activeConnection.end()) {
            return;
        }
        auto& etConnection = it->second;
        for(auto etIt = etConnection.begin(), etEnd = etConnection.end(); etIt != etEnd; ++etIt) {
            if(etIt->node == connReq.conn.node) {
                etConnection.erase(etIt);
                break;
            }
        }
        if(etConnection.empty()) {
            activeConnection.erase(it);
        }
    } else {
        auto& connections = activeConnection[connReq.etId];
        connections.push_back(connReq.conn);
    }
}

void ETSystem::updatePendingConnections() {
    auto it = pendingConnection.begin();
    while(it != pendingConnection.end()) {
        if(isRouteSafe(it->etId)) {
            updateConnections(*it);
            it = pendingConnection.erase(it);
        } else {
            ++it;
        }
    }
}
