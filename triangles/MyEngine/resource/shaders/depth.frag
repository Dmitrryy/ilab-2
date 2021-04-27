/*************************************************************************
 *
 * depth.frag
 *
 * Created by dmitry
 * 27.02.2021
 *
 ***/

#version 450


// input vertex atributes
//=======================================================================================
layout(location = 0) in vec3 inFragColor;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inFragPosition;
layout(location = 3) in vec3 inLightPosition;
layout(location = 4) in float inLightFarPlane;
//=======================================================================================
//=======================================================================================


void main()
{
    float lightDistance = length(inFragPosition - inLightPosition);

    lightDistance = lightDistance / inLightFarPlane;

    gl_FragDepth = lightDistance;
}