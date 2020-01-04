//
// Created by templechen on 2020-01-04.
//

#include <native_log.h>
#include <matrix_util.h>
#include "AssimpIndicesFilter.h"
#include "native_log.h"

void AssimpIndicesFilter::doFrame() {
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

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawObject.indicesBuffer);
//        glDrawArrays(GL_TRIANGLES, 0, 3 * drawObject.triangleSize);

        glDrawElements(GL_TRIANGLES, drawObject.numIndices, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void AssimpIndicesFilter::recursiveGenBuffers(const struct aiScene *sc, const struct aiNode *nd) {
    for (int i = 0; i < nd->mNumMeshes; ++i) {
        const struct aiMesh *mesh = scene->mMeshes[nd->mMeshes[i]];

        int num = mesh->mNumFaces * 3;
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
        drawObject.indicesBuffer = indiceBuffer;
        drawObject.numIndices = numIndices;
        (&drawObjects)->push_back(drawObject);
        drawCall++;
    }
    for (int i = 0; i < nd->mNumChildren; ++i) {
        this->recursiveGenBuffers(sc, nd->mChildren[i]);
    }
}
