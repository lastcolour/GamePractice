#include "EditorAssetsSetup.hpp"

#include <filesystem>

EditorAssetsSetup::EditorAssetsSetup() {
}

EditorAssetsSetup::~EditorAssetsSetup() {
}

bool EditorAssetsSetup::init() {
    ETNode<ETAssetsSetup>::connect(getEntityId());
    return true;
}

void EditorAssetsSetup::deinit() {
    ETNode<ETAssetsSetup>::disconnect();
}

void EditorAssetsSetup::ET_getAssetsSearchPath(std::string& searchPath) {
    auto cwd = std::filesystem::current_path();
    searchPath = cwd.string();
}