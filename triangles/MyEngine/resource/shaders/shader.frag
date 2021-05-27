/*************************************************************************
 *
 * shader.frag
 *
 * Created by dmitry
 * 22.02.2021
 *
 ***/

#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shader_draw_parameters : require



struct lightInfo
{
    vec3 position;
    vec3 color;
    float farPlane;
    float epsilon;
    float shadowOpacity;
};


// push constants
//=======================================================================================
layout( push_constant ) uniform constants
{
    mat4 viewProjMatrix;
    vec3 cameraPosition;
} pushConstants;
//=======================================================================================
//=======================================================================================


// set 0
//=======================================================================================
layout (set = 1, binding = 0) uniform samplerCube shadowCubeMap;

layout (set = 1, binding = 1) readonly buffer lightsInfo
{
    lightInfo light_info[];
};
//=======================================================================================
//=======================================================================================


// input vertex atributes
//=======================================================================================
layout(location = 0) in vec3 inFragColor;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inFragPosition;
layout(location = 3) in vec3 inLightPosition;
layout(location = 4) in float inLightFarPlane;
layout(location = 5) in vec3 inLightColor;
//=======================================================================================
//=======================================================================================


// output atributes
//=======================================================================================
layout(location = 0) out vec4 outFragColor;
//=======================================================================================
//=======================================================================================


#define EPSILON 0.05
#define SHADOW_OPACITY 0.5

float ShadowCalculation(vec3 fragPos, vec3 lightPos, float farPlane, samplerCube shadowMap);



void main() {

    vec3 normal = normalize(inNormal);
    // фоновое освещение
    vec3 ambient = 0.3 * inFragColor;
    // диффузный компонент
    vec3  lightDir = normalize(inLightPosition - inFragPosition);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3  diffuse = diff * inLightColor;

    // Shadow
    float shadow = ShadowCalculation(inFragPosition, inLightPosition, inLightFarPlane, shadowCubeMap);

    outFragColor = vec4((ambient + shadow * diffuse) * inFragColor, 1.0);
}




float ShadowCalculation(vec3 fragPos, vec3 lightPos, float farPlane, samplerCube shadowMap)
{
    // расчет вектора между положением фрагмента и положением источника света
    vec3 fragToLight = fragPos - lightPos;
    // полученный вектор направления от источника к фрагменту
    // используется для выборки из кубической карты глубин
    float closestDepth = texture(shadowMap, fragToLight).r;
    // получено линейное значение глубины в диапазоне [0,1]
    // проведем обратную трансформацию в исходный диапазон
    closestDepth *= farPlane;
    // получим линейное значение глубины для текущего фрагмента
    // как расстояние от фрагмента до источника света
    float dist = length(fragToLight);
    //float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.3;
    float shadow = (dist <= closestDepth + EPSILON) ? 1.0 : SHADOW_OPACITY;

    return shadow;
}