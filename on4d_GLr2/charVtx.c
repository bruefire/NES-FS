#version 120

attribute vec2 vPos;
attribute vec2 tPos;

uniform vec4 vMod;

varying vec2 txr;

void main()
{
	txr = tPos;
	
	gl_Position = vec4
	(
		vMod.x + vPos.x*vMod.z, 
		vMod.y + vPos.y*vMod.w, 
		0, 
		1
	);
	
}

