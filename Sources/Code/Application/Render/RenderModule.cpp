#include "Render/RenderModule.hpp"
#include "Render/Render.hpp"
#include "Render/RenderGeometryManager.hpp"
#include "Render/RenderFontManager.hpp"
#include "Render/RenderMaterialManager.hpp"
#include "Render/RenderCamera.hpp"

RenderModule::RenderModule() :
    SystemModule("Render") {}

RenderModule::LogicsContainerPtrT RenderModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            Render,
            RenderCamera,
            RenderGeometryManager,
            RenderMaterialManager,
            RenderFontManager>()
        );
    return container;
}