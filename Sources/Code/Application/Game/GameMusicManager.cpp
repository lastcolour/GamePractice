#include "GameMusicManager.hpp"
#include "Reflect/ReflectUtils.hpp"

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
        classInfo->addField("mainSongs", &GameMusicManager::mainSongs);
        classInfo->addField("levelsSongs", &GameMusicManager::levelsSongs);
        classInfo->addField("gameSongs", &GameMusicManager::gameSongs);
    }
}

GameMusicManager::GameMusicManager() {
}

GameMusicManager::~GameMusicManager() {
}

void GameMusicManager::ET_setTheme(EMusicTheme newTheme) {
}

bool GameMusicManager::init() {
    ETNode<ETGameMusic>::connect(getEntityId());
    return true;
}

void GameMusicManager::deinit() {
}