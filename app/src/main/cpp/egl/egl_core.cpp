//
// Created by cw on 2018/7/24.
//

#include "egl_core.h"
#include "assert.h"
#include "../base/native_log.h"

egl_core::egl_core() {
    init(NULL,0);
}

egl_core::~egl_core() {
    release();
}

egl_core::egl_core(EGLContext sharedContext, int flags) {
    init(sharedContext,flags);
}

bool egl_core::init(EGLContext sharedContext, int flags) {
    assert(mEGLDisplay == EGL_NO_DISPLAY);
    if (mEGLDisplay != EGL_NO_DISPLAY){
        ALOGD("EGL already set up");
        return false;
    }
    if (sharedContext == NULL){
        sharedContext = EGL_NO_CONTEXT;
    }
    mEGLDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    assert(mEGLDisplay != EGL_NO_DISPLAY);
    if (mEGLDisplay == EGL_NO_DISPLAY){
        ALOGD("unable to get egldisplay");
        return false;
    }

    if (!eglInitialize(mEGLDisplay,0,0)){
        mEGLDisplay = EGL_NO_DISPLAY;
        ALOGD("unable to initialize egldisplay");
        return false;
    }

    //尝试使用GLES3
    if ((flags & FLAG_TRY_GLES3) != 0) {
        EGLConfig config = getConfig(flags, 3);
        if (config != NULL){
            int attrib3_list[] = {EGL_CONTEXT_CLIENT_VERSION,3,EGL_NONE};
            EGLContext context = eglCreateContext(mEGLDisplay,config,sharedContext,attrib3_list);
            if (checkEGLError("eglCreateContext")){
                mEGLConfig = config;
                mEGLContext = context;
                mGLVersion = 3;
            }
        }
    }

    //GLES3没有获取到，尝试使用GLES2
    if (mEGLContext == EGL_NO_CONTEXT){
        EGLConfig config = getConfig(flags, 2);
        if (config != NULL){
            int attrib2_list[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
            EGLContext context = eglCreateContext(mEGLDisplay,config,sharedContext,attrib2_list);
            if (checkEGLError("eglCreateContext")){
                mEGLConfig = config;
                mEGLContext = context;
                mGLVersion = 2;
            }
        }
    }

    // 获取eglPresentationTimeANDROID方法的地址
    eglPresentationTimeANDROID = (PFNEGLPRESENTATIONTIMEANDROIDPROC)
            eglGetProcAddress("eglPresentationTimeANDROID");
    if (!eglPresentationTimeANDROID) {
        ALOGE("eglPresentationTimeANDROID is not available!");
    }

    //confirm with query
    int values[1] = {0};
    eglQueryContext(mEGLDisplay,mEGLContext,EGL_CONTEXT_CLIENT_VERSION,values);
    ALOGD("EGLContext created, client version: %d",values[0]);
    return true;

}

EGLConfig egl_core::getConfig(int flags, int version) {
    int renderableType = EGL_OPENGL_ES2_BIT;
    if (version >=3){
        renderableType |= EGL_OPENGL_ES3_BIT_KHR;
    }
//    int attrib[] = {
//            EGL_RED_SIZE, 8,
//            EGL_GREEN_SIZE, 8,
//            EGL_BLUE_SIZE, 8,
//            EGL_ALPHA_SIZE, 8,
//            EGL_RENDERABLE_TYPE, renderableType,
//            EGL_NONE, 0, // placeholder for recordable [@-3]
//            EGL_NONE
//    };
    int attrib[] = {EGL_RENDERABLE_TYPE,
                              EGL_OPENGL_ES2_BIT,  // Request opengl ES2.0
                              EGL_SURFACE_TYPE,
                              EGL_WINDOW_BIT,
                              EGL_BLUE_SIZE,
                              8,
                              EGL_GREEN_SIZE,
                              8,
                              EGL_RED_SIZE,
                              8,
                              EGL_DEPTH_SIZE,
                              16,
                              EGL_NONE};
    int length = sizeof(attrib)/ sizeof(attrib[0]);
    if ((flags & FLAG_RECORDABLE) != 0){
        attrib[length -3] = EGL_RECORDABLE_ANDROID;
        attrib[length-2] = 1;
    }
    EGLConfig configs = NULL;
    int numConfig;
    if (!eglChooseConfig(mEGLDisplay,attrib,&configs,1,&numConfig)){
        ALOGD("can not get RGBA888 %d EGLConfig",version);
        return NULL;
    }
    return configs;
}

void egl_core::release() {
    if(mEGLDisplay != EGL_NO_DISPLAY){
        eglMakeCurrent(mEGLDisplay,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);
        eglDestroyContext(mEGLDisplay, mEGLContext);
        eglReleaseThread();
        eglTerminate(mEGLDisplay);
    }
    mEGLDisplay = EGL_NO_DISPLAY;
    mEGLContext = EGL_NO_CONTEXT;
    mEGLConfig = NULL;
    ALOGD("egl release success");
}

EGLContext egl_core::getContext() {
    return mEGLContext;
}

EGLSurface egl_core::createWindowSurface(ANativeWindow *surface) {
    if (surface == NULL){
        return NULL;
    }
    int surface_attrib[] = {EGL_NONE};
    EGLSurface eglSurface = eglCreateWindowSurface(mEGLDisplay,mEGLConfig,surface,surface_attrib);
    if (checkEGLError("eglCreateWindowSurface")){
        if (eglSurface == NULL) {
            ALOGD("eglSurface is null");
            return NULL;
        }
    }
    return eglSurface;
}

EGLSurface egl_core::createOffscreenSurface(int width, int height) {
    int surface_attrib[] = {
            EGL_WIDTH,width,
            EGL_HEIGHT,height,
            EGL_NONE
    };
    EGLSurface eglSurface = eglCreatePbufferSurface(mEGLDisplay,mEGLConfig,surface_attrib);
    if (checkEGLError("eglCreatePbufferSurface")){
        if (eglSurface !=NULL){
            return eglSurface;
        }
    }
    return eglSurface;
}

void egl_core::releaseSurface(EGLSurface surface) {
    if (mEGLDisplay != EGL_NO_DISPLAY) {
        eglDestroySurface(mEGLDisplay, surface);
    }
}

void egl_core::makeCurrent(EGLSurface eglSurface) {
    if (mEGLDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(mEGLDisplay, eglSurface, eglSurface, mEGLContext);
    }
}

void egl_core::makeCurrent(EGLSurface drawSurface, EGLSurface readSurface) {
    if (mEGLDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(mEGLDisplay, drawSurface, readSurface, mEGLContext);
    }
}

void egl_core::makeNothingCurrent() {
    if (mEGLDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(mEGLDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }
}

void egl_core::swapBuffers(EGLSurface surface) {
    if (mEGLDisplay != EGL_NO_DISPLAY) {
        eglSwapBuffers(mEGLDisplay, surface);
    }
}

void egl_core::setPresentationTime(EGLSurface eglSurface, long long nsecs) {
    eglPresentationTimeANDROID(mEGLDisplay, eglSurface, nsecs);
}

bool egl_core::isCurrent(EGLSurface eglSurface) {
    return mEGLContext == eglGetCurrentContext() && eglSurface == eglGetCurrentSurface(EGL_DRAW);
}

int egl_core::querySurface(EGLSurface eglSurface, int what) {
    int value;
    eglQuerySurface(mEGLDisplay,eglSurface,what,&value);
    return value;
}

const char *egl_core::queryString(int what) {
    return eglQueryString(mEGLDisplay,what);
}

int egl_core::getGlVersion() {
    return mGLVersion;
}

bool egl_core::checkEGLError(const char *msg) {
    int error;
    if ((error = eglGetError())!=EGL_SUCCESS){
        ALOGD("%s : egl error %x", msg,error);
        return false;
    }
    return true;
}
