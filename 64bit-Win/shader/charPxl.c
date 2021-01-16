#version 120

varying vec2 txr;
uniform sampler2D uTex;

void main()
{
	vec4 tmp = texture2D(uTex,txr);
	gl_FragColor = tmp;
}