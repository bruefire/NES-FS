#version 330 core


layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vColor;
uniform float asp;

uniform mat4 MVP;
uniform float scale;
uniform float direct;
uniform vec3 loc;
out vec3 col;

void main()
{
	vec3 adjPos;
	if(direct < 1.9)
		adjPos = vec3(vPos.x*direct, vPos.y*direct, vPos.z) * scale;
	else
		adjPos = vec3(vPos.x, -vPos.z, vPos.y) * scale;
	
	adjPos = vec3(
		((adjPos.x + loc.x) + 1.0) * 0.25 - asp,
		adjPos.y + loc.y,
		((adjPos.z + loc.z) + 1.0) * 0.25 - 1.0
	);
    gl_Position = MVP * vec4(adjPos, 1.0);
	col = vColor;
}
