#include "Game/Logics/ObjectiveTarget.hpp"
#include "Reflect/ReflectContext.hpp"

void ObjectiveTarget::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<ObjectiveTarget>("ObjectiveTarget")) {
        classInfo->addField("minScore", &ObjectiveTarget::minScore);
    }
}

ObjectiveTarget::ObjectiveTarget() :
    minScore(0) {
}

ObjectiveTarget::~ObjectiveTarget() {
}

