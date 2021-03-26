#include "GameMusicManager.hpp"
#include "Reflect/ReflectUtils.hpp"
#include "Math/Random.hpp"

#include <cassert>

namespace {

const char* GAME_MUSIC_CONFIG = "Config/GameMusic.json";

Sound* getRandomSoundByWeight(std::vector<GameSongNode>& songNodes) {
    if(songNodes.empty()) {
        return nullptr;
    }
    Math::RandomFloatGenerator floatGen;
    float totalWeight = 0.f;
    for(auto& node : songNodes) {
        assert(node.weight >= 0.f && "Invalid node weight");
        totalWeight += node.weight;
    }
    floatGen.setRange(0.f, totalWeight);
    float selectWeight = floatGen.generate();
    for(auto& node : songNodes) {
        if(selectWeight <= node.weight) {
            return &node.sound;
        } else {
            selectWeight -= node.weight;
        }
    }
    return &(songNodes.back().sound);
}

} // namespace

void GameSongNode::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameSongNode>("GameSongNode")) {
        classInfo->addField("sound", &GameSongNode::sound);
        classInfo->addField("weight", &GameSongNode::weight);
    }
}

GameSongNode::GameSongNode() :
    weight(1.f) {
}

void GameMusicManager::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameMusicManager>("GameMusicManager")) {
        classInfo->addField("fadeInDuration", &GameMusicManager::fadeInDuration);
        classInfo->addField("fadeOutDuration", &GameMusicManager::fadeOutDuration);
        classInfo->addField("menuSongs", &GameMusicManager::menuSongs);
        classInfo->addField("gameSongs", &GameMusicManager::gameSongs);
    }
}

GameMusicManager::GameMusicManager() :
    currSound(nullptr),
    currTheme(EMusicTheme::None),
    fadeInDuration(0.1f),
    fadeOutDuration(0.1f) {
}

GameMusicManager::~GameMusicManager() {
}

void GameMusicManager::ET_setTheme(EMusicTheme newTheme) {
    if(currTheme == newTheme) {
        return;
    }
    currTheme = newTheme;
    if(currSound) {
        currSound->fadeOutStop(fadeOutDuration);
        currSound = nullptr;
    }
    Sound* newSound = nullptr;
    switch(currTheme) {
        case EMusicTheme::None: {
            break;
        }
        case EMusicTheme::Menu: {
            newSound = getRandomSoundByWeight(menuSongs);
            break;
        }
        case EMusicTheme::Game: {
            newSound = getRandomSoundByWeight(gameSongs);
            break;
        }
        default: {
            assert(false && "Invalid theme");
        }
    }
    if(newSound == nullptr) {
        return;
    }

    currSound = newSound;
    currSound->setLooped(true);
    currSound->fadeInPlay(fadeInDuration);
}

bool GameMusicManager::init() {
    if(!ReflectUtils::LoadObjectFromAsset(*this, GAME_MUSIC_CONFIG)) {
        LogError("[GameMusicManager::init] Can't load game music configs from: '%s'", GAME_MUSIC_CONFIG);
        return true;
    }
    ETNode<ETGameMusic>::connect(getEntityId());
    return true;
}

void GameMusicManager::deinit() {
}