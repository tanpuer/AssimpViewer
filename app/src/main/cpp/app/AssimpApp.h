#pragma once

#include "jni.h"
#include "EGLCore.h"
#include "memory"
#include "AssetManager.h"
#include "IFilter.h"

class AssimpApp {

public:

    AssimpApp(JNIEnv *env, jobject javaAssetManager);

    ~AssimpApp();

    void create(ANativeWindow *window);

    void change(int width, int height, long time);

    void destroy();

    void doFrame();

    void setScroll(int scrollX, int scrollY);

    void setScale(int scale);

    void release();

private:

    std::unique_ptr<EGLCore> mEGLCore;
    std::unique_ptr<IFilter> mFilter;
    int mWidth = 0, mHeight = 0;

};