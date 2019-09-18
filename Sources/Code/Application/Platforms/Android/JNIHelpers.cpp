#include "Platforms/Android/JNIHelpers.hpp"
#include "Platforms/Android/AndroidPlatformHandler.hpp"

namespace JNI {

JNIEnv* JNIAttacher::JV_ENV = nullptr;

JNIAttacher::JNIAttacher() {
    if(JV_ENV) {
        assert(false && "Double try to attach to JavaVM thread");
        return;
    }
    GetAndroindPlatformHandler()->attachToJavaVMThread(JV_ENV);
}

JNIAttacher::~JNIAttacher() {
    if(!JV_ENV) {
        assert(false && "No JNIEnv to attach from JavaVM thread");
        return;
    }
    GetAndroindPlatformHandler()->detachFromJavaVMTrehad();
    JV_ENV = nullptr;
}

JNIEnv* JNIAttacher::GetJNIEnv() {
    if(!JV_ENV) {
        assert(false && "No JNIEnv thread");
        return nullptr;
    }
    return JV_ENV;
}

bool CheckJavaException() {
    JNIEnv* jv_env =  JNIAttacher::GetJNIEnv();
    if(!jv_env->ExceptionCheck()) {
        return true;
    }

    std::string exceptionMsg;
    {

        jthrowable jv_exception = jv_env->ExceptionOccurred();
        jv_env->ExceptionClear();

        jclass jv_clazz = jv_env->GetObjectClass(jv_exception);
        jmethodID jv_methodId = jv_env->GetMethodID(jv_clazz, "getMessage", "()Ljava/lang/String;");

        assert(jv_methodId != 0 && "Invalid method id");

        jstring jv_retObject = static_cast<jstring>(jv_env->CallObjectMethod(jv_exception, jv_methodId));

        const char *cMsgStr = jv_env->GetStringUTFChars(jv_retObject, nullptr);
        exceptionMsg = cMsgStr;

        jv_env->ReleaseStringUTFChars(jv_retObject, cMsgStr);
        jv_env->DeleteLocalRef(jv_retObject);
        jv_env->DeleteLocalRef(jv_clazz);
        jv_env->DeleteLocalRef(jv_exception);
    }

    LogError("[CheckJavaException] Exception message: %s", exceptionMsg);
    return false;
}

} // namespace