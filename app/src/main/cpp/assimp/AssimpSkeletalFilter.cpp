//
// Created by templechen on 2020-01-02.
//

#include <native_log.h>
#include "AssimpSkeletalFilter.h"

void AssimpSkeletalFilter::recursiveGenBuffers(const struct aiScene *sc, const struct aiNode *nd) {
    for (int i = 0; i < nd->mNumMeshes; ++i) {
        const struct aiMesh *mesh = scene->mMeshes[nd->mMeshes[i]];

        int num = mesh->mNumFaces * 3;
        int32_t stride = sizeof(SHADER_VERTEX);
        auto *p = new SHADER_VERTEX[num];
        int index = 0;
        for (int t = 0; t < mesh->mNumFaces; ++t) {
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
