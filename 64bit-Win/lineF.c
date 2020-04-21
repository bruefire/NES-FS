#version 330 core

// Ouput data
in vec3 fCol;
in float Dec;
out vec3 color;

uniform int bgMode;

float PIE = 3.1415926535;


//----------------------------------------
void main()
{
	
	gl_FragDepth = 1.001-Dec;

	vec3 white = vec3(1,1,1);
	if(bgMode==1) color = (white - fCol)*Dec; 
	else color = fCol*Dec;

}
