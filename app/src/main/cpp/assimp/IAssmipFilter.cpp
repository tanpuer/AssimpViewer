//
// Created by templechen on 2020-01-01.
//

#include "IAssmipFilter.h"

bool IAssmipFilter::fileExist(const std::string &filename) {
    bool ret;
    FILE *fp = fopen(filename.c_str(), "rb");
    if (fp) {
        ret = true;
        fclose(fp);
    } else {
        ret = false;
    }

    return ret;
}

void IAssmipFilter::setScroll(int scrollX, int scrollY) {
    this->scrollX = scrollX;
    this->scrollY = scrollY;
}

void IAssmipFilter::setScale(int scale) {
    this->scaleIndex = scale / 1000000.0F;
}
