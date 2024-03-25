//
// Created by templechen on 2020-01-01.
//

#ifndef ASSIMPVIEWER_IFILTER_H
#define ASSIMPVIEWER_IFILTER_H

#include <jni.h>
#include "string"

#define GET_STR(x) #x
#define BUFFER_OFFSET(i) ((char*)NULL + (i))

class IFilter {

public:

    virtual void init() = 0;

    virtual void release() = 0;

    virtual void doFrame() = 0;

    virtual void setNativeWindowSize(int width, int height) = 0;

    virtual void loadObj() = 0;

    virtual void initShaders() = 0;

    bool fileExist(const std::string& filename);

    virtual void setScroll(int scrollX, int scrollY);

    virtual void setScale(int scale);

    virtual void setJavaAssetManager(jobject javaAssetManager, JNIEnv *env) = 0;

    int scrollX = 0;
    int scrollY = 0;
    float scaleIndex = 1.0f;

};


#endif //ASSIMPVIEWER_IFILTER_H
