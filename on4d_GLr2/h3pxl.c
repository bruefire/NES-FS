#version 330 core

// from vertex shader
in vec3 vPos;
in vec2 txr;
in vec3 fCol;

// uniform data
uniform vec3 locR;

// output data
out vec3 color;


// functions (declare)
float pyth3(float x, float y, float z);


// functions (define)
float pyth3(float x, float y, float z) { return sqrt(x * x + y * y + z * z); }///ok



// ---------> エントリ関数 <----------
void main()
{
	vec2 gl = vec2(gl_FragCoord.x, gl_FragCoord.y);

	// 深度の算出

	float dec;
	//...

	dec = 1.0 - pyth3(locR.x, locR.y, locR.z);
	color = vec3(1.0, dec, 1.0);
	//gl_FragDepth = 1.001 - dec;

}