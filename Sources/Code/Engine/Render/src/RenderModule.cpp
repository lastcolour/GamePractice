#include "Render/RenderModule.hpp"
#include "Render.hpp"
#include "RenderGeometryManager.hpp"
#include "RenderFontManager.hpp"
#include "RenderMaterialManager.hpp"
#include "RenderCamera.hpp"
#include "RenderTextureManager.hpp"
#include "RenderNodeManager.hpp"
#include "Debug/DebugRender.hpp"
#include "Logics/RenderSimpleLogic.hpp"
#include "Logics/RenderTextLogic.hpp"
#include "Logics/RenderImageLogic.hpp"
#include "Logics/RenderColoredTextureLogic.hpp"
#include "Logics/RenderLinearGradientRect.hpp"
#include "Entity/EntityLogicsRegister.hpp"
#include "RenderConfig.hpp"

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
            RenderNodeManager,
            RenderFontManager,
            DebugRender>()
        );
    return container;
}

void RenderModule::reflectSystemConfigs(ReflectContext& ctx) const {
    ctx.reflect<RenderConfig>();
}

void RenderModule::registerEntityLogics(EntityLogicsRegister& logicsRegister) const {
    logicsRegister.registerLogic<RenderSimpleLogic>();
    logicsRegister.registerLogic<RenderTextLogic>();
    logicsRegister.registerLogic<RenderImageLogic>();
    logicsRegister.registerLogic<RenderColoredTextureLogic>();
    logicsRegister.registerLogic<RenderLinearGradientRect>();
}