#ifndef __JNI_HELPERS_HPP__
#define __JNI_HELPERS_HPP__

#include "ETApplicationInterfaces.hpp"

#include <jni.h>
#include <cassert>

namespace JNI {

bool CheckJavaException();

class JNIAttacher {
public:

    JNIAttacher();
    ~JNIAttacher();

private:

    JNIAttacher(const JNIAttacher&) = delete;
    JNIAttacher& operator=(const JNIAttacher&) = delete;

public:

    static JNIEnv* GetJNIEnv();

private:

    struct JavaEnvData {
        JNIEnv* env;
        bool inMain;

        JavaEnvData() :
            env(nullptr),
            inMain(false) {}
    };

private:

    static JavaEnvData ENV_DATA;
};

class JVObject {
public:

    explicit JVObject(jobject jv_object) : value(jv_object) {}
    ~JVObject() = default;

    template<typename ... ArgsT>
    JVObject callObjectMethod(const char* jv_method_name, const char* jv_method_sign, ArgsT ... args) {
        assert(false && "No suitable specialization exist");
        return JVObject(nullptr);
    }

    template<>
    JVObject callObjectMethod<>(const char* jv_method_name, const char* jv_method_sign) {
        assert(value && "Invalid object to call method");
        JNIEnv* jv_env = JNIAttacher::GetJNIEnv();
        jclass jv_class = jv_env->GetObjectClass(value);
        jmethodID jv_methodId = jv_env->GetMethodID(jv_class, jv_method_name, jv_method_sign);
        if(jv_methodId == 0) {
            LogWarning("[JVObject::callObjectMethod] Can't find required method: '%s' sign: '%s'",
                jv_method_name, jv_method_sign);
            return JVObject(nullptr);
        }
        jobject jv_object = jv_env->CallObjectMethod(value, jv_methodId);
        return JVObject(jv_object);
    }

    JVObject callObjectMethod(const char* jv_method_name, const char* jv_method_sign, JVObject& objectArg) {
        assert(value && "Invalid object to call method");
        JNIEnv* jv_env = JNIAttacher::GetJNIEnv();
        jclass jv_class = jv_env->GetObjectClass(value);
        jmethodID jv_methodId = jv_env->GetMethodID(jv_class, jv_method_name, jv_method_sign);
        if(jv_methodId == 0) {
            LogWarning("[JVObject::callObjectMethod] Can't find required method: '%s' sign: '%s'",
                jv_method_name, jv_method_sign);
            return JVObject(nullptr);
        }
        jobject jv_object = jv_env->CallObjectMethod(value, jv_methodId, objectArg.value);
        return JVObject(jv_object);
    }

    template<typename ... ArgsT>
    void callVoidMethod(const char* jv_method_name, const char* jv_method_sign, ArgsT ... args) {
        assert(false && "No suitable specialization exist");
    }

    template<>
    void callVoidMethod<int>(const char* jv_method_name, const char* jv_method_sign, int param) {
        assert(value && "Invalid object to call method");
        JNIEnv* jv_env = JNIAttacher::GetJNIEnv();
        jclass jv_class = jv_env->GetObjectClass(value);
        jmethodID jv_methodId = jv_env->GetMethodID(jv_class, jv_method_name, jv_method_sign);
        if(jv_methodId == 0) {
            LogWarning("[JVObject::callVoidMethod] Can't find required method: '%s' sign: '%s'",
                jv_method_name, jv_method_sign);
            return;
        }
        jv_env->CallVoidMethod(value, jv_methodId, param);
    }

    void deleteLocalRef() {
        if(!value) {
            return;
        }
        JNIEnv* jv_env = JNIAttacher::GetJNIEnv();
        jv_env->DeleteLocalRef(value);
        value = nullptr;
    }

    std::string copyToString() const {
        assert(value && "Invalid object to cast to string");
        std::string resStr;
        {

            JNIEnv* jv_env = JNIAttacher::GetJNIEnv();
            const char *cStr = jv_env->GetStringUTFChars(reinterpret_cast<jstring>(value), 0);
            if(cStr) {
                resStr = cStr;
                jv_env->ReleaseStringUTFChars(reinterpret_cast<jstring>(value), cStr);
            }
        }
        return resStr;
    }

public:

    jobject value;
};

class JVClass {
public:

    static JVClass FindClass(const char* className) {
        JNIEnv* jv_env = JNIAttacher::GetJNIEnv();
        jclass jv_class = jv_env->FindClass(className);
        return JVClass(jv_class);
    }

public:

    explicit JVClass(jclass jv_class) : value(jv_class) {}
    ~JVClass() = default;

    template<typename RetType>
    RetType getStaticField(const char* jv_field_name, const char* jv_field_sign) {
        assert(false && "No suitable specialization exist");
        return RetType();
    }

    template<>
    int getStaticField<int>(const char* jv_field_name, const char* jv_field_sign) {
        JNIEnv* jv_env = JNIAttacher::GetJNIEnv();
        jfieldID jv_fieldId = jv_env->GetStaticFieldID(value, jv_field_name, jv_field_sign);
        if(jv_fieldId == 0) {
            LogWarning("[JVClass::getStaticField] Can't find required static field: '%s' sign: '%s'",
                jv_field_name, jv_field_sign);
            return 0;
        }
        return jv_env->GetStaticIntField(value, jv_fieldId);
    }

    JVObject getStaticObjectField(const char* jv_field_name, const char* jv_field_sign) {
        JNIEnv* jv_env = JNIAttacher::GetJNIEnv();
        jfieldID jv_fieldId = jv_env->GetStaticFieldID(value, jv_field_name, jv_field_sign);
        if(jv_fieldId == 0) {
            LogWarning("[JVClass::getStaticObjectField] Can't find required static field: '%s' sign: '%s'",
                jv_field_name, jv_field_sign);
            return JVObject(0);
        }
        return JVObject(jv_env->GetStaticObjectField(value, jv_fieldId));
    }

public:

    jclass value;
};

} // namespace JNI

#endif /* __JNI_HELPERS_HPP__ */