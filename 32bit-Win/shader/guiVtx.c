#version 120

attribute vec2 vPos;
uniform vec4 vMod;

void main()
{
	gl_Position = vec4
	(
		vMod.x + vPos.x*vMod.z, 
		vMod.y + vPos.y*vMod.w, 
		0, 
		1
	);
	
}

