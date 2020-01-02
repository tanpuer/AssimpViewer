//
// Created by templechen on 2020-01-01.
//

#ifndef ASSIMPVIEWER_ASSIMPLOOPER_H
#define ASSIMPVIEWER_ASSIMPLOOPER_H


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

    AssimpLooper(ANativeWindow *nativeWindow);

    virtual void handleMessage(LooperMessage *msg);

    void setScroll(int scrollX, int scrollY);

    void setScale(int scale);

private:

    AssimpRenderer *renderer = nullptr;

    ANativeWindow *nativeWindow;
};


#endif //ASSIMPVIEWER_ASSIMPLOOPER_H
