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
    assimpLooper->sendMessage(assimpLooper->kMsgAssimpViewerCreated);
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
        assimpLooper->sendMessage(assimpLooper->kMsgAssimpViewerDestroyed);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_cw_assimpviewer_NativeAssimpView_nativeAssimpDoFrame(
        JNIEnv *env,
        jobject instance
) {
    if (assimpLooper != nullptr) {
        assimpLooper->sendMessage(assimpLooper->kMsgAssimpViewerDoFrame);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_cw_assimpviewer_NativeAssimpView_nativeAssimpScroll(
        JNIEnv *env,
        jobject instance,
        jint scrollX,
        jint scrollY
) {
    if (assimpLooper != nullptr) {
        assimpLooper->setScroll(scrollX, scrollY);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_cw_assimpviewer_NativeAssimpView_nativeAssimpScrollAsync(
        JNIEnv *env,
        jobject instance,
        jint scrollX,
        jint scrollY
) {
    if (assimpLooper != nullptr) {
        assimpLooper->sendMessage(assimpLooper->kMsgAssimpViewerScroll, scrollX, scrollY);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_cw_assimpviewer_NativeAssimpView_nativeAssimpScale(
        JNIEnv *env,
        jobject instance,
        jint scale
) {
    if (assimpLooper != nullptr) {
        assimpLooper->setScale(scale);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_cw_assimpviewer_NativeAssimpView_nativeAssimpScaleAsync(
        JNIEnv *env,
        jobject instance,
        jint scale
) {
    if (assimpLooper != nullptr) {
        assimpLooper->sendMessage(assimpLooper->kMsgAssimpViewerScale, scale, 0);
    }
}
