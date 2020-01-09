//
// Created by templechen on 2020-01-01.
//

#include "AssimpRenderer.h"
#include "AssimpBaseFilter.h"
#include "AssimpSkeletalFilter.h"
#include "AssimpIndicesFilter.h"

AssimpRenderer::AssimpRenderer() {

}

AssimpRenderer::~AssimpRenderer() {

}

IFilter *AssimpRenderer::initFilter() {
//    filter = new AssimpBaseFilter();
//    filter = new AssimpIndicesFilter();
    filter = new AssimpSkeletalFilter();
    filter->setJavaAssetManager(javaAssetManager, javaVm);
    return filter;
}

