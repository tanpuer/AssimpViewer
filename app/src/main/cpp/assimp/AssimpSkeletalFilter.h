//
// Created by templechen on 2020-01-02.
//

#ifndef ASSIMPVIEWER_ASSIMPSKELETALFILTER_H
#define ASSIMPVIEWER_ASSIMPSKELETALFILTER_H


#include "AssimpBaseFilter.h"
#include "math_3d.h"

using namespace std;

#define NUM_BONES_PER_VEREX 4
#define MAX_BONES 100

class AssimpSkeletalFilter : public AssimpBaseFilter {

public:
    struct VertexBoneData {
        GLfloat IDs[NUM_BONES_PER_VEREX];
        GLfloat Weights[NUM_BONES_PER_VEREX];

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

    void loadObj() override;

    void setJavaAssetManager(jobject javaAssetManager, JavaVM *javaVm) override;

    void release() override;

protected:
    void import3DModel() override;

    void recursiveGenBuffers(const struct aiScene *sc, const struct aiNode *nd) override;

    float GetRunningTime();

    void BoneTransform(float TimeInSeconds, vector<Matrix4f> &Transforms);

    void
    ReadNodeHeirarchy(float AnimationTime, const aiNode *pNode, const Matrix4f &ParentTransform);

    const aiNodeAnim *FindNodeAnim(const aiAnimation *pAnimation, const string NodeName);

    void CalcInterpolatedScaling(aiVector3D &Out, float AnimationTime, const aiNodeAnim *pNodeAnim);

    void
    CalcInterpolatedRotation(aiQuaternion &Out, float AnimationTime, const aiNodeAnim *pNodeAnim);

    void
    CalcInterpolatedPosition(aiVector3D &Out, float AnimationTime, const aiNodeAnim *pNodeAnim);

    uint FindScaling(float AnimationTime, const aiNodeAnim *pNodeAnim);

    uint FindRotation(float AnimationTime, const aiNodeAnim *pNodeAnim);

    uint FindPosition(float AnimationTime, const aiNodeAnim *pNodeAnim);

    GLint GetUniformLocation(const char *pUniformName);

    void getCameraInfo();

private:

    GLuint m_boneLocation[MAX_BONES];
    long numVertices;
    map<string, uint> m_BoneMapping;
    int m_NumBones;
    vector<BoneInfo> m_BoneInfo;
    long totalNumVertices;
    Matrix4f m_GlobalInverseTransform;
    long long m_startTime;
    jobject javaAssetManager = NULL;
    JNIEnv *env = nullptr;
    JavaVM *javaVm = nullptr;
};


#endif //ASSIMPVIEWER_ASSIMPSKELETALFILTER_H
