//
// Created by templechen on 2020-01-01.
//

#ifndef ASSIMPVIEWER_IASSMIPFILTER_H
#define ASSIMPVIEWER_IASSMIPFILTER_H

#include "string"

#define GET_STR(x) #x
#define BUFFER_OFFSET(i) ((char*)NULL + (i))

class IAssmipFilter {

public:

    virtual void init() = 0;

    virtual void release() = 0;

    virtual void doFrame() = 0;

    virtual void setNativeWindowSize(int width, int height) = 0;

    virtual void loadObj() = 0;

    virtual void initShaders() = 0;

    bool fileExist(const std::string& filename);

};


#endif //ASSIMPVIEWER_IASSMIPFILTER_H
