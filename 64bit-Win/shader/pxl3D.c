#version 330 core

// Ouput data
in vec3 col;
out vec4 color;
uniform float alpha;

//----------------------------------------
void main()
{

	color = vec4(col.xyz, alpha);
	if(alpha < -0.1)
		color = vec4(0.0, 0.0, 0.0, 0.1);

}
