#include "Render/RenderModule.hpp"
#include "Render/Render.hpp"
#include "Render/RenderGeometryManager.hpp"
#include "Render/RenderFontManager.hpp"
#include "Render/RenderMaterialManager.hpp"
#include "Render/RenderCamera.hpp"
#include "Render/RenderTextureManager.hpp"
#include "Render/Logics/RenderSimpleLogic.hpp"
#include "Render/Logics/RenderTextLogic.hpp"
#include "Render/Logics/RenderImageLogic.hpp"
#include "Render/Logics/RenderColoredTextureLogic.hpp"
#include "Render/Logics/RenderLinearGradientRect.hpp"
#include "Entity/EntityLogicRegister.hpp"

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

void RenderModule::registerEntityLogics(EntityLogicRegister& logicRegister) const {
    logicRegister.registerLogic<RenderSimpleLogic>("RenderSimple");
    logicRegister.registerLogic<RenderTextLogic>("RenderText");
    logicRegister.registerLogic<RenderImageLogic>("RenderImage");
    logicRegister.registerLogic<RenderColoredTextureLogic>("RenderColoredTexture");
    logicRegister.registerLogic<RenderLinearGradientRect>("RenderLinearGradientRect");
}