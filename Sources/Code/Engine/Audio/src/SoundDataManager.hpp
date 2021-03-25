#ifndef __SOUND_DATA_MANGER_HPP__
#define __SOUND_DATA_MANGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Audio/ETSoundManagers.hpp"
#include "SoundProxy.hpp"

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
    std::shared_ptr<SoundData> ET_createSoundData(const std::string& fileName) override;
    void ET_updateData() override;

private:

    void updateProxies();
    void updateSoundData();

private:

    std::mutex mutex;
    std::vector<std::unique_ptr<SoundProxy>> soundProxies;
    std::unordered_map<std::string, std::shared_ptr<SoundData>> sounds;
};

#endif /* __SOUND_DATA_MANGER_HPP__ */