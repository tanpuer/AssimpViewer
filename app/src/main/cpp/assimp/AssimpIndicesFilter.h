//
// Created by templechen on 2020-01-04.
//

#ifndef ASSIMPVIEWER_ASSIMPINDICESFILTER_H
#define ASSIMPVIEWER_ASSIMPINDICESFILTER_H


#include "AssimpBaseFilter.h"

class AssimpIndicesFilter : public AssimpBaseFilter {

public:
    void doFrame() override;

protected:
    void recursiveGenBuffers(const struct aiScene *sc, const struct aiNode *nd) override;

};


#endif //ASSIMPVIEWER_ASSIMPINDICESFILTER_H
