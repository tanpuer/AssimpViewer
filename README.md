# AssimpViewer
show 3D models with skeleton animations using Assimp
1. 请将assets目录下的cowboy.dae和cowboy.png 通过adb push命令放到/sdcard目录下，如果想要尝试其他模型，请自行修改文件目录。
2. 支持手势放大缩小和旋转，如果模型没显示出来，请尝试放大缩小试试，相机位置目前被我定死了。

![img](https://github.com/tanpuer/AssimpViewer/blob/master/output.gif)

编译脚本，仅供参考
1. 下载Assimp，master分支，assimp目录下建立此脚本文件，再创建buildAndroid文件夹。
2. 将contrib/unzip/crypt.h中的typedef unsigned long z_crc_t;注释掉，不然会报错。
3. 运行脚本，就能在buildAndroid目录下能拿到so和头文件啦。

#!/bin/bash
SDK_PATH=/Users/dzsb-000848/Library/Android/sdk
export ANDROID_NDK_PATH=$SDK_PATH/ndk-bundle
export ANDROID_SDK_PATH=SDK_PATH
export CMAKE_TOOLCHAIN=$SDK_PATH/ndk-bundle/build/cmake/android.toolchain.cmake
export ANDROID_NDK_TOOLCHAIN=$SDK_PATH/android-toolchain
export PATH=$PATH:$SDK_PATH/android-toolchain/bin

function build 
{
cmake \
-DCMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN \
-DANDROID_ABI=$1 \
-DANDROID_ARM_NEON=ON \
-DCMAKE_INSTALL_PREFIX=buildAndroid/assimp/$1 \
-DANDROID_NATIVE_API_LEVEL=android-21 \
-DANDROID_STL=c++_shared \
-DANDROID_NDK=$ANDROID_NDK_PATH \
-DCMAKE_BUILD_TYPE=Release \
-DANDROID_FORCE_ARM_BUILD=TRUE \
-DCMAKE_CXX_FLAGS=-Wno-c++11-narrowing \
-DANDROID_TOOLCHAIN=clang \
-DASSIMP_BUILD_ZLIB=ON \
-DASSIMP_BUILD_OBJ_IMPORTER=TRUE \
-DASSIMP_BUILD_FBX_IMPORTER=TRUE \
-DASSIMP_BUILD_COLLADA_IMPORTER=TRUE \
-DASSIMP_BUILD_TESTS=OFF \
-DASSIMP_NO_EXPORT=ON \
-DASSIMP_BUILD_ASSIMP_TOOLS=OFF \
-DASSIMP_BUILD_SAMPLES=OFF \
-DASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT=OFF \
-DASSIMP_INSTALL_PDB=OFF \

make clean
make -j8 install/strip

}

build armeabi-v7a

build arm64-v8a