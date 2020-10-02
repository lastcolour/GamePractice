#include "Android/JNIHelpers.hpp"
#include "Android/AndroidPlatformHandler.hpp"

namespace JNI {

JNIAttacher::JavaEnvData JNIAttacher::ENV_DATA;

JNIAttacher::JNIAttacher() {
    if(ENV_DATA.env) {
        assert(false && "Double try to attach to JavaVM thread");
        return;
    }

    JavaVM* javaVM = static_cast<JavaVM*>(GetAndroindPlatformHandler()->getJavaVM());
    jint res = javaVM->GetEnv(reinterpret_cast<void**>(&ENV_DATA.env), JNI_VERSION_1_6);
    if(res == JNI_EDETACHED) {
        res = javaVM->AttachCurrentThread(&ENV_DATA.env, nullptr);
        if(res == JNI_OK) {
            ENV_DATA.inMain = false;
        }
        assert(res == JNI_OK && "Can't attach to JavaVM thread");
    } else {
        ENV_DATA.inMain = true;
    }
}

JNIAttacher::~JNIAttacher() {
    if(!ENV_DATA.env) {
        assert(false && "No JNIEnv to attach from JavaVM thread");
        return;
    }
    if(!ENV_DATA.inMain) {
        JavaVM* javaVM = static_cast<JavaVM*>(GetAndroindPlatformHandler()->getJavaVM());
        javaVM->DetachCurrentThread();
    }
    ENV_DATA.env = nullptr;
    ENV_DATA.inMain = false;
}

JNIEnv* JNIAttacher::GetJNIEnv() {
    if(!ENV_DATA.env) {
        assert(false && "No JNIEnv thread");
        return nullptr;
    }
    return ENV_DATA.env;
}

JVObject JVObject::GetActivityObject() {
    jobject jvObject = static_cast<jobject>(
            GetAndroindPlatformHandler()->getActivityJavaObject());
    return JVObject(jvObject);
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