#include "Game/ViewScripts/ProgressionStars.hpp"
#include "Reflect/ReflectContext.hpp"

void ProgressionStars::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<ProgressionStars>("ProgressionStars")) {
        classInfo->addField("first", &ProgressionStars::fristId);
        classInfo->addField("second", &ProgressionStars::secondId);
        classInfo->addField("third", &ProgressionStars::thirdId);
    }
}

ProgressionStars::ProgressionStars() {
}

ProgressionStars::~ProgressionStars() {
}