//
// Created by templechen on 2020-01-02.
//

#ifndef ASSIMPVIEWER_ASSIMPSKELETALFILTER_H
#define ASSIMPVIEWER_ASSIMPSKELETALFILTER_H


#include "AssimpBaseFilter.h"

class AssimpSkeletalFilter : public AssimpBaseFilter{

protected:
    void recursiveGenBuffers(const struct aiScene *sc, const struct aiNode *nd) override;

};



#endif //ASSIMPVIEWER_ASSIMPSKELETALFILTER_H
