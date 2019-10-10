#include "Render/RenderModule.hpp"
#include "Render/Render.hpp"
#include "Render/RenderGeometryManager.hpp"
#include "Render/RenderFontManager.hpp"
#include "Render/RenderMaterialManager.hpp"
#include "Render/RenderCamera.hpp"
#include "Render/RenderTextureManager.hpp"

RenderModule::RenderModule() :
    SystemModule("Render") {}

RenderModule::LogicsContainerPtrT RenderModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            Render,
            RenderCamera,
            RenderGeometryManager,
            RenderMaterialManager,
            RenderTextureManager,
            RenderFontManager>()
        );
    return container;
}

RenderModule::ConfigsPtrT RenderModule::getSystemConfigs() const {
    return nullptr;
}
