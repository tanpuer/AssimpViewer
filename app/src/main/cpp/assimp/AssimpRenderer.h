//
// Created by templechen on 2020-01-01.
//

#ifndef ASSIMPVIEWER_ASSIMPRENDERER_H
#define ASSIMPVIEWER_ASSIMPRENDERER_H


#include <android/native_window.h>
#include <window_surface.h>
#include "IAssmipFilter.h"

class AssimpRenderer {

public:

    AssimpRenderer();

    ~AssimpRenderer();

    void assmipViewerCreated(ANativeWindow *nativeWindow);

    void assmipViewerChanged(int width, int height);

    void assmipViewerDestroyed();

    void assimpViewerDoFrame();

private:

    egl_core *eglCore;

    window_surface *windowSurface;

    IAssmipFilter *filter;

};


#endif //ASSIMPVIEWER_ASSIMPRENDERER_H
