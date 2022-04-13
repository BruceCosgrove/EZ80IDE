#type vertex
#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 localPosition;
layout(location = 2) in float thickness;
layout(location = 3) in vec4 color;

layout(std140, binding = 0) uniform Camera
{
	mat4 viewProjection;
};

struct VertexOutput
{
	vec2 localPosition;
	float thickness;
	vec4 color;
};

layout(location = 0) out VertexOutput Output;

void main()
{
	Output.localPosition = localPosition;
	Output.thickness = thickness;
	Output.color = color;
	gl_Position = viewProjection * vec4(position, 1.0);
}

#type fragment
#version 450 core

struct VertexOutput
{
	vec2 localPosition;
	float thickness;
	vec4 color;
};

layout(location = 0) in VertexOutput Input;

layout(location = 0) out vec4 outColor;

void main()
{
	const float fade = 0.01;
	float dist = sqrt(dot(Input.localPosition, Input.localPosition));
	float t = 1.0 - Input.thickness;
	if (dist > 1.0 || dist < t - fade)
		discard;

	float alpha = (1.0 - smoothstep(1.0 - fade, 1.0, dist)) * smoothstep(t - fade, t, dist);
	outColor = vec4(Input.color.rgb, Input.color.a * alpha);
}
