//
// Created by templechen on 2020-01-01.
//

#ifndef ASSIMPVIEWER_ASSIMPRENDERER_H
#define ASSIMPVIEWER_ASSIMPRENDERER_H


#include <android/native_window.h>
#include "BaseRenderer.h"

class AssimpRenderer : public BaseRenderer{

public:

    AssimpRenderer();

    ~AssimpRenderer();

    IFilter *initFilter() override;

    jobject javaAssetManager = NULL;

    JavaVM *javaVm;

};


#endif //ASSIMPVIEWER_ASSIMPRENDERER_H
