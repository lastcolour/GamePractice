#include "Platforms/Android/AndroidAudioDevice.hpp"
#include "Platforms/Android/JNIHelpers.hpp"
#include "Platforms/Android/AndroidPlatformHandler.hpp"

AndroidAudioDevice::AndroidAudioDevice() {
}

AndroidAudioDevice::~AndroidAudioDevice() {
}

bool AndroidAudioDevice::readDeviceConfig() {
    JNI::JNIAttacher attacher;

    JNI::JVObject activityObj = GetAndroindPlatformHandler()->getActivityJavaObject();

    auto contextClass = JNI::JVClass::FindClass("android/content/Context");
    auto audioService = contextClass.getStaticObjectField("AUDIO_SERVICE", "Ljava/lang/String;");
    auto audioManager = activityObj.callObjectMethod("getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;", audioService);

    auto audioManagerClass = JNI::JVClass::FindClass("android/media/AudioManager");

    auto outputSmapleRateId = audioManagerClass.getStaticObjectField("PROPERTY_OUTPUT_SAMPLE_RATE", "Ljava/lang/String;");
    auto sampleRate = audioManager.callObjectMethod("getProperty", "(Ljava/lang/String;)Ljava/lang/String;", outputSmapleRateId);

    auto bufferSizeId = audioManagerClass.getStaticObjectField("PROPERTY_OUTPUT_FRAMES_PER_BUFFER", "Ljava/lang/String;");
    auto bufferSize = audioManager.callObjectMethod("getProperty", "(Ljava/lang/String;)Ljava/lang/String;", bufferSizeId);

    std::string sampleRateStr = sampleRate.copyToString();
    std::string bufferSizeStr = bufferSize.copyToString();

    if(!JNI::CheckJavaException()) {
        LogError("[AndroidAuidoDevice::ET_getAudioConfig] Can't get audio config");
        return false;
    }

    deviceConfig.frameRate = std::stoi(sampleRateStr);
    deviceConfig.framesPerBurst = std::stoi(bufferSizeStr);
    return true;
}

bool AndroidAudioDevice::init() {
    if(!readDeviceConfig()) {
        return false;
    }
    ETNode<ETAndroidAudioDevice>::connect(getEntityId());
    return true;
}

void AndroidAudioDevice::deinit() {
    ETNode<ETAndroidAudioDevice>::disconnect();
}

const AndroidAudioDeviceConfig* AndroidAudioDevice::ET_getAudioConfig() const {
    return &deviceConfig;
}
