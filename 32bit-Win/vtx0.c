#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec2 TXR;
// �A�E�g�v�b�g�f�[�^�B�e�t���O�����g�ŏ������܂�܂��B
out vec2 TXR_0;

void main(){




    gl_Position = vec4(vPosition, 1);
	
    // ���_��UV���W�ł��B���ʂȋ�Ԃ͂���܂���B
    TXR_0 = TXR;
}

