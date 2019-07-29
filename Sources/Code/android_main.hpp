#ifndef __ANDROID_MAIN_HPP__
#define __ANDROID_MAIN_HPP__

#include <cstdio>

struct ANativeActivity;

extern "C" {
    extern void ANativeActivity_onCreate(ANativeActivity* activity, void* savedState, size_t savedStateSize);
}

#endif /* __ANDROID_MAIN_HPP__ */