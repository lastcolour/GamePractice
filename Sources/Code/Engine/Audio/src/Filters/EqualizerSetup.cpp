#include "Filters/EqualizerSetup.hpp"
#include "Reflect/ReflectContext.hpp"

void EqualizerSetup::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<EqualizerSetup>("EqualizerSetup")) {
        classInfo->addField("volume", &EqualizerSetup::volume);
        classInfo->addField("low", &EqualizerSetup::low);
        classInfo->addField("lowMid", &EqualizerSetup::lowMid);
        classInfo->addField("mid", &EqualizerSetup::mid);
        classInfo->addField("highMid", &EqualizerSetup::highMid);
        classInfo->addField("high", &EqualizerSetup::high);
    }
}

EqualizerSetup::EqualizerSetup() :
    volume(1.f),
    low(0.f),
    lowMid(0.f),
    mid(0.f),
    highMid(0.f),
    high(0.f) {
}