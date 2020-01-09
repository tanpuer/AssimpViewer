//
// Created by templechen on 2020-01-01.
//

#ifndef ASSIMPVIEWER_ASSIMPLOOPER_H
#define ASSIMPVIEWER_ASSIMPLOOPER_H


#include <jni.h>
#include "../base/Looper.h"
#include "AssimpRenderer.h"

class AssimpLooper : public Looper {

public:

    enum {
        kMsgAssimpViewerCreated,
        kMsgAssimpViewerChanged,
        kMsgAssimpViewerDestroyed,
        kMsgAssimpViewerDoFrame,
        kMsgAssimpViewerScroll,
        kMsgAssimpViewerScale
    };

    AssimpLooper(ANativeWindow *nativeWindow, jobject javaAssetManager, JavaVM *javaVm);

    virtual void handleMessage(LooperMessage *msg);

    void setScroll(int scrollX, int scrollY);

    void setScale(int scale);

private:

    AssimpRenderer *renderer = nullptr;

    ANativeWindow *nativeWindow;

    jobject javaAssetManager;

    JavaVM *javaVm;
};


#endif //ASSIMPVIEWER_ASSIMPLOOPER_H
