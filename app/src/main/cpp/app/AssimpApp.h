#pragma once

#include "jni.h"
#include "EGLCore.h"
#include "memory"
#include "AssetManager.h"
#include "IFilter.h"
#include "AssimpSkeletalFilter.h"

class AssimpApp {

public:

    AssimpApp(JNIEnv *env, jobject javaAssetManager, JavaVM* jvm);

    ~AssimpApp();

    void create(ANativeWindow *window);

    void change(int width, int height);

    void destroy();

    void doFrame();

    void setScroll(int scrollX, int scrollY);

    void setScale(int scale);

    void release();

private:

    std::unique_ptr<EGLCore> mEGLCore;
    std::unique_ptr<AssimpSkeletalFilter> mFilter;
    int mWidth = 0, mHeight = 0;

};