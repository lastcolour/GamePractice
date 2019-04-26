#include "Render/RenderModule.hpp"
#include "Render/Render.hpp"

RenderModule::RenderModule() :
    SystemModule("Render") {}

RenderModule::LogicsContainerPtrT RenderModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            Render>()
        );
    return container;
}