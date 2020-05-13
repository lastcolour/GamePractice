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

RenderModule::LogicsContainerPtrT RenderModule::createSystemLogics() const {
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

void RenderModule::reflectSystemConfigs(ReflectContext& ctx) const {
}

void RenderModule::registerEntityLogics(EntityLogicRegister& logicRegister) const {
    logicRegister.registerLogic<RenderSimpleLogic>();
    logicRegister.registerLogic<RenderTextLogic>();
    logicRegister.registerLogic<RenderImageLogic>();
    logicRegister.registerLogic<RenderColoredTextureLogic>();
    logicRegister.registerLogic<RenderLinearGradientRect>();
}