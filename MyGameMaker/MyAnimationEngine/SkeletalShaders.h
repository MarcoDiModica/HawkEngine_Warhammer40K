#pragma once

const char* vertexShaderSource = R"(
	#version 440 core
	layout (location = 0) in vec3 position; 
	layout (location = 1) in vec3 normal;
	layout (location = 2) in vec2 uv;
	layout (location = 3) in vec4 boneIds;
	layout (location = 4) in vec4 boneWeights;

	out vec2 tex_cord;
	out vec3 v_normal;
	out vec3 v_pos;
	out vec4 bw;

	uniform mat4 bone_transforms[50];
	uniform mat4 view_projection_matrix;
	uniform mat4 model_matrix;

	void main()
	{
		bw = vec4(0);
		if(int(boneIds.x) == 1)
		bw.z = boneIds.x;
		//boneWeights = normalize(boneWeights);
		mat4 boneTransform  =  mat4(0.0);
		boneTransform  +=    bone_transforms[int(boneIds.x)] * boneWeights.x;
		boneTransform  +=    bone_transforms[int(boneIds.y)] * boneWeights.y;
		boneTransform  +=    bone_transforms[int(boneIds.z)] * boneWeights.z;
		boneTransform  +=    bone_transforms[int(boneIds.w)] * boneWeights.w;
		vec4 pos =boneTransform * vec4(position, 1.0);
		gl_Position = view_projection_matrix * model_matrix * pos;
		v_pos = vec3(model_matrix * boneTransform * pos);
		tex_cord = uv;
		v_normal = mat3(transpose(inverse(model_matrix * boneTransform))) * normal;
		v_normal = normalize(v_normal);
	}

	)";
const char* fragmentShaderSource = R"(
	#version 440 core

	in vec2 tex_cord;
	in vec3 v_normal;
	in vec3 v_pos;
	in vec4 bw;
	out vec4 color;

	uniform sampler2D diff_texture;

	vec3 lightPos = vec3(0.2, 1.0, -3.0);
	
	void main()
	{
		vec3 lightDir = normalize(lightPos - v_pos);
		float diff = max(dot(v_normal, lightDir), 0.2);
		vec3 dCol = diff * texture(diff_texture, tex_cord).rgb; 
		color = vec4(dCol, 1);
	}
	)";
