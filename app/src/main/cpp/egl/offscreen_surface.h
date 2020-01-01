//
// Created by cw on 2018/7/24.
//

#ifndef EGLDEMO_OFFSCREENSURFACE_H
#define EGLDEMO_OFFSCREENSURFACE_H

#include "base_surface.h"

class offscreen_surface : public base_surface{

public:
    offscreen_surface(egl_core* eglCore,int width, int height);
    void release();

};


#endif //EGLDEMO_OFFSCREENSURFACE_H
