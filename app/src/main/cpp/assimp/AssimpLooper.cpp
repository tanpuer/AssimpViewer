//
// Created by templechen on 2020-01-01.
//

#include <native_log.h>
#include "AssimpLooper.h"

void AssimpLooper::handleMessage(Looper::LooperMessage *msg) {
    switch (msg->what) {
        case kMsgAssmipViewerCreated: {
            renderer = new AssimpRenderer();
            renderer->assmipViewerCreated(nativeWindow);
            break;
        }
        case kMsgAssimpViewerChanged: {
            if (renderer != nullptr) {
                renderer->assmipViewerChanged(msg->arg1, msg->arg2);
            }
            break;
        }
        case kMsgAssmipViewerDestroyed: {
            if (renderer != nullptr) {
                renderer->assmipViewerDestroyed();
            }
            quit();
            break;
        }
        case kMsgAssmipViewerDoFrame: {
            if (renderer != nullptr) {
                renderer->assimpViewerDoFrame();
            }
            break;
        }
        default: {
            ALOGE("unknown type for assmip viewer");
            break;
        }
    }
}

AssimpLooper::AssimpLooper(ANativeWindow *nativeWindow) {
    this->nativeWindow = nativeWindow;
}
