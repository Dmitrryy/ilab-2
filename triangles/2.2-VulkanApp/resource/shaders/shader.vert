#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
	mat4 view;
	mat4 proj;
} ubo;

layout(binding = 1) uniform UniformEntityInfo {
	mat4 model;
	vec3 normal;
	vec3 color;
} uei[];

layout(location = 0) in vec3 inPosition;
layout(location = 1) in uint entityId;

layout(location = 0) out vec3 fragColor;

const vec3 light1 = normalize(vec3(0.0, -1.0, -1.0));
const vec3 light2 = normalize(vec3(0.0, 1.0, -1.0));
const float minLight = 0.2;

void main() 
{
    gl_Position = ubo.proj * ubo.view * uei[entityId].model * vec4(inPosition, 1.0);
    fragColor = uei[entityId].color 
			* min(1.0, max(minLight, abs(dot(uei[entityId].normal, light1)) + abs(dot(uei[entityId].normal, light2))));
}