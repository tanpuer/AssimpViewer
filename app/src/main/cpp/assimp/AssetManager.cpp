//
// Created by templechen on 2020-01-09.
//

#include "AssetManager.h"

char *AssetManager::readFile(const char *path) {
    AAsset *asset = AAssetManager_open(asset_manager, path, AASSET_MODE_BUFFER);
    off_t fileLength = AAsset_getLength(asset);
    char *fileContent = new char[fileLength + 1];
    AAsset_read(asset, fileContent, fileLength);
    fileContent[fileLength] = '\0';
    AAsset_close(asset);
    return fileContent;
}

AssetManager::AssetManager(JNIEnv *env, jobject javaAssetManager) {
    asset_manager = AAssetManager_fromJava(env, javaAssetManager);
}
