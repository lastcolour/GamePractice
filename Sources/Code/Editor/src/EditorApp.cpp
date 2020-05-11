#include "EditorApp.hpp"
#include "Platforms/Desktop/DesktopPlatform.hpp"

EditorApp::EditorApp() :
    Application(Application::PlatformPtrT(new DesktopPlatform(0, nullptr))) {
}

EditorApp::~EditorApp() {
}

bool EditorApp::initialize() {
    return init();
}

void EditorApp::deinitiazlie() {
    return;
}

const char* EditorApp::getReflectModel() const {
    return nullptr;
}

void EditorApp::buildModules(ModuleListT& modules) {
}