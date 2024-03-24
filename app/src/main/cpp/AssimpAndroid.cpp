#include <jni.h>
#include <string>
#include <AssimpLooper.h>
#include "android/native_window_jni.h"
#include "native_log.h"
#include "AssimpApp.h"

static const char *AssimpEngine = "com/cw/assimpviewer/AssimpEngine";
jobject globalAssets = nullptr;
static AssimpApp *assimpApp = nullptr;

extern "C" JNIEXPORT void JNICALL
native_Init(JNIEnv *env, jobject instance, jobject javaAssetManager) {
    ALOGD("native_init")
    globalAssets = env->NewGlobalRef(javaAssetManager);
    assimpApp = new AssimpApp(env, globalAssets);
}

extern "C" JNIEXPORT void JNICALL
native_Created(JNIEnv *env, jobject instance, jobject surface) {
    ALOGD("native_init")
    if (assimpApp != nullptr) {
        assimpApp->create(ANativeWindow_fromSurface(env, surface));
    }
}

extern "C" JNIEXPORT void JNICALL
native_Changed(JNIEnv *env, jobject instance, jint width, jint height) {
    ALOGD("native_init")
    if (assimpApp != nullptr) {
        assimpApp->change(width, height);
    }
}

extern "C" JNIEXPORT void JNICALL
native_Destroyed(JNIEnv *env, jobject instance) {
    ALOGD("native_init")
    if (assimpApp != nullptr) {
        assimpApp->destroy();
    }
}

extern "C" JNIEXPORT void JNICALL
native_DoFrame(JNIEnv *env, jobject instance) {
    ALOGD("native_init")
    if (assimpApp != nullptr) {
        assimpApp->doFrame();
    }
}

extern "C" JNIEXPORT void JNICALL
native_Scroll(JNIEnv *env, jobject instance, jint scrollX, jint scrollY) {
    ALOGD("native_init")
    if (assimpApp != nullptr) {
        assimpApp->setScroll(scrollX, scrollY);
    }
}

extern "C" JNIEXPORT void JNICALL
native_Scale(JNIEnv *env, jobject instance, jint scale) {
    ALOGD("native_init")
    if (assimpApp != nullptr) {
        assimpApp->setScale(scale);
    }
}

extern "C" JNIEXPORT void JNICALL
native_Release(JNIEnv *env, jobject instance, jint scale) {
    ALOGD("native_init")
    if (assimpApp != nullptr) {
        assimpApp->release();
        delete assimpApp;
        assimpApp = nullptr;
    }
}

static JNINativeMethod g_RenderMethods[] = {
        {"nativeAssimpInit",      "(Landroid/content/res/AssetManager;)V", (void *) native_Init},
        {"nativeAssimpCreated",   "(Landroid/view/Surface;)V",             (void *) native_Created},
        {"nativeAssimpChanged",   "(II)V",                                 (void *) native_Changed},
        {"nativeAssimpDestroyed", "()V",                                   (void *) native_Destroyed},
        {"nativeAssimpDoFrame",   "()V",                                   (void *) native_DoFrame},
        {"nativeAssimpScroll",    "(II)V",                                 (void *) native_Scroll},
        {"nativeAssimpScale",     "(I)V",                                  (void *) native_Scale},
        {"nativeRelease",         "()V",                                   (void *) native_Release},
};

static int RegisterNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *nativeMethods,
                                 int methodNum) {
    ALOGD("RegisterNativeMethods start %s", className)
    jclass clazz = env->FindClass(className);
    if (clazz == nullptr) {
        ALOGD("RegisterNativeMethods fail clazz == null")
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, nativeMethods, methodNum) < 0) {
        ALOGD("RegisterNativeMethods fail")
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

static void UnRegisterNativeMethods(JNIEnv *env, const char *className) {
    ALOGD("UnRegisterNativeMethods start")
    jclass clazz = env->FindClass(className);
    if (clazz == nullptr) {
        ALOGD("UnRegisterNativeMethods fail clazz == null")
    }
    env->UnregisterNatives(clazz);
}

extern "C" jint JNI_OnLoad(JavaVM *jvm, void *p) {
    JNIEnv *env = nullptr;
    if (jvm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    RegisterNativeMethods(env, AssimpEngine, g_RenderMethods, std::size(g_RenderMethods));
    return JNI_VERSION_1_6;
}

extern "C" void JNI_OnUnload(JavaVM *jvm, void *p) {
    ALOGD("JNI_OnUnload")
    JNIEnv *env = nullptr;
    if (jvm->GetEnv((void **) env, JNI_VERSION_1_6) != JNI_OK) {
        return;
    }
    UnRegisterNativeMethods(env, AssimpEngine);
}
