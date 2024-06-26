cmake_minimum_required(VERSION 3.4.1)

find_library(log-lib log)
find_library(android-lib android)

aux_source_directory(src/main/cpp srcs)
aux_source_directory(src/main/cpp/base srcs)
aux_source_directory(src/main/cpp/assimp srcs)
aux_source_directory(src/main/cpp/app srcs)

add_library(assimpviewer SHARED ${srcs})

set(JNI_LIBS_DIR ${CMAKE_SOURCE_DIR}/src/main/jniLibs)

add_library(assmip
        SHARED
        IMPORTED)
set_target_properties(assmip
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libassimp.so)

include_directories(
        ${JNI_LIBS_DIR}/include
        ${JNI_LIBS_DIR}/include/assmip

        src/main/cpp/base
        src/main/cpp/assimp
        src/main/cpp/app
)

find_library(z-lib z)

target_link_libraries(assimpviewer assmip
        ${log-lib} ${android-lib}
        android EGL GLESv3 jnigraphics
        ${z-lib}
        )
