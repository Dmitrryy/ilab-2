
#version 450

#extension GL_ARB_separate_shader_objects : enable

//#extension GL_EXT_nonuniform_qualifier : require
#extension GL_ARB_shader_draw_parameters : require

layout(set = 0, binding = 0) uniform UniformBufferObject {
	mat4 view;
	mat4 proj;
} ubo;

layout(set = 0, binding = 1) readonly buffer object_transform
{
    mat4 model_matrix[];
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in uint entityId;

layout(location = 0) out vec3 fragColor;

vec3 light1 = normalize(vec3(0.0, -1.0, -1.0));
vec3 light2 = normalize(vec3(0.0, 1.0, -1.0));
float minLight = 0.2;

void main() 
{
    gl_Position = ubo.proj * ubo.view * model_matrix[  entityId  ] * vec4(inPosition, 1.0);
    fragColor = inColor * min(1.0, max(minLight, abs(dot(inNormal, light1)) + abs(dot(inNormal, light2))));
}