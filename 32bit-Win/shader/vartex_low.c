#version 330 core


attribute vec3 vPosition;
attribute vec3 vColor;

attribute vec3 vPos1;
attribute vec3 vPos2;
attribute vec3 vPos3;

attribute vec2 tPos1;
attribute vec2 tPos2;
attribute vec2 tPos3;


uniform vec2 scl_rad;
uniform vec3 objRot;
uniform vec3 objStd;
uniform vec3 locR;
uniform float revMd;

uniform mat4 MVP;

varying vec4 fpNorm;	// �@��
varying float fn3Rt;
varying float fr;

varying vec2 txr[3];
varying vec3 fCol;
varying vec4 ptsE1, ptsE2, ptsE3;


float PIE = 3.1415926535;


//-----------------------------------
void clcSlope4(vec4 drawP1, vec4 drawP2, out vec4 slopeC, out vec4 betaC);///ok
void tudeRst(inout float, inout float, float, int);///ok
void all_tudeRst_0(inout vec4 vect, vec3 locT);///ok
void all_tudeRst_1(inout vec4 vect, vec3 locT);///ok
vec4 tudeToEuc(vec3 locT);
vec3 eucToTude(vec4 vecT);	 ///ok
float pyth2(float x, float y);
float pyth3(float x, float y, float z);
float atan2(float x, float y);
float pyth4(vec4 vec);
//-----------------------------------


float pyth4(vec4 vec){
	 return sqrt( vec.x*vec.x +vec.y*vec.y +vec.z*vec.z +vec.w*vec.w ); 
}///ok
void clcSlope4(vec4 drawP1, vec4 drawP2, out vec4 slopeC, out vec4 betaC)///ok
{	///-- 4���������̌X�����v�Z
	///--
	
	//	adjY4(&drawP1, gen1);	//--�l����
	slopeC.x = (drawP1.x-drawP2.x)/(drawP1.y-drawP2.y);
	slopeC.z = (drawP1.z-drawP2.z)/(drawP1.y-drawP2.y);
	slopeC.w = (drawP1.w-drawP2.w)/(drawP1.y-drawP2.y);
	betaC.x = drawP1.x-(slopeC.x*drawP1.y);
	betaC.z = drawP1.z-(slopeC.z*drawP1.y);
	betaC.w = drawP1.w-(slopeC.w*drawP1.y);
}
void tudeRst(inout float vec_1, inout float vec_2, float locT, int mode)///ok
{//-- ��,�o,�[���Z�b�g��]
	float tRot = atan2(vec_1, vec_2);
	float R = pyth2(vec_1, vec_2);
	if(0==mode){
		vec_1 = R * sin(tRot - locT);
		vec_2 = R * cos(tRot - locT);
	}else{
		vec_1 = R * sin(tRot + locT);
		vec_2 = R * cos(tRot + locT);
	}
}
void all_tudeRst_0(inout vec4 vect, vec3 locT)///ok
{//-- ��,�o,�[���Z�b�g��]
		tudeRst(vect.x, vect.y, locT.x, 0);//-- X-Y ��]
		tudeRst(vect.y, vect.z, locT.y, 0);//-- Y-Z ��]
		tudeRst(vect.z, vect.w, locT.z, 0);//-- Z-W ��]
}
void all_tudeRst_1(inout vec4 vect, vec3 locT)///ok
{//-- ��,�o,�[���Z�b�g��]
		tudeRst(vect.z, vect.w, locT.z, 1);//-- Z-W ��]
		tudeRst(vect.y, vect.z, locT.y, 1);//-- Y-Z ��]
		tudeRst(vect.x, vect.y, locT.x, 1);//-- X-Y ��]
}

vec4 tudeToEuc(vec3 locT){	// [��,�o,�[]���W��[XYZ]W���W�ɕϊ�
	vec4 vecT = vec4( 
				 sin(locT.z) * sin(locT.y) * sin(locT.x),//X
				 sin(locT.z) * sin(locT.y) * cos(locT.x),//Y
				 sin(locT.z) * cos(locT.y),		//Z
				 cos(locT.z)	//W
	);
	return vecT;
}
vec3 eucToTude(vec4 vecT){	 ///ok
	vec3 locT;
	locT.x = atan2(vecT.x, vecT.y);								
	locT.y = atan2(pyth2(vecT.x,vecT.y), vecT.z);	
	locT.z = atan2(pyth3(vecT.x,vecT.y,vecT.z), vecT.w);

	return locT;
}
float pyth2(float x, float y){ return sqrt(pow(x,2.0)+pow(y,2.0)); }///ok
float pyth3(float x, float y, float z){ return sqrt(pow(x,2.0)+pow(y,2.0)+pow(z,2.0)); }///ok
float atan2(float x, float y){///ok
	float deg = atan(x / y); deg += PIE*float(y<0.0);
	if(x==0.0){
		deg = 0.0; deg += PIE*float(y<0.0);
	}
	if(y==0.0){
		deg = 0.5*PIE; deg += PIE*float(x<0.0);
	}
	return deg;
}

void setVertex(inout vec3 vPos)
{
		vec4 pts1 = vec4(	// �n�_ XYZW
			0.0,
			sin(scl_rad[0] * vPos.z / scl_rad[1]),
			0.0,
			cos(scl_rad[0] * vPos.z / scl_rad[1])
		);
		tudeRst(pts1.z, pts1.y, vPos.y, 1);	// �p�x1
		tudeRst(pts1.x, pts1.z, vPos.x, 1);	// �p�x2

		///-- ���]�̔��f
		tudeRst(pts1.x, pts1.y, objRot.z, 1);	// �p�x3
		tudeRst(pts1.y, pts1.z, objRot.y, 1);	// �p�x2
		tudeRst(pts1.x, pts1.z, objRot.x, 1);	// �p�x1

		///-- �(��])�ɍ��킹��
		tudeRst(pts1.x, pts1.y, objStd[2], 1);	// ����3
		tudeRst(pts1.y, pts1.z, objStd[1], 1);	// ����2
		tudeRst(pts1.x, pts1.y, objStd[0], 1);	// ����1
		///-- �I�u�W�F�N�g���W�ɍ��킹��
		all_tudeRst_1(pts1, locR);	// ����3
		//---- ���W�ϊ����Ċi�[ (end ----
		vPos = eucToTude(pts1); 
	
}

//--------------=============================
//--------------=============================
//--------------=============================



void main()
{
	vec3 vPosX = vPosition;
	vec3 vPos[3] = vec3[](vPos1, vPos2, vPos3);
	

	for(int i=0;i<3;i++)
		setVertex(vPos[i]);
	
	setVertex(vPosX);
	
	
	vec3 drawPts = tudeToEuc(vPosX).xyz;
	tudeRst(drawPts.y, drawPts.z, 0.5*PIE, 1); //-- Y�������s���� (�݊��̂���)
	drawPts.z *= -1.0;
	
	//------------------------------------------------
	// ���W�I���g���V�F�[�_�ōs���Ă���������
	//--------------------------------
	
	//-- �F�ݒ�
	fCol = vColor;

	//-- �ʒu���
	vec4 a = ptsE1 = tudeToEuc( vPos[0] );
	vec4 b = ptsE2 = tudeToEuc( vPos[1] );
	vec4 c = ptsE3 = tudeToEuc( vPos[2] );
	
	//-- �e�X�N�`��
	txr[0] = tPos1;
	txr[1] = tPos2;
	txr[2] = tPos3;


	///-- �_���R�s�[, EUC���W�ɕϊ�, Y-Z���Ԃ�90�x��], Z���𔽓]
	vec3 drawPt[3], drawPt_td[3];
	vec4 drawP4[3];
	for(int j=0;j<3;j++){	
		drawPt_td[j] = vPos[j];
		drawP4[j] = tudeToEuc(drawPt_td[j]);
		drawPt[j] = drawP4[j].xyz;
			
		tudeRst(drawPt[j].y, drawPt[j].z, 0.5*PIE, 1); //-- Y�������s���� (�݊��̂���)
		drawPt[j].z *= -1.0;
	}
	////---�v�Z�̂��߂ɒly�𒲐� > ����ɃJ�����O�ʂ̐�����؂�o�� ---///
	//bool nRev = adjY_pls(drawPt);

//////////////////////////////////aaaa
	/// �x�N�g����(����)
	vec4 prod = vec4
	(
		-a[1]*b[2]*c[3] -a[2]*b[3]*c[1] -a[3]*b[1]*c[2] 
		+a[1]*b[3]*c[2] +a[2]*b[1]*c[3] +a[3]*b[2]*c[1],

		+a[0]*b[2]*c[3] +a[2]*b[3]*c[0] +a[3]*b[0]*c[2]
		-a[0]*b[3]*c[2] -a[2]*b[0]*c[3] -a[3]*b[2]*c[0],

		-a[0]*b[1]*c[3] -a[1]*b[3]*c[0] -a[3]*b[0]*c[1]
		+a[0]*b[3]*c[1] +a[1]*b[0]*c[3] +a[3]*b[1]*c[0],

		+a[0]*b[1]*c[2] +a[1]*b[2]*c[0] +a[2]*b[0]*c[1]
		-a[0]*b[2]*c[1] -a[1]*b[0]*c[2] -a[2]*b[1]*c[0]
	);
	float prodRate = pyth4(prod);
	fpNorm = prod*(1.0/prodRate);
	fn3Rt = 1.0/pyth3(fpNorm.x, fpNorm.y, fpNorm.z);
	
	
	fr = revMd;
////////////////////////////////////////aaaa
	
	// ���_���Z�͕��ʂ�3D�Ɠ��l
	drawPts *= revMd;
	gl_Position = MVP * vec4(drawPts, 1.0);


	
}

