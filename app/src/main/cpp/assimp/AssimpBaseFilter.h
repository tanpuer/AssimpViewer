//
// Created by templechen on 2020-01-01.
//

#ifndef ASSIMPVIEWER_ASSIMPBASEFILTER_H
#define ASSIMPVIEWER_ASSIMPBASEFILTER_H


#include <GLES3/gl3.h>
#include "IAssmipFilter.h"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "map"
#include "vecmath.h"
#include "vector"

struct SHADER_VERTEX {
    float pos[3];
    float normal[3];
    float color[3];
    float uv[2];
};

struct DrawObject {
    GLuint buffer;
    std::string textureName;
    int triangleSize;
    GLuint textureId;
};

enum SHADER_ATTRIBUTES {
    ATTRIB_VERTEX,
    ATTRIB_NORMAL,
    ATTRIB_UV,
    ATTRIB_COLOR
};

struct SHADER_PARAMS {
    GLuint program;
    GLuint light0;
    GLuint materialDiffuse;
    GLuint materialAmbient;
    GLuint materialSpecular;

    GLuint projectionMatrix;
    GLuint viewMatrix;

    GLuint samplerObj;
};

struct SHADER_MATERIALS {
    float diffuseColor[3];
    float specularColor[4];
    float ambientColor[3];
};

class AssimpBaseFilter : public IAssmipFilter {

public:

    virtual void init();

    virtual void release();

    virtual void doFrame();

    virtual void setNativeWindowSize(int width, int height);

    virtual void loadObj();

    virtual void initShaders();

private:

    void import3DModel();

    void recursiveGenBuffers(const struct aiScene *sc, const struct aiNode *nd);

    const aiScene *scene = NULL;
    // Create an instance of the Importer class
    Assimp::Importer importer;

    // images / texture
    std::map<std::string, GLuint *> textureIdMap;
    GLuint *textureIds;

    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint program;

    ndk_helper::Mat4 projectionMatrix;
    ndk_helper::Mat4 viewMatrix;
    ndk_helper::Mat4 modelMatrix;
    SHADER_PARAMS *shaderProgram;
    int drawCall = 0;
    std::vector<DrawObject> drawObjects;

    float CAM_X = 0.f;
    float CAM_Y = 0.f;
    float CAM_Z = 7.f;
};


#endif //ASSIMPVIEWER_ASSIMPBASEFILTER_H
