#version 330 core

// from vertex shader
in vec3 vPos;
in vec2 txr;
in vec3 fCol;

// uniform data
//...

// output data
out vec3 color;


void main()
{
	vec2 gl = vec2(gl_FragCoord.x, gl_FragCoord.y);

	// [“x‚ÌZo

	float dec;
	//...


	color = vec3(1.0, 1.0, 1.0);
	//gl_FragDepth = 1.001 - dec;

}