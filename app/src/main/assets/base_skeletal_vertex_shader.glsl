precision mediump float;
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
    highp vec4 p = BoneTransform * vec4(myVertex, 1);
    //            highp vec4 p = vec4(myVertex,1);
    gl_Position = uPMatrix * p;
    texCoord = myUV;
    highp vec3 worldNormal = vec3(mat3(uMVMatrix[0].xyz, uMVMatrix[1].xyz, uMVMatrix[2].xyz) * myNormal);
    highp vec3 ecPosition = p.xyz;
    diffuseLight = dot(worldNormal, normalize(-vLight0+ecPosition)) * vec4(1.0, 1.0, 1.0, 1.0);
    normal = worldNormal;
    position = ecPosition;
}
