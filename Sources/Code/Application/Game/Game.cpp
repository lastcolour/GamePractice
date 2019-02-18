#include "Game/Game.hpp"
#include "Game/GameObject.hpp"
#include "Render/RenderLogic.hpp"
#include "Logger.hpp"
#include "Environment.hpp"
#include "Assets.hpp"
#include "JSONNode.hpp"
#include "Render/Color.hpp"
#include "Surface.hpp"
#include "Math/Vector.hpp"
namespace {
    const char* GAME_OBJECTS = "Game/GameObjects.json";
}

Game::Game() :
    gameObjects(),
    objCount(0u) {
}

Game::~Game() {
}

bool Game::init() {
    auto surface = GetEnv()->getSurface();
    int width = surface->getWidth();
    int height = surface->getHeight();

    int totalOjb = 10;
    float xStep = static_cast<float>(width) / totalOjb;
    float yStep = static_cast<float>(height) / totalOjb;
    float offset = 5.f;

    for(int i=0; i<totalOjb; ++i) {
        for(int j=0; j<totalOjb; ++j) {
            auto obj = createObject("Simple");
            if(!obj) {
                return false;
            }
            Vec2 botLeft(xStep * i, yStep * j);
            botLeft.x += offset;
            botLeft.y += offset;
            Vec2 topRight(xStep * (i + 1), yStep * (j + 1));
            topRight.x -= offset;
            topRight.y -= offset;
            ColorF col(1.f, static_cast<float>(i / totalOjb), static_cast<float>(j / totalOjb));
            if(auto renderLogic = obj->getRender()) {
                auto& mat = renderLogic->getMaterila();
            }
            gameObjects.emplace_back(std::move(obj));
        }
    }
    return true;
}

bool Game::shouldRun() {
    return true;
}

void Game::update() {
    for(auto& obj : gameObjects) {
        obj->update();
    }
}

std::string Game::createNewNameForObject(const std::string& baseName) {
    std::string newName = StringFormat("%s_%i", baseName.empty() ? "Unknown" : baseName.c_str(), objCount);
    ++objCount;
    return newName;
}

std::unique_ptr<GameObject> Game::createObject(const std::string& objectName) {
    auto assets = GetEnv()->getAssets();
    auto buff = assets->loadAsset(GAME_OBJECTS);
    if(!buff) {
        LogWarning("[Game::createObject] Can't load game objects: %s", GAME_OBJECTS);
        return nullptr;
    }
    auto rootNode = JSONNode::ParseBuffer(buff);
    if(!rootNode) {
        LogWarning("[Game::createObject] Can't load game objects: %s", GAME_OBJECTS);
        return nullptr;
    }
    rootNode = rootNode.object("GameObjects");
    if(!rootNode) {
        LogWarning("[Game::createObject] Can't find required root node in '%s'", GAME_OBJECTS);
        return nullptr;
    }
    for(const auto& objNode : rootNode) {
        std::string baseName;
        objNode.value("name", baseName);
        if(objectName != baseName) {
            continue;
        }
        std::string objName = createNewNameForObject(baseName);
        std::unique_ptr<GameObject> objPtr(new GameObject(objName));
        auto logics = objNode.object("logics");
        if(!logics) {
            LogWarning("[Game::createObject] Skip object '%s' while it doesn't have any logic");
            continue;
        }
        for(const auto& logicNode : logics) {
            std::string logicType;
            logicNode.value("type", logicType);
            std::unique_ptr<RenderLogic> logicPtr;
            if(logicType == "RenderLogic") {
                auto logicData = logicNode.object("data");
                if(!logicData) {
                    LogWarning("[Game::createObject] Can't find logic data for '%s' for '%s'", logicType, objName);
                    continue;
                }
                logicPtr.reset(new RenderLogic);
                if(!logicPtr->init(logicData)) {
                    LogWarning("[Game::createObject] Can't init logic '%s' for '%s'", logicType, objName);
                    continue;
                }
            } else {
                LogWarning("[Game::createObject] Unknown logic type '%s' inside object '%s'", logicType, objName);
                continue;
            }
            if(!logicPtr) {
                objPtr->addRender(std::move(logicPtr));
            }
        }
        return std::move(objPtr);
    }
    LogWarning("[Game::createObject] Can't find any object with name: '%s'", objectName);
    return nullptr;
}
