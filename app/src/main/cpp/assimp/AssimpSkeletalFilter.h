//
// Created by templechen on 2020-01-02.
//

#ifndef ASSIMPVIEWER_ASSIMPSKELETALFILTER_H
#define ASSIMPVIEWER_ASSIMPSKELETALFILTER_H


#include "AssimpBaseFilter.h"
#include "ogldev_math_3d.h"

using namespace std;

#define NUM_BONES_PER_VEREX 4
#define MAX_BONES 100

class AssimpSkeletalFilter : public AssimpBaseFilter {

public:
    struct VertexBoneData {
        float IDs[NUM_BONES_PER_VEREX];
        float Weights[NUM_BONES_PER_VEREX];

        VertexBoneData() {
            Reset();
        };

        void Reset() {
            ZERO_MEM(IDs);
            ZERO_MEM(Weights);
        }

        void AddBoneData(uint BoneID, float Weight);
    };

    struct BoneInfo {
        Matrix4f BoneOffset;
        Matrix4f FinalTransformation;

        BoneInfo() {
            BoneOffset.SetZero();
            FinalTransformation.SetZero();
        }
    };

    void doFrame() override;

    void initShaders() override;

protected:
    void import3DModel() override;

protected:
    void recursiveGenBuffers(const struct aiScene *sc, const struct aiNode *nd) override;

    float GetRunningTime();

    void BoneTransform(float TimeInSeconds, vector<Matrix4f> &Transforms);

    void
    ReadNodeHeirarchy(float AnimationTime, const aiNode *pNode, const Matrix4f &ParentTransform);

    const aiNodeAnim *FindNodeAnim(const aiAnimation *pAnimation, const string NodeName);

    void CalcInterpolatedScaling(aiVector3D &Out, float AnimationTime, const aiNodeAnim *pNodeAnim);
    void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);

    uint FindScaling(float AnimationTime, const aiNodeAnim *pNodeAnim);
    uint FindRotation(float AnimationTime, const aiNodeAnim *pNodeAnim);
    uint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);

    GLint GetUniformLocation(const char* pUniformName);
public:
    void loadObj() override;

private:

    std::map<const char *, const aiNode *> mBoneNodesByName;
    GLuint m_boneLocation[MAX_BONES];
//    vector<VertexBoneData> Bones;
    long numVertices;
    map<string, uint> m_BoneMapping;
    int m_NumBones;
    vector<BoneInfo> m_BoneInfo;
    long totalNumVertices;

    Matrix4f m_GlobalInverseTransform;
    long long m_startTime;
    GLuint boneBuffer;
};


#endif //ASSIMPVIEWER_ASSIMPSKELETALFILTER_H
