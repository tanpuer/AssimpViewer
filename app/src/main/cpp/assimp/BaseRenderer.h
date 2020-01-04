//
// Created by templechen on 2020-01-04.
//

#ifndef ASSIMPVIEWER_BASERENDERER_H
#define ASSIMPVIEWER_BASERENDERER_H


#include <android/native_window.h>
#include <window_surface.h>
#include "IFilter.h"

class BaseRenderer {

public:

    void viewerCreated(ANativeWindow *nativeWindow);

    void viewerChanged(int width, int height);

    void viewerDestroyed();

    void viewerDoFrame();

    void setScroll(int scrollX, int scrollY);

    void setScale(int scale);

    virtual IFilter *initFilter() = 0;

protected:

    egl_core *eglCore;

    window_surface *windowSurface;

    IFilter *filter;

};


#endif //ASSIMPVIEWER_BASERENDERER_H
