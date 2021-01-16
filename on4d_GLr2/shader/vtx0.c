#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 TXR;
// アウトプットデータ。各フラグメントで書き込まれます。
out vec2 TXR_0;

void main(){




    gl_Position = vec4(vPosition, 1);
	
    // 頂点のUV座標です。特別な空間はありません。
    TXR_0 = TXR;
}

