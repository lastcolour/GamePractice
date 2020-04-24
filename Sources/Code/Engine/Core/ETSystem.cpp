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

bool ETSystem::isActiveConnectionExist(TypeId etId, ConnectionArrayT*& connectionArray) {
    auto it = activeConnection.find(etId);
    if(it != activeConnection.end()) {
        connectionArray = &(it->second);
        return true;
    }
    return false;
}

void ETSystem::handleConnectionRequest(const ETSystem::ETConnectionRequest& connReq) {
    std::lock_guard<std::recursive_mutex> lock(pendingConnMutex);
    if(!connReq.isDisconnect) {
        if(isConnectRequestedDisconnect(*connReq.conn.node)) {
            pendingConnection.push_back(connReq);
            return;
        } else {
            syncRoute.pushRoute(connReq.etId); 
            bool res = isDoubleConnect(connReq.etId, *connReq.conn.node);
            syncRoute.popRoute();
            if(res) {
                return;
            }
        }
    } else {
        bool needContinue = false;
        syncRoute.pushRoute(connReq.etId);
        auto it = activeConnection.find(connReq.etId);
        if(it != activeConnection.end()) {
            auto& connections = it->second;
            for(auto& currConn: connections) {
                if(currConn.node == connReq.conn.node) {
                    currConn.addressId = InvalidEntityId;
                    needContinue = true;
                }
            }
        }
        syncRoute.popRoute();
        if(!needContinue) {
            return;
        }
    }

    if(syncRoute.tryBlockRoute(connReq.etId)) {
        registerConnection(connReq);
        syncRoute.unlockRoute(connReq.etId);
    } else {
        pendingConnection.push_back(connReq);
    }
}

void ETSystem::registerConnection(const ETConnectionRequest& connReq) {
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
    std::lock_guard<std::recursive_mutex> lock(pendingConnMutex);
    auto it = pendingConnection.begin();
    while(it != pendingConnection.end()) {
        if(syncRoute.tryBlockRoute(it->etId)) {
            registerConnection(*it);
            syncRoute.unlockRoute(it->etId);
            it = pendingConnection.erase(it);
        } else {
            ++it;
        }
    }
}

bool ETSystem::isConnectRequestedDisconnect(const ETNodeBase& node) const {
    for(auto& req : pendingConnection) {
        if(req.isDisconnect && req.conn.node == &node) {
            return true;
        }
    }
    return false;
}

bool ETSystem::isDoubleConnect(TypeId etId, const ETNodeBase& node) const {
    auto it = activeConnection.find(etId);
    if(it != activeConnection.end()) {
        const auto& connections = it->second;
        for(const auto& currConn: connections) {
            if(currConn.node == &node) {
                return true;
            }
        }
    }
    return false;
}