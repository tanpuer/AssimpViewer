//
// Created by templechen on 2020-01-02.
//

#include <native_log.h>
#include <matrix_util.h>
#include <gl_utils.h>
#include "AssimpSkeletalFilter.h"
#include "stb_image.h"

static const char *SKELETAL_VERTEX_SHADER = GET_STR(
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
        varying vec3 vMyColor;
        attribute vec4 BoneIDs;
        attribute highp vec4 Weights;
        const int MAX_BONES = 100;
        uniform mat4 gBones[MAX_BONES];
        void main(void) {
            vMyColor = myColor;
            mat4 BoneTransform = gBones[int(BoneIDs[0])] * Weights[0];
            BoneTransform += gBones[int(BoneIDs[1])] * Weights[1];
            BoneTransform += gBones[int(BoneIDs[2])] * Weights[2];
            BoneTransform += gBones[int(BoneIDs[3])] * Weights[3];
            highp vec4 p = BoneTransform * vec4(myVertex,1);
//            highp vec4 p = vec4(myVertex,1);
            gl_Position = uPMatrix * p;
            texCoord = myUV;
            highp vec3 worldNormal = vec3(mat3(uMVMatrix[0].xyz, uMVMatrix[1].xyz, uMVMatrix[2].xyz) * myNormal);
            highp vec3 ecPosition = p.xyz;
            diffuseLight = dot( worldNormal, normalize(-vLight0+ecPosition)) * vec4(1.0, 1.0, 1.0, 1.0);
            normal = worldNormal;
            position = ecPosition;
        }
);

static const char *SKELETAL_FRAGMEMT_SHADER = GET_STR(
        uniform lowp vec3 vMaterialAmbient;
        uniform lowp vec4 vMaterialSpecular;
        uniform sampler2D samplerObj;
        varying mediump vec2 texCoord;
        varying lowp vec4 diffuseLight;
        uniform highp vec3   vLight0;
        varying mediump vec3 position;
        varying mediump vec3 normal;
        varying vec3 vMyColor;
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

void AssimpSkeletalFilter::VertexBoneData::AddBoneData(uint BoneID, float Weight) {
    for (uint i = 0; i < ARRAY_SIZE_IN_ELEMENTS(IDs); i++) {
        if (Weights[i] == 0.0) {
            IDs[i] = BoneID;
            Weights[i] = Weight;
            return;
        }
        ALOGE("todo %d %f", BoneID, Weight);
    }

    // should never get here - more bones than we have space for
//    assert(0);
}

void AssimpSkeletalFilter::recursiveGenBuffers(const struct aiScene *sc, const struct aiNode *nd) {
    for (int i = 0; i < nd->mNumMeshes; ++i) {
        const struct aiMesh *mesh = scene->mMeshes[nd->mMeshes[i]];

        int num = mesh->mNumVertices;
        int32_t stride = sizeof(SHADER_VERTEX);

        auto *p = new SHADER_VERTEX[mesh->mNumVertices];
        for (int i = 0; i < mesh->mNumVertices; ++i) {
            p[i].pos[0] = mesh->mVertices[i].x;
            p[i].pos[1] = mesh->mVertices[i].y;
            p[i].pos[2] = mesh->mVertices[i].z;
            if (mesh->HasNormals()) {
                p[i].normal[0] = mesh->mNormals[i].x;
                p[i].normal[1] = mesh->mNormals[i].y;
                p[i].normal[2] = mesh->mNormals[i].z;
            } else {
                p[i].normal[0] = 0.0;
                p[i].normal[1] = 0.0;
                p[i].normal[2] = 0.0;
            }
            if (mesh->HasVertexColors(0)) {
                p[i].color[0] = mesh->mColors[0][i].r;
                p[i].color[1] = mesh->mColors[0][i].g;
                p[i].color[2] = mesh->mColors[0][i].b;
            } else {
                p[i].color[0] = 0.0;
                p[i].color[1] = 0.0;
                p[i].color[2] = 0.0;
            }
            if (mesh->HasTextureCoords(0)) {
                p[i].uv[0] = mesh->mTextureCoords[0][i].x;
                p[i].uv[1] = 1 - mesh->mTextureCoords[0][i].y;
            } else {
                p[i].uv[0] = 0.0;
                p[i].uv[1] = 0.0;
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

        auto *Bones = new VertexBoneData[totalNumVertices];
        //bones
        for (int i = 0; i < mesh->mNumBones; ++i) {
            uint BoneIndex = 0;
            string BoneName(mesh->mBones[i]->mName.data);
            if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {
                BoneIndex = m_NumBones;
                m_NumBones++;
                BoneInfo bi;
                m_BoneInfo.push_back(bi);
                m_BoneInfo[BoneIndex].BoneOffset = mesh->mBones[i]->mOffsetMatrix;
                m_BoneMapping[BoneName] = BoneIndex;
            } else {
                BoneIndex = m_BoneMapping[BoneName];
            }

            for (uint j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
                long vertexId = mesh->mBones[i]->mWeights[j].mVertexId;
                float Weight = mesh->mBones[i]->mWeights[j].mWeight;
                Bones[vertexId].AddBoneData(BoneIndex, Weight);
            }
        }

        GLuint boneBuffer;
        glGenBuffers(1, &boneBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, boneBuffer);
        glBufferData(GL_ARRAY_BUFFER, totalNumVertices * sizeof(VertexBoneData), Bones, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        delete[] Bones;

        unsigned int numIndices = mesh->mNumFaces * 3;
        std::vector<short> indices;
        for (int t = 0; t < mesh->mNumFaces; ++t) {
            const struct aiFace *face = &mesh->mFaces[t];
            assert(face->mNumIndices == 3);
            for (int i = 0; i < face->mNumIndices; ++i) {
                indices.push_back(face->mIndices[i]);
            }
        }
        GLuint indiceBuffer;
        glGenBuffers(1, &indiceBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        DrawObject drawObject;
        drawObject.buffer = buffer;
        drawObject.textureName = path.data;
        drawObject.triangleSize = num / 3;
        drawObject.textureId = *textureIdMap[path.data];
        drawObject.boneBuffer = boneBuffer;
        drawObject.indicesBuffer = indiceBuffer;
        drawObject.numIndices = numIndices;
        (&drawObjects)->push_back(drawObject);
        drawCall++;

        numVertices += mesh->mNumVertices;
    }
    for (int i = 0; i < nd->mNumChildren; ++i) {
        this->recursiveGenBuffers(sc, nd->mChildren[i]);
    }
}

void AssimpSkeletalFilter::import3DModel() {
    const std::string filePath = "/sdcard/cowboy.dae";
//    const std::string filePath = "/sdcard/11803_Airplane_v1_l1.obj";
//    const std::string filePath = "/sdcard/batman.obj";
//    const std::string filePath = "/sdcard/Japanese_Temple.obj";
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

    for (int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh *mesh = scene->mMeshes[i];
        totalNumVertices += mesh->mNumVertices;
    }
    m_GlobalInverseTransform = scene->mRootNode->mTransformation;

    recursiveGenBuffers(scene, scene->mRootNode);

    m_startTime = GetCurrentTimeMillis();
}

float AssimpSkeletalFilter::GetRunningTime() {
    float RunningTime = (float) ((double) GetCurrentTimeMillis() - (double) m_startTime) / 1000.0f;
    return RunningTime;
}

void AssimpSkeletalFilter::BoneTransform(float TimeInSeconds, vector<Matrix4f> &Transforms) {
    Matrix4f Identity;
    Identity.InitIdentity();

    float TicksPerSecond = (float) (scene->mAnimations[0]->mTicksPerSecond != 0
                                    ? scene->mAnimations[0]->mTicksPerSecond : 25.0f);
    float TimeInTicks = TimeInSeconds * TicksPerSecond;
    float AnimationTime = fmod(TimeInTicks, (float) scene->mAnimations[0]->mDuration);

    ReadNodeHeirarchy(AnimationTime, scene->mRootNode, Identity);
    Transforms.resize(m_NumBones);

    for (uint i = 0; i < m_NumBones; i++) {
        Transforms[i] = m_BoneInfo[i].FinalTransformation;
    }
}

void AssimpSkeletalFilter::ReadNodeHeirarchy(float AnimationTime, const aiNode *pNode,
                                             const Matrix4f &ParentTransform) {
    string NodeName(pNode->mName.data);

    const aiAnimation *pAnimation = scene->mAnimations[0];

    Matrix4f NodeTransformation(pNode->mTransformation);

    const aiNodeAnim *pNodeAnim = FindNodeAnim(pAnimation, NodeName);

    if (pNodeAnim) {
        // Interpolate scaling and generate scaling transformation matrix
        aiVector3D Scaling;
        CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
        Matrix4f ScalingM;
        ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);

        // Interpolate rotation and generate rotation transformation matrix
        aiQuaternion RotationQ;
        CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
        Matrix4f RotationM = Matrix4f(RotationQ.GetMatrix());

        // Interpolate translation and generate translation transformation matrix
        aiVector3D Translation;
        CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
        Matrix4f TranslationM;
        TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);

        // Combine the above transformations
        NodeTransformation = TranslationM * RotationM * ScalingM;
    }

    Matrix4f GlobalTransformation = ParentTransform * NodeTransformation;

    if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
        uint BoneIndex = m_BoneMapping[NodeName];
        assert(BoneIndex < m_BoneInfo.size());
        m_BoneInfo[BoneIndex].FinalTransformation =
                m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
    }

    for (uint i = 0; i < pNode->mNumChildren; i++) {
        ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
    }
}

const aiNodeAnim *
AssimpSkeletalFilter::FindNodeAnim(const aiAnimation *pAnimation, const string NodeName) {
    for (uint i = 0; i < pAnimation->mNumChannels; i++) {
        const aiNodeAnim *pNodeAnim = pAnimation->mChannels[i];

        if (string(pNodeAnim->mNodeName.data) == NodeName) {
            return pNodeAnim;
        }
    }

    return NULL;
}

void AssimpSkeletalFilter::CalcInterpolatedScaling(aiVector3D &Out, float AnimationTime,
                                                   const aiNodeAnim *pNodeAnim) {
    if (pNodeAnim->mNumScalingKeys == 1) {
        Out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

    uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
    uint NextScalingIndex = (ScalingIndex + 1);
    assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
    float DeltaTime = (float) (pNodeAnim->mScalingKeys[NextScalingIndex].mTime -
                               pNodeAnim->mScalingKeys[ScalingIndex].mTime);
    float Factor =
            (AnimationTime - (float) pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D &Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D &End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}

uint AssimpSkeletalFilter::FindScaling(float AnimationTime, const aiNodeAnim *pNodeAnim) {
    assert(pNodeAnim->mNumScalingKeys > 0);

    for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
        if (AnimationTime < (float) pNodeAnim->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);

    return 0;
}

uint AssimpSkeletalFilter::FindRotation(float AnimationTime, const aiNodeAnim *pNodeAnim) {
    assert(pNodeAnim->mNumRotationKeys > 0);

    for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
        if (AnimationTime < (float) pNodeAnim->mRotationKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);

    return 0;
}

uint AssimpSkeletalFilter::FindPosition(float AnimationTime, const aiNodeAnim *pNodeAnim) {
    for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
        if (AnimationTime < (float) pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);

    return 0;
}

void AssimpSkeletalFilter::CalcInterpolatedRotation(aiQuaternion &Out, float AnimationTime,
                                                    const aiNodeAnim *pNodeAnim) {
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumRotationKeys == 1) {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }

    uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
    uint NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    float DeltaTime = (float) (pNodeAnim->mRotationKeys[NextRotationIndex].mTime -
                               pNodeAnim->mRotationKeys[RotationIndex].mTime);
    float Factor =
            (AnimationTime - (float) pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion &StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion &EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
    Out = Out.Normalize();
}

void AssimpSkeletalFilter::CalcInterpolatedPosition(aiVector3D &Out, float AnimationTime,
                                                    const aiNodeAnim *pNodeAnim) {
    if (pNodeAnim->mNumPositionKeys == 1) {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }

    uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
    uint NextPositionIndex = (PositionIndex + 1);
    assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
    float DeltaTime = (float) (pNodeAnim->mPositionKeys[NextPositionIndex].mTime -
                               pNodeAnim->mPositionKeys[PositionIndex].mTime);
    float Factor =
            (AnimationTime - (float) pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D &Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D &End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}

void AssimpSkeletalFilter::doFrame() {
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

    //animation
    if (scene->mNumAnimations > 0) {
        vector<Matrix4f> Transforms;
        float RunningTime = GetRunningTime();
        BoneTransform(RunningTime, Transforms);
        for (uint i = 0; i < Transforms.size(); i++) {
            assert(i < MAX_BONES);
            //Transform.Print();
            glUniformMatrix4fv(m_boneLocation[i], 1, GL_TRUE, (const GLfloat *) Transforms[i]);
        }
    }

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

        if (drawObject.boneBuffer > 0) {
            glBindBuffer(GL_ARRAY_BUFFER, drawObject.boneBuffer);
            int32_t stride = sizeof(VertexBoneData);
            glVertexAttribPointer(ATTRIB_BONES, 4, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0));
            glEnableVertexAttribArray(ATTRIB_BONES);
            glVertexAttribPointer(ATTRIB_WEIGHT, 4, GL_FLOAT, GL_FALSE, stride,
                                  BUFFER_OFFSET(4 * sizeof(float)));
            glEnableVertexAttribArray(ATTRIB_WEIGHT);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawObject.indicesBuffer);

        glDrawElements(GL_TRIANGLES, drawObject.numIndices, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));


        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void AssimpSkeletalFilter::loadObj() {
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
    glBindAttribLocation(program, ATTRIB_BONES, "BoneIDs");
    glBindAttribLocation(program, ATTRIB_WEIGHT, "Weights");

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

    if (scene->mNumAnimations > 0) {
        for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_boneLocation); i++) {
            char Name[128];
            memset(Name, 0, sizeof(Name));
            SNPRINTF(Name, sizeof(Name), "gBones[%d]", i);
            m_boneLocation[i] = GetUniformLocation(Name);
        }
    }
}

GLint AssimpSkeletalFilter::GetUniformLocation(const char *pUniformName) {
    GLint location = glGetUniformLocation(shaderProgram->program, pUniformName);

    if (location < 0) {
        ALOGE("Warning! Unable to get the location of uniform %s", pUniformName);
    }

    return location;
}

void AssimpSkeletalFilter::initShaders() {
    if (scene->mNumAnimations > 0) {
        vertexShader = loadShader(GL_VERTEX_SHADER, SKELETAL_VERTEX_SHADER);
        fragmentShader = loadShader(GL_FRAGMENT_SHADER, SKELETAL_FRAGMEMT_SHADER);
    } else {
        AssimpBaseFilter::initShaders();
    }
}
