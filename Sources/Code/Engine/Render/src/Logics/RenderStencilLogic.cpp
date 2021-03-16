#include "Logics/RenderStencilLogic.hpp"
#include "Render/ETRenderNode.hpp"
#include "Reflect/EnumInfo.hpp"

void RenderStencilLogic::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<EStencilOpType>("EStencilOpType")) {
        enumInfo->addValues<EStencilOpType>({
            {"Read", EStencilOpType::Read},
            {"ReadIncrease", EStencilOpType::ReadIncrease},
            {"Write", EStencilOpType::Write},
            {"Disable", EStencilOpType::Disabled}
        });
    }
    if(auto classInfo = ctx.classInfo<RenderStencilLogic>("RenderStencilLogic")) {
        classInfo->addField("mode", &RenderStencilLogic::mode);
        classInfo->addField("refVal", &RenderStencilLogic::refVal);
    }
}

RenderStencilLogic::RenderStencilLogic() :
    refVal(0),
    mode(EStencilOpType::Disabled) {
}

RenderStencilLogic::~RenderStencilLogic() {
}

void RenderStencilLogic::init() {
    ETNode<ETEntityEvents>::connect(getEntityId());
    if(refVal < 0 || refVal > 255) {
        LogWarning("[RenderStencilLogic::init] Ref val '%d' is out of range: [0, 255]", refVal);
        refVal = Math::Clamp(refVal, 0, 255);
    }
}

void RenderStencilLogic::deinit() {
    auto defaultData = StencilWirteReadData();
    ET_SendEvent(getEntityId(), &ETRenderNode::ET_setStencilData, defaultData);
}

void RenderStencilLogic::ET_onLoaded() {
    StencilWirteReadData data;
    if(mode != EStencilOpType::Disabled) {
        data.refVal = refVal;
    } else {
        data.refVal = 0;
    }
    data.mode = mode;
    ET_SendEvent(getEntityId(), &ETRenderNode::ET_setStencilData, data);

    ETNode<ETEntityEvents>::disconnect();
}