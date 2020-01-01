//
// Created by cw on 2018/7/24.
//

#ifndef EGLDEMO_EGLSURFACEBASE_H
#define EGLDEMO_EGLSURFACEBASE_H

#include "egl_core.h"


class base_surface {

public:
    base_surface(egl_core* eglCore);

    //创建窗口surface
    void createWindowSurface(ANativeWindow* nativeWindow);
    //创建离屏surface
    void createOffscreenSurface(int width, int height);

    int getHeight();
    int getWidth();

    void releaseEglSurface();

    void makeCurrent();

    void swapBuffer();

    void makeCurrentReadFrom(base_surface *baseSurface);

    void setPresentationTime(long long nsecs);

    //获取当前帧缓冲
    char * getCurrentFrame();

protected:
    egl_core* mEglCore;
    EGLSurface mEGLSurface = EGL_NO_SURFACE;
    int mWidth = -1;
    int mHeight = -1;
};


#endif //EGLDEMO_EGLSURFACEBASE_H
