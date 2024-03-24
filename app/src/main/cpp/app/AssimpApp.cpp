//
// Created by cw on 2024/2/4.
//

#include "AssimpApp.h"
#include "AssimpSkeletalFilter.h"

AssimpApp::AssimpApp(JNIEnv *env, jobject javaAssetManager) {
    mFilter = std::make_unique<AssimpSkeletalFilter>();
    mFilter->setJavaAssetManager(javaAssetManager, env);
}

AssimpApp::~AssimpApp() {

}

void AssimpApp::create(ANativeWindow *window) {
    mEGLCore = std::make_unique<EGLCore>();
    mEGLCore->createGLEnv(nullptr, window, 0, 0, false);
    mEGLCore->makeCurrent();
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
}

void AssimpApp::change(int width, int height, long time) {
    mWidth = width;
    mHeight = height;
    glViewport(0, 0, width, height);
    mFilter->setNativeWindowSize(width, height);
    mEGLCore->swapBuffer();
}

void AssimpApp::destroy() {
    mEGLCore.reset(nullptr);
}

void AssimpApp::doFrame() {
    if (mEGLCore == nullptr || mFilter == nullptr) {
        return;
    }
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    mFilter->doFrame();
    mEGLCore->swapBuffer();
}

void AssimpApp::setScroll(int scrollX, int scrollY) {
    if (mEGLCore == nullptr || mFilter == nullptr) {
        return;
    }
    mFilter->setScroll(scrollX, scrollY);
}

void AssimpApp::setScale(int scale) {
    if (mEGLCore == nullptr || mFilter == nullptr) {
        return;
    }
    mFilter->setScale(scale);
}

void AssimpApp::release() {
    mFilter->release();
    mFilter = nullptr;
}
