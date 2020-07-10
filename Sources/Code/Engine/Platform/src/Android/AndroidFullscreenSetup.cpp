#include "Platforms/Android/AndroidFullscreenSetup.hpp"
#include "Platforms/Android/JNIHelpers.hpp"
#include "Platforms/Android/AndroidPlatformHandler.hpp"

AndroidFullscreenSetup::AndroidFullscreenSetup() {
}

AndroidFullscreenSetup::~AndroidFullscreenSetup() {
}

bool AndroidFullscreenSetup::init() {
    ETNode<ETAndroidMainThreadActivityEvents>::connect(getEntityId());
    return true;
}

void AndroidFullscreenSetup::deinit() {
    ETNode<ETAndroidMainThreadActivityEvents>::disconnect();
}

void AndroidFullscreenSetup::ET_onMainThreadActivityEvent(ActivityEventType eventType) {
    if(eventType == ActivityEventType::OnWindowFocusGet) {
        makeAppFullScreen();
    }
}

void AndroidFullscreenSetup::makeAppFullScreen() {
    JNI::JNIAttacher attacher;

    JNI::JVObject activityObj = GetAndroindPlatformHandler()->getActivityJavaObject();
    JNI::JVObject windowObj =  activityObj.callObjectMethod("getWindow", "()Landroid/view/Window;");
    JNI::JVObject decorViewOjb = windowObj.callObjectMethod("getDecorView", "()Landroid/view/View;");

    JNI::JVClass viewClass = JNI::JVClass::FindClass("android/view/View");

    const int SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN = viewClass.getStaticField<int>("SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN", "I");
    const int SYSTEM_UI_FLAG_HIDE_NAVIGATION = viewClass.getStaticField<int>("SYSTEM_UI_FLAG_HIDE_NAVIGATION", "I");
    const int SYSTEM_UI_FLAG_FULLSCREEN = viewClass.getStaticField<int>("SYSTEM_UI_FLAG_FULLSCREEN", "I");
    const int SYSTEM_UI_FLAG_IMMERSIVE_STICKY = viewClass.getStaticField<int>("SYSTEM_UI_FLAG_IMMERSIVE_STICKY", "I");
    const int SYSTEM_UI_FLAG_LAYOUT_STABLE = viewClass.getStaticField<int>("SYSTEM_UI_FLAG_LAYOUT_STABLE", "I");

    decorViewOjb.callVoidMethod("setSystemUiVisibility", "(I)V",
        SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
        | SYSTEM_UI_FLAG_HIDE_NAVIGATION
        | SYSTEM_UI_FLAG_FULLSCREEN
        | SYSTEM_UI_FLAG_IMMERSIVE_STICKY
        | SYSTEM_UI_FLAG_LAYOUT_STABLE);

    if(!JNI::CheckJavaException()) {
        LogError("[AndroidFullscreenSetup::makeAppFullScreen] Make app fullscreen failed");
        return;
    }

    windowObj.deleteLocalRef();
    decorViewOjb.deleteLocalRef();

    LogDebug("[AndroidFullscreenSetup::makeAppFullScreen] Make app fullscreen successed");
}
