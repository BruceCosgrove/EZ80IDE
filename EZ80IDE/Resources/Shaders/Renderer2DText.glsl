#type vertex
#version 450 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 tintColor;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in uint texIndex;
layout(location = 4) in vec2 curveInfo;

layout(std140, binding = 0) uniform Camera
{
	mat4 viewProjection;
};

struct VertexOutput
{
	vec2 texCoord;
	vec4 tintColor;
};

struct CurveInfo
{
	float width;
	float fade;
};

layout(location = 0) out VertexOutput Output;
layout(location = 2) out flat uint Output_texIndex;
layout(location = 3) out flat CurveInfo Output_curveInfo;

void main()
{
	Output.texCoord = texCoord;
	Output.tintColor = tintColor;
	Output_texIndex = texIndex;
	Output_curveInfo.width = curveInfo.x;
	Output_curveInfo.fade = curveInfo.y;
	gl_Position = viewProjection * vec4(position, 0.0, 1.0);
}

#type fragment
#version 450 core

struct VertexOutput
{
	vec2 texCoord;
	vec4 tintColor;
};

struct CurveInfo
{
	float width;
	float fade;
};

layout(location = 0) in VertexOutput Input;
layout(location = 2) in flat uint Input_texIndex;
layout(location = 3) in flat CurveInfo Input_curveInfo;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D textures[32];

vec4 GetTextureColor(uint index, vec2 uv)
{
	switch (index)
	{
		case 0: return texture(textures[0], uv);
		case 1: return texture(textures[1], uv);
		case 2: return texture(textures[2], uv);
		case 3: return texture(textures[3], uv);
		case 4: return texture(textures[4], uv);
		case 5: return texture(textures[5], uv);
		case 6: return texture(textures[6], uv);
		case 7: return texture(textures[7], uv);
		case 8: return texture(textures[8], uv);
		case 9: return texture(textures[9], uv);
		case 10: return texture(textures[10], uv);
		case 11: return texture(textures[11], uv);
		case 12: return texture(textures[12], uv);
		case 13: return texture(textures[13], uv);
		case 14: return texture(textures[14], uv);
		case 15: return texture(textures[15], uv);
		case 16: return texture(textures[16], uv);
		case 17: return texture(textures[17], uv);
		case 18: return texture(textures[18], uv);
		case 19: return texture(textures[19], uv);
		case 20: return texture(textures[20], uv);
		case 21: return texture(textures[21], uv);
		case 22: return texture(textures[22], uv);
		case 23: return texture(textures[23], uv);
		case 24: return texture(textures[24], uv);
		case 25: return texture(textures[25], uv);
		case 26: return texture(textures[26], uv);
		case 27: return texture(textures[27], uv);
		case 28: return texture(textures[28], uv);
		case 29: return texture(textures[29], uv);
		case 30: return texture(textures[30], uv);
		case 31: return texture(textures[31], uv);
		default: return vec4(0.0);
	}
}

void main()
{
	float distanceToCurve = 1.0 - GetTextureColor(Input_texIndex, Input.texCoord).r;
	float alpha = 1.0 - smoothstep(Input_curveInfo.width, Input_curveInfo.width + Input_curveInfo.fade, distanceToCurve);
	if (alpha == 0.0) discard;

	outColor = Input.tintColor;
	outColor.a *= alpha;
}
