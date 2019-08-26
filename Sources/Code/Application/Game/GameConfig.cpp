#include "Game/GameConfig.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/JSONNode.hpp"

namespace {

const char* GAME_CONFIG_ASSET_FILE = "Configs/GameConfig.json";
const char* GAME_CONFIG_LOCAL_FILE = "GameConfig.json";

} // namespace

GameConfig::GameConfig() {
}

GameConfig::~GameConfig() {
}

bool GameConfig::init() {
    JSONNode node;
    ET_SendEventReturn(node, &ETAssets::ET_loadLocalJSONFile, GAME_CONFIG_LOCAL_FILE);
    if(!node) {
        ET_SendEventReturn(node, &ETAssets::ET_loadJSONAsset, GAME_CONFIG_ASSET_FILE);
        if(!node) {
            LogError("[GameConfig::init] Can't load game config from asset: %s", GAME_CONFIG_ASSET_FILE);
            return false;
        }
        bool saveRes = false;
        ET_SendEventReturn(saveRes, &ETAssets::ET_saveLocalFile, GAME_CONFIG_LOCAL_FILE, node.flushToBuffer());
        if(!saveRes) {
            LogError("[GameConfig::init] Can't upack config file to: %s", GAME_CONFIG_LOCAL_FILE);
        }
    }
    readConfig(node);
    ETNode<ETGameConfig>::connect(getEntityId());
    return true;
}

void GameConfig::deinit() {
    ETNode<ETGameConfig>::disconnect();
}

void GameConfig::readConfig(const JSONNode& node) {
    {
        JSONNode gameNode = node.object("Game");
        gameNode.read("highScore", highScore);
    }
    {
        JSONNode optionsNode = node.object("Options");
        optionsNode.read("sound", soundEnabled);
        optionsNode.read("vibration", vibrationEnabled);
    }
    {
        JSONNode mainNode = node.object("Main");
        mainNode.read("mainView", mainView);
    }
}

void GameConfig::saveConfig() {
    JSONNode node;
    {
        JSONNode gameNode;
        gameNode.write("highScore", highScore);
        node.write("Game", gameNode);
    }
    {
        JSONNode optionsNode;
        optionsNode.write("sound", soundEnabled);
        optionsNode.write("vibration", soundEnabled);
        node.write("Options", optionsNode);
    }
    {
        JSONNode mainNode;
        mainNode.write("mainView", mainView);
        node.write("Main", mainNode);
    }
    bool saveRes = false;
    Buffer buff;
    ET_SendEventReturn(saveRes, &ETAssets::ET_saveLocalFile, GAME_CONFIG_LOCAL_FILE, buff);
    if(!saveRes) {
        LogError("[GameConfig::saveConfig] Can't update local config: %s", GAME_CONFIG_LOCAL_FILE);
    }
}

bool GameConfig::ET_isSoundEnabled() const {
    return soundEnabled;
}

void GameConfig::ET_setSoundEnabled(bool flag) {
    soundEnabled = flag;
    saveConfig();
}

bool GameConfig::ET_isVibrationEnabled() const {
    return vibrationEnabled;
}

void GameConfig::ET_setVibrationEnabled(bool flag) {
    vibrationEnabled = flag;
    saveConfig();
}

const char* GameConfig::ET_getMainViewName() const {
    return mainView.c_str();
}

int GameConfig::ET_getHighScore() const {
    return highScore;
}

void GameConfig::ET_setHighScore(int newHighScore) {
    highScore = newHighScore;
    saveConfig();
}