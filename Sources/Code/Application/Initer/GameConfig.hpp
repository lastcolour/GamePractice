#ifndef __GAME_CONFIG_HPP__
#define __GAME_CONFIG_HPP__

#include "Core/SystemLogic.hpp"
#include "GAme/ETGameInterfaces.hpp"

class JSONNode;

struct GameGeneralConfig {

    std::string mainView;

public:

    void read(const JSONNode& node);
};

struct GameLocalConfig {
    int highScore;
    bool soundEnabled;
    bool vibrationEnabled;

public:

    GameLocalConfig();

    void read(const JSONNode& node);
    void save(JSONNode& node);
};

class GameConfig : public SystemLogic,
    public ETNode<ETGameConfig> {
public:

    GameConfig();
    virtual ~GameConfig();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETGameConfig
    bool ET_isSoundEnabled() const override;
    void ET_setSoundEnabled(bool flag) override;
    bool ET_isVibrationEnabled() const override;
    void ET_setVibrationEnabled(bool flag) override;
    const char* ET_getMainViewName() const override;
    int ET_getHighScore() const override;
    void ET_setHighScore(int newHighScore) override;

private:

    bool loadGeneralConfig();
    bool loadLocalConfig();
    JSONNode unpackAndLoadConfig(const char* fromFile, const char* toFile);
    void updateLocalConfig();

private:

    GameGeneralConfig generalConfig;
    GameLocalConfig localConfig;
};

#endif /* __GAME_CONFIG_HPP__ */