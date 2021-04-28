/*************************************************************************
 *
 * reflection.frag
 *
 * Created by dmitry
 * 24.04.2021
 *
 ***/

#version 450


// push constants
//=======================================================================================
layout( push_constant ) uniform constants
{
    mat4 viewProjMatrix;
    vec3 cameraPosition;
} pushConstants;
//=======================================================================================
//=======================================================================================



layout (set = 2, binding = 0) uniform samplerCube samplerCubeMap;

layout (location = 0) in vec3 inUVW;
layout (location = 1) in vec3 inNormal;

layout (location = 0) out vec4 outFragColor;

void main()
{
    vec3 I = normalize(inUVW - pushConstants.cameraPosition);
    vec3 R = reflect(I, inNormal);
    outFragColor = vec4(texture(samplerCubeMap, R).rgb, 1.0);
}