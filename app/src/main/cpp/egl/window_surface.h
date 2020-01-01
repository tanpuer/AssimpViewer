//
// Created by cw on 2018/7/24.
//

#ifndef EGLDEMO_WINDOWSURFACE_H
#define EGLDEMO_WINDOWSURFACE_H

#include "base_surface.h"

class window_surface : public base_surface {

public:
    window_surface(ANativeWindow* nativeWindow, egl_core* eglCore, bool releaseSurface);
    window_surface(ANativeWindow* nativeWindow, egl_core* eglCore);

    void release(bool releaseNativeWindow);

    void recreate(egl_core* eglCore);

private:
    ANativeWindow *mSurface;
    bool mReleaseSurface;
};


#endif //EGLDEMO_WINDOWSURFACE_H
