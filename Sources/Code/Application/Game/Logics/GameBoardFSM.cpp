#include "Game/Logics/GameBoardFSM.hpp"

#include <cassert>

namespace {

bool canTransitToMatchState(const GameBoardState& state) {
    if(state.hasMovingElems) {
        return false;
    }
    if(state.hasMergingElems) {
        return false;
    }
    if(state.hasTriggeringElems) {
        return false;
    }
    if(!state.isMatchRequested || state.isMatchBlocked) {
        return false;
    }
    return true;
}

bool tryGetSubPass(const GameBoardState& state, EGameBoardUpdatePass& outPass) {
    if(canTransitToMatchState(state)) {
        outPass = EGameBoardUpdatePass::Match;
        return true;
    }
    if(state.isRespawnRequested) {
        outPass = EGameBoardUpdatePass::Respawn;
        return true;
    }
    return false;
}

} // namespace

GameBoardState::GameBoardState() :
    hasMovingElems(false),
    hasMergingElems(false),
    hasTriggeringElems(false),
    isMatchRequested(false),
    isMatchBlocked(false),
    isRespawnRequested(false) {
}

GameBoardFSM::GameBoardFSM() :
    updatePass(EGameBoardUpdatePass::Static) {
}

GameBoardFSM::~GameBoardFSM() {
}

bool GameBoardFSM::queryPass(EGameBoardUpdatePass& outPass) {
    switch(updatePass) {
        case EGameBoardUpdatePass::Static: {
            if(state.hasMovingElems) {
                updatePass = EGameBoardUpdatePass::Move;
            } else if(state.hasMergingElems) {
                updatePass = EGameBoardUpdatePass::Merge;
            } else if(state.hasTriggeringElems) {
                updatePass = EGameBoardUpdatePass::Trigger;
            } else if(state.isMatchRequested) {
                if(!state.isMatchBlocked) {
                    updatePass = EGameBoardUpdatePass::Match;
                }
            } else if(state.isRespawnRequested) {
                updatePass = EGameBoardUpdatePass::Respawn;
            } else if(updatePass == EGameBoardUpdatePass::Static) {
                return false;
            }
            break;
        }
        case EGameBoardUpdatePass::Move: {
            if(state.hasMovingElems) {
                updatePass = EGameBoardUpdatePass::Move;
            } else {
                updatePass = EGameBoardUpdatePass::Static;
                return queryPass(outPass);
            }
            break;
        }
        case EGameBoardUpdatePass::Merge: {
            if(state.hasMergingElems) {
                updatePass = EGameBoardUpdatePass::Merge;
            } else {
                updatePass = EGameBoardUpdatePass::Static;
                return queryPass(outPass);
            }
            break;
        }
        case EGameBoardUpdatePass::Trigger: {
            return false;
        }
        case EGameBoardUpdatePass::Match: {
            if(!state.isMatchRequested || state.isMatchBlocked) {
                updatePass = EGameBoardUpdatePass::Static;
                return queryPass(outPass);
            }
            break;
        }
        case EGameBoardUpdatePass::Respawn: {
            updatePass = EGameBoardUpdatePass::Static;
            return queryPass(outPass);
        }
        default: {
            assert(false && "Invalid update pass");
        }
    }
    outPass = updatePass;
    return true;
}

bool GameBoardFSM::querySubPass(EGameBoardUpdatePass& outPass) {
    switch(updatePass) {
        case EGameBoardUpdatePass::Static: {
            return false;
        }
        case EGameBoardUpdatePass::Move: {
            if(!tryGetSubPass(state, updatePass)) {
                return false;
            }
            outPass = updatePass;
            break;
        }
        case EGameBoardUpdatePass::Merge: {
            if(!tryGetSubPass(state, updatePass)) {
                return false;
            }
            outPass = updatePass;
            break;
        }
        case EGameBoardUpdatePass::Trigger: {
            if(!tryGetSubPass(state, updatePass)) {
                return false;
            }
            outPass = updatePass;
            break;
        }
        case EGameBoardUpdatePass::Match: {
            if(!state.isRespawnRequested) {
                return false;
            }
            updatePass = EGameBoardUpdatePass::Respawn;
            break;
        }
        case EGameBoardUpdatePass::Respawn: {
            return false;
        }
        default: {
            assert(false && "Invalid update pass");
        }
    }

    outPass = updatePass;
    return true;
}

GameBoardState& GameBoardFSM::getState() {
    return state;
}

const GameBoardState& GameBoardFSM::getState() const {
    return state;
}

bool GameBoardFSM::canQueryPass() const {
    return state.hasMovingElems
        || state.hasMergingElems
        || state.hasTriggeringElems
        || state.isMatchRequested
        || state.isRespawnRequested;
}