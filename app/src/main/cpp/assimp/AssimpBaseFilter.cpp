//
// Created by templechen on 2020-01-01.
//

#include <native_log.h>
#include <gl_utils.h>
#include <matrix_util.h>
#include "AssimpBaseFilter.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static const char *VERTEX_SHADER = GET_STR(
        attribute highp vec3 myVertex;
        attribute highp vec3 myNormal;
        attribute mediump vec2 myUV;
        attribute mediump vec4 myBone;
        varying mediump vec2 texCoord;
        varying lowp vec4 diffuseLight;
        varying mediump vec3 position;
        varying mediump vec3 normal;
        uniform highp mat4 uMVMatrix;
        uniform highp mat4 uPMatrix;
        uniform highp vec3 vLight0;
        uniform lowp vec4 vMaterialDiffuse;
        uniform lowp vec3 vMaterialAmbient;
        uniform lowp vec4 vMaterialSpecular;
        attribute vec3 myColor;
        varying mediump vec3 vMyColor;
        void main(void) {
            vMyColor = myColor;
            highp vec4 p = vec4(myVertex,1);
            gl_Position = uPMatrix * p;
            texCoord = myUV;
            highp vec3 worldNormal = vec3(mat3(uMVMatrix[0].xyz, uMVMatrix[1].xyz, uMVMatrix[2].xyz) * myNormal);
            highp vec3 ecPosition = p.xyz;
            diffuseLight = dot( worldNormal, normalize(-vLight0+ecPosition)) * vec4(1.0, 1.0, 1.0, 1.0);
            normal = worldNormal;
            position = ecPosition;
        }
);

static const char *FRAGMEMT_SHADER = GET_STR(
        uniform lowp vec3 vMaterialAmbient;
        uniform lowp vec4 vMaterialSpecular;
        uniform sampler2D samplerObj;
        varying mediump vec2 texCoord;
        varying lowp vec4 diffuseLight;
        uniform highp vec3   vLight0;
        varying mediump vec3 position;
        varying mediump vec3 normal;
        varying mediump vec3 vMyColor;
        void main() {
            mediump vec3 halfVector = normalize(-vLight0 + position);
            mediump float NdotH = max(dot(normalize(normal), halfVector), 0.0);
            mediump float fPower = vMaterialSpecular.w;
            mediump float specular = pow(NdotH, fPower);
            lowp vec4 colorSpecular = vec4( vMaterialSpecular.xyz * specular, 1);
            // increase ambient light to brighten the teapot :-)
//            gl_FragColor = diffuseLight * texture2D(samplerObj, texCoord) +
//            2.0f * vec4(vMaterialAmbient.xyz, 1.0f) + colorSpecular;
            gl_FragColor = texture2D(samplerObj, texCoord);
//            gl_FragColor = vec4(vMyColor, 1.0);
//            gl_FragColor = mix(texture2D(samplerObj, texCoord), vec4(vMyColor, 1.0), 0.5);
        }
);

void AssimpBaseFilter::init() {
    loadObj();
}

void AssimpBaseFilter::release() {
    for (int i = 0; i < drawObjects.size(); ++i) {
        DrawObject drawObject = drawObjects[i];
        if (drawObject.buffer > 0) {
            glDeleteBuffers(1, &drawObject.buffer);
        }
        if (drawObject.indicesBuffer > 0) {
            glDeleteBuffers(1, &drawObject.indicesBuffer);
        }
        if (drawObject.boneBuffer > 0) {
            glDeleteBuffers(1, &drawObject.boneBuffer);
        }
        if (drawObject.textureId > 0) {
            glDeleteTextures(1, &drawObject.textureId);
        }
    }
    if (shaderProgram != nullptr) {
        glDeleteProgram(shaderProgram->program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        shaderProgram = nullptr;
    }
}

void AssimpBaseFilter::doFrame() {
    viewMatrix = ndk_helper::Mat4::LookAt(ndk_helper::Vec3(CAM_X, CAM_Y, CAM_Z / scaleIndex),
                                          ndk_helper::Vec3(0.f, 0.f, 0.f),
                                          ndk_helper::Vec3(0.f, 1.f, 0.f));
    modelMatrix = ndk_helper::Mat4::Identity();
    ndk_helper::Mat4 scrollXMat = ndk_helper::Mat4::RotationY(-PI * scrollX / 180);
    ndk_helper::Mat4 scrollYMat = ndk_helper::Mat4::RotationX(-PI * scrollY / 180);
    modelMatrix = scrollXMat * scrollYMat * modelMatrix;

    viewMatrix = viewMatrix * modelMatrix;

    //
    // Feed Projection and Model View matrices to the shaders
    ndk_helper::Mat4 mat_vp = projectionMatrix * viewMatrix;

    glUseProgram(shaderProgram->program);

    SHADER_MATERIALS material = {
            {1.0f, 0.5f, 0.5f},
            {1.0f, 1.0f, 1.0f, 10.f},
            {0.1f, 0.1f, 0.1f},};

    // Update uniforms
    glUniform4f(shaderProgram->materialDiffuse, material.diffuseColor[0],
                material.diffuseColor[1], material.diffuseColor[2], 1.f);

    glUniform4f(shaderProgram->materialSpecular, material.specularColor[0],
                material.specularColor[1], material.specularColor[2],
                material.specularColor[3]);
    glUniform3f(shaderProgram->materialAmbient, material.ambientColor[0],
                material.ambientColor[1], material.ambientColor[2]);

    glUniformMatrix4fv(shaderProgram->projectionMatrix, 1, GL_FALSE,
                       mat_vp.Ptr());
    glUniformMatrix4fv(shaderProgram->viewMatrix, 1, GL_FALSE, viewMatrix.Ptr());
    glUniform3f(shaderProgram->light0, 100.f, -200.f, -6000.f);

    for (int i = 0; i < drawObjects.size(); ++i) {
        DrawObject drawObject = drawObjects[i];
        glBindBuffer(GL_ARRAY_BUFFER, drawObject.buffer);

        int32_t iStride = sizeof(SHADER_VERTEX);
        glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, iStride,
                              BUFFER_OFFSET(0));
        glEnableVertexAttribArray(ATTRIB_VERTEX);

        glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, iStride,
                              BUFFER_OFFSET(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(ATTRIB_NORMAL);

        glVertexAttribPointer(ATTRIB_COLOR, 3, GL_FLOAT, GL_FALSE, iStride,
                              BUFFER_OFFSET(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(ATTRIB_COLOR);

        glVertexAttribPointer(ATTRIB_UV, 2, GL_FLOAT, GL_FALSE, iStride,
                              BUFFER_OFFSET(9 * sizeof(GLfloat)));
        glEnableVertexAttribArray(ATTRIB_UV);

        glBindTexture(GL_TEXTURE_2D, 0);

        GLuint textureId = drawObject.textureId;
        if (textureId > 0) {
            glActiveTexture(GL_TEXTURE0 + textureId - 1);
            glBindTexture(GL_TEXTURE_2D, textureId);
            glUniform1i(shaderProgram->samplerObj, textureId - 1);
        }

        glDrawArrays(GL_TRIANGLES, 0, 3 * drawObject.triangleSize);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void AssimpBaseFilter::setNativeWindowSize(int width, int height) {
    // Init Projection matrices
    int32_t viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    const float CAM_NEAR = 1.f;
    const float CAM_FAR = 100000000.f;
    if (viewport[2] < viewport[3]) {
        float aspect =
                static_cast<float>(viewport[2]) / static_cast<float>(viewport[3]);
        projectionMatrix =
                ndk_helper::Mat4::Perspective(aspect, 1.0f, CAM_NEAR, CAM_FAR);
    } else {
        float aspect =
                static_cast<float>(viewport[3]) / static_cast<float>(viewport[2]);
        projectionMatrix =
                ndk_helper::Mat4::Perspective(1.0f, aspect, CAM_NEAR, CAM_FAR);
    }
}

void AssimpBaseFilter::loadObj() {
    import3DModel();

    initShaders();
    initShaders();

    GLuint program;
    GLint linked;
    program = glCreateProgram();
    if (program == 0) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return;
    }
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glBindAttribLocation(program, ATTRIB_VERTEX, "myVertex");
    glBindAttribLocation(program, ATTRIB_NORMAL, "myNormal");
    glBindAttribLocation(program, ATTRIB_UV, "myUV");
    glBindAttribLocation(program, ATTRIB_COLOR, "myColor");

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint infoLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1) {
            char *infoLog = (char *) malloc(sizeof(char) * infoLen);
            glGetProgramInfoLog(program, infoLen, NULL, infoLog);
            ALOGE("Error linking program:\n%s\n", infoLog);
            free(infoLog);
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(program);
        return;
    }
    shaderProgram = new SHADER_PARAMS();
    shaderProgram->projectionMatrix = glGetUniformLocation(program, "uPMatrix");
    shaderProgram->viewMatrix = glGetUniformLocation(program, "uMVMatrix");

    shaderProgram->light0 = glGetUniformLocation(program, "vLight0");
    shaderProgram->materialDiffuse = glGetUniformLocation(program, "vMaterialDiffuse");
    shaderProgram->materialAmbient = glGetUniformLocation(program, "vMaterialAmbient");
    shaderProgram->materialSpecular =
            glGetUniformLocation(program, "vMaterialSpecular");
    shaderProgram->samplerObj = glGetUniformLocation(program, "samplerObj");
    shaderProgram->program = program;


}

void AssimpBaseFilter::initShaders() {
    vertexShader = loadShader(GL_VERTEX_SHADER, VERTEX_SHADER);
    fragmentShader = loadShader(GL_FRAGMENT_SHADER, FRAGMEMT_SHADER);
}

void AssimpBaseFilter::import3DModel() {
//    const std::string filePath = "/sdcard/batman.obj";
//    const std::string filePath = "/sdcard/12228_Dog_v1_L2.obj";
//    const std::string filePath = "/sdcard/12228_Dog_v1_L2.obj";
//    const std::string filePath = "/sdcard/11803_Airplane_v1_l1.obj";
    const std::string filePath = "/sdcard/cowboy.dae";
    assert(fileExist(filePath));
    scene = importer.ReadFile(filePath, aiProcessPreset_TargetRealtime_Quality);
    if (!scene) {
        ALOGE("load model error %s", filePath.data());
    }
    ALOGD("load model success %s", filePath.data());

    if (!scene->HasTextures()) {
        /* getTexture Filenames and Numb of Textures */
        for (unsigned int m = 0; m < scene->mNumMaterials; m++) {
            int texIndex = 0;
            aiReturn texFound = AI_SUCCESS;
            aiString path;    // filename
            while (texFound == AI_SUCCESS) {
                texFound = scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
                textureIdMap[path.data] = NULL; //fill map with textures, pointers still NULL yet
                texIndex++;
            }
        }
        int numTextures = textureIdMap.size();
        /* create and fill array with GL texture ids */
        textureIds = new GLuint[numTextures];
        glGenTextures(numTextures, textureIds); /* Texture name generation */
        /* get iterator */
        std::map<std::string, GLuint *>::iterator itr = textureIdMap.begin();
        std::string basepath = "/sdcard/";
        for (int i = 0; i < numTextures; i++) {
            //save IL image ID
            std::string filename = (*itr).first;
            (*itr).second = &textureIds[i]; // save texture id for filename in map
            itr++;

            std::string fileloc = basepath + filename;
            int x, y, comp;
            unsigned char *data = stbi_load(fileloc.c_str(), &x, &y, &comp, STBI_default);

            GLuint format = GL_RGB;
            if (comp == 1) {
                format = GL_LUMINANCE;
            } else if (comp == 2) {
                format = GL_LUMINANCE_ALPHA;
            } else if (comp == 3) {
                format = GL_RGB;
            } else if (comp == 4) {
                format = GL_RGBA;
            } else {
                //todo
                ALOGE("unSupport type %d %s", comp, fileloc.data());
            }

            if (nullptr != data) {
                glActiveTexture(GL_TEXTURE0 + textureIds[i] - 1);
                glBindTexture(GL_TEXTURE_2D, textureIds[i]);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexImage2D(GL_TEXTURE_2D, 0, format, x, y, 0, format, GL_UNSIGNED_BYTE, data);
                glBindTexture(GL_TEXTURE_2D, 0);
                stbi_image_free(data);
                ALOGD("load image success %s", fileloc.data());
            } else {
                ALOGE("load image fail %s", fileloc.c_str());
            }
        }
    }

    recursiveGenBuffers(scene, scene->mRootNode);
    ALOGD("total draw call size is %d", drawCall);
}

void AssimpBaseFilter::recursiveGenBuffers(const struct aiScene *sc, const struct aiNode *nd) {
//    ALOGD("recursiveGenBuffers %s %d", nd->mName.data, nd->mNumMeshes);
    for (int i = 0; i < nd->mNumMeshes; ++i) {
        const struct aiMesh *mesh = scene->mMeshes[nd->mMeshes[i]];

        int num = mesh->mNumFaces * 3;
        int32_t stride = sizeof(SHADER_VERTEX);
        auto *p = new SHADER_VERTEX[num];
//        ALOGD("aiMesh %d %d", mesh->mNumVertices, mesh->mNumFaces);
        int index = 0;
        for (int t = 0; t < mesh->mNumFaces; ++t) {
            //默认都是三角形 mNumIndices = 3
//            const struct aiFace *face = &mesh->mFaces[t];
//            assert(face->mNumIndices == 3);
//            GLenum face_mode;
//            switch (face->mNumIndices) {
//                case 1: {
//                    face_mode = GL_POINTS;
//                    break;
//                }
//                case 2: {
//                    face_mode = GL_LINES;
//                    break;
//                }
//                case 3: {
//                    face_mode = GL_TRIANGLES;
//                    break;
//                }
//                default: {
//                    ALOGE("unSupport face mode!");
//                    break;
//                }
//            }
            const struct aiFace *face = &mesh->mFaces[t];
            assert(face->mNumIndices == 3);
            for (int j = 0; j < face->mNumIndices; ++j) {
                int vertexIndex = face->mIndices[j];

                p[index].pos[0] = mesh->mVertices[vertexIndex].x;
                p[index].pos[1] = mesh->mVertices[vertexIndex].y;
                p[index].pos[2] = mesh->mVertices[vertexIndex].z;

//                p[index].pos[0] = mesh->mVertices[vertexIndex].x / 100;
//                p[index].pos[1] = mesh->mVertices[vertexIndex].y / 100;
//                p[index].pos[2] = mesh->mVertices[vertexIndex].z / 100;
                if (mesh->HasNormals()) {
                    p[index].normal[0] = mesh->mNormals[vertexIndex].x;
                    p[index].normal[1] = mesh->mNormals[vertexIndex].y;
                    p[index].normal[2] = mesh->mNormals[vertexIndex].z;
                } else {
                    p[index].normal[0] = 0.0;
                    p[index].normal[1] = 0.0;
                    p[index].normal[2] = 0.0;
                }
                if (mesh->HasVertexColors(0)) {
                    p[index].color[0] = mesh->mColors[0][vertexIndex].r;
                    p[index].color[1] = mesh->mColors[0][vertexIndex].g;
                    p[index].color[2] = mesh->mColors[0][vertexIndex].b;
                } else {
                    p[index].color[0] = 1.0;
                    p[index].color[1] = 1.0;
                    p[index].color[2] = 1.0;
                }
                if (mesh->HasTextureCoords(0)) {
                    p[index].uv[0] = mesh->mTextureCoords[0][vertexIndex].x;
                    p[index].uv[1] = 1 - mesh->mTextureCoords[0][vertexIndex].y;
                } else {
                    p[index].uv[0] = 0.0;
                    p[index].uv[1] = 0.0;
                }
                index++;
            }
        }

        GLuint buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, stride * num, p, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        delete[] p;

        aiString path;
        scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &path);
        if (textureIdMap.find(path.data) == textureIdMap.end()) {
            ALOGE("material index is not in materialIdMap. pls check, %s", path.data);
        }

        DrawObject drawObject;
        drawObject.buffer = buffer;
        drawObject.textureName = path.data;
        drawObject.triangleSize = num / 3;
        drawObject.textureId = *textureIdMap[path.data];
        (&drawObjects)->push_back(drawObject);
        drawCall++;
    }
    for (int i = 0; i < nd->mNumChildren; ++i) {
        this->recursiveGenBuffers(sc, nd->mChildren[i]);
    }
}