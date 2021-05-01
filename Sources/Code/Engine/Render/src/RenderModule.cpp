#include "Render/RenderModule.hpp"
#include "Render.hpp"
#include "RenderGeometryManager.hpp"
#include "RenderFontManager.hpp"
#include "RenderShaderManager.hpp"
#include "RenderCamera.hpp"
#include "RenderTextureManager.hpp"
#include "RenderNodeManager.hpp"
#include "RenderTickManager.hpp"
#include "Debug/DebugRender.hpp"
#include "Debug/TasksStatsTracker.hpp"
#include "Logics/ParticlesSystem.hpp"
#include "Logics/RenderSimpleLogic.hpp"
#include "Logics/RenderTextLogic.hpp"
#include "Logics/RenderImageLogic.hpp"
#include "Logics/NinePatchImageLogic.hpp"
#include "Logics/RenderLinearGradientRect.hpp"
#include "Logics/RenderBlurLogic.hpp"
#include "Logics/RenderStencilLogic.hpp"
#include "Entity/EntityLogicsRegister.hpp"
#include "RenderConfig.hpp"
#include "Core/GlobalData.hpp"

RenderModule::RenderModule() :
    SystemModule("Render") {}

RenderModule::LogicsContainerPtrT RenderModule::createSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            Render,
            RenderCamera,
            RenderGeometryManager,
            RenderShaderManager,
            RenderTextureManager,
            RenderNodeManager,
            RenderFontManager,
            RenderTickManager,
            DebugRender,
            TasksStatsTracker>()
        );
    return container;
}

void RenderModule::createSystemConfigs() const {
    CreateGlobal<RenderConfig>("Config/Render.json");
}

void RenderModule::registerEntityLogics(EntityLogicsRegister& logicsRegister) const {
    logicsRegister.registerLogic<RenderSimpleLogic>();
    logicsRegister.registerLogic<RenderTextLogic>();
    logicsRegister.registerLogic<RenderImageLogic>();
    logicsRegister.registerLogic<NinePatchImageLogic>();
    logicsRegister.registerLogic<RenderLinearGradientRect>();
    logicsRegister.registerLogic<ParticlesSystem>();
    logicsRegister.registerLogic<RenderBlurLogic>();
    logicsRegister.registerLogic<RenderStencilLogic>();
}