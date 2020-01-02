//
// Created by templechen on 2020-01-01.
//

#include <native_log.h>
#include "AssimpLooper.h"

void AssimpLooper::handleMessage(Looper::LooperMessage *msg) {
    switch (msg->what) {
        case kMsgAssimpViewerCreated: {
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
        case kMsgAssimpViewerDestroyed: {
            if (renderer != nullptr) {
                renderer->assmipViewerDestroyed();
            }
            quit();
            break;
        }
        case kMsgAssimpViewerDoFrame: {
            if (renderer != nullptr) {
                renderer->assimpViewerDoFrame();
            }
            break;
        }
        case kMsgAssimpViewerScroll: {
            if (renderer != nullptr) {
                renderer->setScroll(msg->arg1, msg->arg2);
            }
            break;
        }
        case kMsgAssimpViewerScale: {
            if (renderer != nullptr) {
                renderer->setScale(msg->arg1);
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

void AssimpLooper::setScroll(int scrollX, int scrollY) {
    if (renderer != nullptr) {
        renderer->setScroll(scrollX, scrollY);
    }
}

void AssimpLooper::setScale(int scale) {
    if (renderer != nullptr) {
        renderer->setScale(scale);
    }
}
