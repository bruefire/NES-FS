#version 330 core

// Ouput data
out vec4 color;
in vec2 TXR_0;

uniform vec3 cols;
uniform sampler2D sfTex;


//----------------------------------------
void main()
{

	gl_FragDepth = 0;
	color.rgb = texture(sfTex, TXR_0).rgb;
	if(0<color.r + color.g + color.b)color.a = 1.0;

}

