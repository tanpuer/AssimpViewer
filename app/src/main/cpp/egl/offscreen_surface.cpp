//
// Created by cw on 2018/7/24.
//

#include "offscreen_surface.h"

offscreen_surface::offscreen_surface(egl_core *eglCore, int width, int height) : base_surface(eglCore){
    createOffscreenSurface(width,height);
}

void offscreen_surface::release() {
    releaseEglSurface();
}
