#ifndef __SOUND_DATA_MANGER_HPP__
#define __SOUND_DATA_MANGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Audio/ETSoundManagers.hpp"
#include "SoundProxy.hpp"

#include <unordered_set>

class SoundDataManager : public SystemLogic,
    public ETNode<ETSoundDataManager> {
public:

    SoundDataManager();
    virtual ~SoundDataManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETSoundDataManager
    SoundProxy* ET_createSoundProxy() override;
    void ET_removeSoundProxy(SoundProxy* state) override;
    std::shared_ptr<SoundData> ET_loadSoundData(const char* fileName) override;

private:

    std::unordered_set<std::unique_ptr<SoundProxy>> soundProxies;
    std::unordered_map<std::string, std::shared_ptr<SoundData>> sounds;
};

#endif /* __SOUND_DATA_MANGER_HPP__ */