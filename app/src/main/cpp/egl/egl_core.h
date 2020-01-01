//
// Created by cw on 2018/7/24.
//

#ifndef EGLDEMO_EGLCORE_H
#define EGLDEMO_EGLCORE_H

#include <android/native_window.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>

#define FLAG_RECORDABLE  0x01

#define FLAG_TRY_GLES3  002

class egl_core {

public:
    egl_core();
    ~egl_core();
    egl_core(EGLContext sharedContext, int flags);
    bool init(EGLContext sharedContext, int flags);
    void release();
    EGLContext getContext();

    //创建EGLSurface
    EGLSurface createWindowSurface(ANativeWindow *surface);
    //创建离屏EGLSurface
    EGLSurface createOffscreenSurface(int width, int height);
    void releaseSurface(EGLSurface surface);

    //切换到当前上下文
    void makeCurrent(EGLSurface eglSurface);
    //切换到某个上下文
    void makeCurrent(EGLSurface drawSurface, EGLSurface readSurface);
    //没有上下文
    void makeNothingCurrent();

    void swapBuffers(EGLSurface surface);

    //设置pts
    void setPresentationTime(EGLSurface eglSurface, long long nsecs);

    //判断是否属于当前上下文
    bool isCurrent(EGLSurface eglSurface);
    //执行查询
    int querySurface(EGLSurface eglSurface, int what);
    //查询字符串
    const char* queryString(int what);
    //获取当前gles版本
    int getGlVersion();
    //检查是否出错
    bool checkEGLError(const char* msg);

private:
    EGLDisplay mEGLDisplay = EGL_NO_DISPLAY;
    EGLContext mEGLContext = EGL_NO_CONTEXT;
    EGLConfig mEGLConfig = NULL;
    int mGLVersion = -1;

    //查找合适的egl
    EGLConfig getConfig(int flags, int version);

    // 设置时间戳方法
    PFNEGLPRESENTATIONTIMEANDROIDPROC eglPresentationTimeANDROID = NULL;
};


#endif //EGLDEMO_EGLCORE_H
