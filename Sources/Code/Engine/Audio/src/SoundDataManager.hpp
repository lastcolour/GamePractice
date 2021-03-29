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
    void ET_removeSoundProxy(SoundProxy* proxy) override;
    std::shared_ptr<SoundData> ET_createSoundData(const std::string& fileName) override;
    void ET_addSoundUpdate(std::function<void(void)> func) override;
    void ET_addSoundCmd(SoundProxy* proxyNode, ESoundCommand cmd, float duration) override;
    void ET_updatePendingStarts() override;
    void ET_updateSoundData() override;

private:

    struct PendingStartInfo {
        float duration;
    };

private:

    bool isSoundProxyExists(const SoundProxy* proxy);
    void createPendingStart(SoundProxy& soundProxy, float duration);
    void removePendingStart(SoundProxy& soundProxy);
    void tryStartSound(SoundProxy& soundProxy, float duration, bool isEvent);
    void tryStopSound(SoundProxy& soundProxy, float duration, bool resetOffset);

private:

    std::mutex mutex;
    std::vector<std::unique_ptr<SoundProxy>> soundProxies;
    std::unordered_map<std::string, std::shared_ptr<SoundData>> sounds;
    std::unordered_map<SoundProxy*, PendingStartInfo> pendingStarts;
};

#endif /* __SOUND_DATA_MANGER_HPP__ */