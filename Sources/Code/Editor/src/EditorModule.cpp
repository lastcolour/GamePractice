#include "EditorModule.hpp"
#include "EditorAssetsSetup.hpp"
#include "EntityEditorHelper.hpp"

EditorModule::EditorModule() :
    SystemModule("Editor") {
}

EditorModule::~EditorModule() {
}

EditorModule::LogicsContainerPtrT EditorModule::createSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            EditorAssetsSetup,
            EntityEditorHelper>()
        );
    return container;
}

void EditorModule::createSystemConfigs() const {
}

void EditorModule::registerEntityLogics(EntityLogicsRegister& logicsRegister) const {
}