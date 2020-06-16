#ifndef __EDITOR_ASSETS_SETUP_HPP__
#define __EDITOR_ASSETS_SETUP_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETAssets.hpp"
#include "Core/ETPrimitives.hpp"

class EditorAssetsSetup : public SystemLogic,
    public ETNode<ETAssetsSetup> {
public:

    EditorAssetsSetup();
    virtual ~EditorAssetsSetup();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETAssetsSetup
    void ET_getAssetsSearchPath(std::string& searchPath) override;
    void ET_getLocalDataRootPath(std::string& localDataRooPath) override;
};

#endif /* __EDITOR_ASSETS_SETUP_HPP__ */