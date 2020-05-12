#include "EditorApp.hpp"
#include "Platforms/Desktop/DesktopPlatform.hpp"
#include "Render/RenderModule.hpp"
#include "Audio/AudioModule.hpp"
#include "Game/GameModule.hpp"
#include "UI/UIModule.hpp"
#include "CoreModule.hpp"
#include "Entity/EntityModule.hpp"

EditorApp::EditorApp() :
    Application(Application::PlatformPtrT(new DesktopPlatform(0, nullptr))) {
}

EditorApp::~EditorApp() {
}

bool EditorApp::initialize() {
    return init();
}

void EditorApp::deinitiazlie() {
}

const char* EditorApp::getReflectModel() const {
    return nullptr;
}

void EditorApp::buildModules(ModuleListT& modules) {
    modules.emplace_back(new CoreModule);
    if(auto platform = getPlatform()) {
        modules.push_back(platform->createPlatformModule());
    }
    modules.emplace_back(new AudioModule);
    modules.emplace_back(new EntityModule);
    modules.emplace_back(new RenderModule);
    modules.emplace_back(new UIModule);
    modules.emplace_back(new GameModule);
}