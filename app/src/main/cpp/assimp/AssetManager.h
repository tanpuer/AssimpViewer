//
// Created by templechen on 2020-01-09.
//

#ifndef ASSIMPVIEWER_ASSETMANAGER_H
#define ASSIMPVIEWER_ASSETMANAGER_H

#include <android/asset_manager_jni.h>

class AssetManager {

public:

    struct FileInfo {
        const unsigned char *data;
        long length;
    };

public:

    AssetManager(JNIEnv *env, jobject javaAssetManager);

    char *readFile(const char *path);

    FileInfo *readFileFromAssets(const char *path);

private:

    AAssetManager *asset_manager = nullptr;

};


#endif //ASSIMPVIEWER_ASSETMANAGER_H
