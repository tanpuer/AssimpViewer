//
// Created by templechen on 2020-01-04.
//

#ifndef ASSIMPVIEWER_ASSIMPINDICESSKELETALFILTER_H
#define ASSIMPVIEWER_ASSIMPINDICESSKELETALFILTER_H


#include "AssimpSkeletalFilter.h"

class AssimpIndicesSkeletalFilter : public AssimpSkeletalFilter {

    void doFrame() override;

    void recursiveGenBuffers(const struct aiScene *sc, const struct aiNode *nd) override;

};


#endif //ASSIMPVIEWER_ASSIMPINDICESSKELETALFILTER_H
