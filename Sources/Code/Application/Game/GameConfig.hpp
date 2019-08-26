#ifndef __GAME_CONFIG_HPP__
#define __GAME_CONFIG_HPP__

#include "Core/SystemLogic.hpp"
#include "GAme/ETGameInterfaces.hpp"

class JSONNode;

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

    void readConfig(const JSONNode& node);
    void saveConfig();

private:

    std::string mainView;
    int highScore;
    bool soundEnabled;
    bool vibrationEnabled;
};

#endif /* __GAME_CONFIG_HPP__ */