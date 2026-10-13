#version 330 core
layout (location = 0) in vec3  aPos;
layout (location = 1) in vec3  aNormal;
layout (location = 2) in vec2  aTexCoord;

layout (location = 3) in ivec4 aJointIds;
layout (location = 4) in vec4  aWeights;

// adjust based on model
#define MAX_JOINTS 261
#define MAX_WEIGHTS 4

out vec2 TexCoord;
out vec3 normal;
out vec4 weights;
flat out ivec4 jointIds;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 jointFinalMatrices[MAX_JOINTS];

void main()
{
	mat4 skinMat = jointFinalMatrices[aJointIds.x] * aWeights.x +
				   jointFinalMatrices[aJointIds.y] * aWeights.y +
				   jointFinalMatrices[aJointIds.z] * aWeights.z +
				   jointFinalMatrices[aJointIds.w] * aWeights.w ;
	vec4 skinPos = vec4(aPos, 1.0) * skinMat;
	
	gl_Position = skinPos * model * view * projection;
	// gl_Position = vec4(aPos, 1.0) * model * view * projection;
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
	
	normal = normalize(aNormal);
	weights = aWeights;
	jointIds = aJointIds;
}
