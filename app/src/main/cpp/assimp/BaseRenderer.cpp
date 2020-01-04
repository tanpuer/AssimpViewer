//
// Created by templechen on 2020-01-04.
//

#include <native_log.h>
#include <GLES3/gl3.h>
#include "BaseRenderer.h"

void BaseRenderer::viewerCreated(ANativeWindow *nativeWindow) {
    ALOGD("viewerCreated");
    eglCore = new egl_core(nullptr, FLAG_TRY_GLES3);
    windowSurface = new window_surface(nativeWindow, eglCore);
    windowSurface->makeCurrent();
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_2D);
    glCullFace(GL_CCW);
    glEnable(GL_BLEND);
    filter = initFilter();
    filter->init();
}

void BaseRenderer::viewerChanged(int width, int height) {
    ALOGD("viewerChanged");
    glViewport(0, 0, width, height);
    filter->setNativeWindowSize(width, height);
}

void BaseRenderer::viewerDestroyed() {
    if (filter != nullptr) {
        filter->release();
        delete filter;
        filter = nullptr;
    }
    if (windowSurface != nullptr) {
        windowSurface->release(true);
        delete windowSurface;
        windowSurface = nullptr;
    }
    if (eglCore != nullptr) {
        delete eglCore;
        eglCore = nullptr;
    }
    ALOGD("viewerDestroyed");
}

void BaseRenderer::viewerDoFrame() {
    glClearColor(0.5f, 0.5f, 0.5f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    filter->doFrame();
    windowSurface->swapBuffer();
}

void BaseRenderer::setScroll(int scrollX, int scrollY) {
    if (filter != nullptr) {
        filter->setScroll(scrollX, scrollY);
    }
}

void BaseRenderer::setScale(int scale) {
    if (filter != nullptr) {
        filter->setScale(scale);
    }
}
