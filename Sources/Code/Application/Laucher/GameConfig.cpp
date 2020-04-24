#include "Laucher/GameConfig.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/JSONNode.hpp"

namespace {

const char* GAME_LOCAL_CONFIG = "Configs/GameLocal.json";
const char* GAME_LOCAL_UNPACKED_CONFIG = "GameLocal.json";

} // namespace

GameLocalConfig::GameLocalConfig() :
    highScore(0),
    soundEnabled(true),
    vibrationEnabled(true) {
}

void GameLocalConfig::read(const JSONNode& node) {
    if(auto gameNode = node.object("Game")) {
        gameNode.read("highScore", highScore);
    }
    if(auto optionsNode = node.object("Options")) {
        optionsNode.read("sound", soundEnabled);
        optionsNode.read("vibration", vibrationEnabled);
    }
}

void GameLocalConfig::save(JSONNode& node) {
    {
        JSONNode gameNode;
        gameNode.write("highScore", highScore);
        node.write("Game", gameNode);
    }
    {
        JSONNode optionsNode;
        optionsNode.write("sound", soundEnabled);
        optionsNode.write("vibration", vibrationEnabled);
        node.write("Options", optionsNode);
    }
}

GameConfig::GameConfig() {
}

GameConfig::~GameConfig() {
}

bool GameConfig::init() {
    if(!loadLocalConfig()) {
        return false;
    }
    ETNode<ETGameConfig>::connect(getEntityId());
    return true;
}

void GameConfig::deinit() {
    ETNode<ETGameConfig>::disconnect();
}

JSONNode GameConfig::unpackAndLoadConfig(const char* assetFile, const char* unpackedFile) {
    JSONNode node;
    ET_SendEventReturn(node, &ETAssets::ET_loadLocalJSONFile, unpackedFile);
    if(!node) {
        ET_SendEventReturn(node, &ETAssets::ET_loadJSONAsset, assetFile);
        if(!node) {
            LogError("[GameConfig::unpackAndLoadConfig] Can't load config from asset %s", assetFile);
            return JSONNode();
        }
        Buffer buff;
        buff = node.flushToBuffer();
        bool isSaved = false;
        ET_SendEventReturn(isSaved, &ETAssets::ET_saveLocalFile, unpackedFile, buff);
        if(!isSaved) {
            LogWarning("[GameConfig::unpackAndLoadConfig] Can't unpack local config from %s to %s", assetFile, unpackedFile);
        } else {
            LogDebug("[GameConfig::unpackAndLoadConfig] Unpack local config from %s to %s", assetFile, unpackedFile);
        }
    }
    return node;
}

bool GameConfig::loadLocalConfig() {
    JSONNode node = unpackAndLoadConfig(GAME_LOCAL_CONFIG, GAME_LOCAL_UNPACKED_CONFIG);
    if(!node) {
        return false;
    }
    localConfig.read(node);
    return true;
}

void GameConfig::updateLocalConfig() {
    JSONNode node;
    localConfig.save(node);
    Buffer buff = node.flushToBuffer();
    bool saveRes = false;
    ET_SendEventReturn(saveRes, &ETAssets::ET_saveLocalFile, GAME_LOCAL_UNPACKED_CONFIG, buff);
    if(!saveRes) {
        LogWarning("[GameConfig::updateLocalConfig] Can't update local config: %s", GAME_LOCAL_UNPACKED_CONFIG);
    } else {
        LogDebug("[GameConfig::updateLocalConfig] Update local config: %s", GAME_LOCAL_UNPACKED_CONFIG);
    }
}

bool GameConfig::ET_isSoundEnabled() const {
    return localConfig.soundEnabled;
}

void GameConfig::ET_setSoundEnabled(bool flag) {
    localConfig.soundEnabled = flag;
    updateLocalConfig();
}

bool GameConfig::ET_isVibrationEnabled() const {
    return localConfig.vibrationEnabled;
}

void GameConfig::ET_setVibrationEnabled(bool flag) {
    localConfig.vibrationEnabled = flag;
    updateLocalConfig();
}

int GameConfig::ET_getHighScore() const {
    return localConfig.highScore;
}

void GameConfig::ET_setHighScore(int newHighScore) {
    localConfig.highScore = newHighScore;
    updateLocalConfig();
}