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
    lowp vec4 colorSpecular = vec4(vMaterialSpecular.xyz * specular, 1);
    // increase ambient light to brighten the teapot :-)
    //            gl_FragColor = diffuseLight * texture2D(samplerObj, texCoord) +
    //            2.0f * vec4(vMaterialAmbient.xyz, 1.0f) + colorSpecular;
    gl_FragColor = texture2D(samplerObj, texCoord);
    //            gl_FragColor = vec4(vMyColor, 1.0);
    //            gl_FragColor = mix(texture2D(samplerObj, texCoord), vec4(vMyColor, 1.0), 0.5);
}