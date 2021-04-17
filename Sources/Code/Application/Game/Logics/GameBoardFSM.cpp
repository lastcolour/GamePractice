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

bool shouldReturnToPass(EGameBoardUpdatePass currentPass) {
    return currentPass == EGameBoardUpdatePass::Move
        || currentPass == EGameBoardUpdatePass::Merge
        || currentPass == EGameBoardUpdatePass::Trigger;
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
    updatePass(EGameBoardUpdatePass::Static),
    returnPass(EGameBoardUpdatePass::Static),
    isStaticWasVisited(false) {
}

GameBoardFSM::~GameBoardFSM() {
}

bool GameBoardFSM::queryPass(EGameBoardUpdatePass& outPass) {
    updatePass = returnPass;
    switch(updatePass) {
        case EGameBoardUpdatePass::Static: {
            if(state.hasTriggeringElems) {
                updatePass = EGameBoardUpdatePass::Trigger;
            } else if(state.hasMergingElems) {
                updatePass = EGameBoardUpdatePass::Merge;
            } else if(state.hasMovingElems) {
                updatePass = EGameBoardUpdatePass::Move;
            } else if(state.isMatchRequested) {
                if(!state.isMatchBlocked) {
                    updatePass = EGameBoardUpdatePass::Match;
                }
            } else if(state.isRespawnRequested) {
                updatePass = EGameBoardUpdatePass::Respawn;
            } else if(updatePass == EGameBoardUpdatePass::Static) {
                if(isStaticWasVisited) {
                    return false;
                } else {
                    isStaticWasVisited = true;
                }
            }
            break;
        }
        case EGameBoardUpdatePass::Move: {
            if(state.hasMovingElems) {
                updatePass = EGameBoardUpdatePass::Move;
            } else {
                updatePass = EGameBoardUpdatePass::Static;
                returnPass = updatePass;
                return queryPass(outPass);
            }
            break;
        }
        case EGameBoardUpdatePass::Merge: {
            if(state.hasMergingElems) {
                updatePass = EGameBoardUpdatePass::Merge;
            } else {
                updatePass = EGameBoardUpdatePass::Static;
                returnPass = updatePass;
                return queryPass(outPass);
            }
            break;
        }
        case EGameBoardUpdatePass::Trigger: {
            if(state.hasTriggeringElems) {
                updatePass = EGameBoardUpdatePass::Trigger;
            } else {
                updatePass = EGameBoardUpdatePass::Static;
                returnPass = updatePass;
                return queryPass(outPass);
            }
        }
        case EGameBoardUpdatePass::Match: {
            if(!state.isMatchRequested || state.isMatchBlocked) {
                updatePass = EGameBoardUpdatePass::Static;
                returnPass = updatePass;
                return queryPass(outPass);
            }
            break;
        }
        case EGameBoardUpdatePass::Respawn: {
            updatePass = EGameBoardUpdatePass::Static;
            returnPass = updatePass;
            return queryPass(outPass);
        }
        default: {
            assert(false && "Invalid update pass");
        }
    }

    returnPass = updatePass;
    outPass = updatePass;

    if(outPass != EGameBoardUpdatePass::Static) {
        isStaticWasVisited = false;
    }

    return true;
}

bool GameBoardFSM::querySubPass(EGameBoardUpdatePass& outPass) {
    EGameBoardUpdatePass resPass = EGameBoardUpdatePass::Static;
    switch(updatePass) {
        case EGameBoardUpdatePass::Static: {
            return false;
        }
        case EGameBoardUpdatePass::Move: {
            if(!tryGetSubPass(state, resPass)) {
                return false;
            }
            break;
        }
        case EGameBoardUpdatePass::Merge: {
            if(!tryGetSubPass(state, resPass)) {
                return false;
            }
            break;
        }
        case EGameBoardUpdatePass::Trigger: {
            if(!tryGetSubPass(state, resPass)) {
                return false;
            }
            break;
        }
        case EGameBoardUpdatePass::Match: {
            if(!state.isRespawnRequested) {
                return false;
            }
            resPass = EGameBoardUpdatePass::Respawn;
            break;
        }
        case EGameBoardUpdatePass::Respawn: {
            return false;
        }
        default: {
            assert(false && "Invalid update pass");
        }
    }

    if(shouldReturnToPass(updatePass)) {
        returnPass = updatePass;
    } else {
        returnPass = resPass;
    }
    updatePass = resPass;

    outPass = resPass;
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