#include "EditorModule.hpp"
#include "EditorAssetsSetup.hpp"

EditorModule::EditorModule() :
    SystemModule("Editor") {
}

EditorModule::~EditorModule() {
}

EditorModule::LogicsContainerPtrT EditorModule::createSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            EditorAssetsSetup>()
        );
    return container;
}

void EditorModule::reflectSystemConfigs(ReflectContext& ctx) const {
}

void EditorModule::registerEntityLogics(EntityLogicRegister& logicRegister) const {
}