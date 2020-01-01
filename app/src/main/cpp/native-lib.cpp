#include <jni.h>
#include <string>
#include <AssimpLooper.h>
#include "android/native_window_jni.h"

AssimpLooper *assimpLooper = nullptr;
extern "C" JNIEXPORT void JNICALL
Java_com_cw_assimpviewer_NativeAssimpView_nativeAssimpCreated(
        JNIEnv *env,
        jobject instance,
        jobject surface
) {
    assimpLooper = new AssimpLooper(ANativeWindow_fromSurface(env, surface));
    assimpLooper->sendMessage(assimpLooper->kMsgAssmipViewerCreated);
}

extern "C" JNIEXPORT void JNICALL
Java_com_cw_assimpviewer_NativeAssimpView_nativeAssimpChanged(
        JNIEnv *env,
        jobject instance,
        jint width,
        jint height
) {
    if (assimpLooper != nullptr) {
        assimpLooper->sendMessage(assimpLooper->kMsgAssimpViewerChanged, width, height);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_cw_assimpviewer_NativeAssimpView_nativeAssimpDestroyed(
        JNIEnv *env,
        jobject instance
) {
    if (assimpLooper != nullptr) {
        assimpLooper->sendMessage(assimpLooper->kMsgAssmipViewerDestroyed);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_cw_assimpviewer_NativeAssimpView_nativeAssimpDoFrame(
        JNIEnv *env,
        jobject instance
) {
    if (assimpLooper != nullptr) {
        assimpLooper->sendMessage(assimpLooper->kMsgAssmipViewerDoFrame);
    }
}
