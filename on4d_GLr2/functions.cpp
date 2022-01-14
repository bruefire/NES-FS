#include <stdint.h>
#include <Complex>
#include "constants.h"
#include "geometry.h"
#include "functions.h"



//====Å•ÉsÉ^ÉSÉâÉXÇÃíËóù
double pyth2(double x, double y){ return sqrt(x*x + y*y); }
double pyth3(double x, double y, double z){ return sqrt(x*x + y*y + z*z); }
double pyth4(double w, double x, double y, double z){ return sqrt(w*w + x*x + y*y + z*z); }
double pyth3(pt3 pts) { return sqrt(pts.x * pts.x + pts.y * pts.y + pts.z * pts.z); }
double pyth4(pt4 pts){ return sqrt(pts.w*pts.w + pts.x*pts.x + pts.y*pts.y + pts.z*pts.z); }
double pyth3OS(pt3 pts, double hLen)
{ 
	return sqrt(hLen*hLen - (pts.x * pts.x + pts.y * pts.y + pts.z * pts.z));
}
double pyth4Sq(pt4 pts)
{ 
	return pts.w * pts.w + pts.x * pts.x + pts.y * pts.y + pts.z * pts.z;
}

/// <summary>
/// this function takes integer only as index.
/// </summary>
double powi(double x, int y)
{
	if (y == 0) return 1.0;

	double m = 1.0;
	if (y > 0) 
	{
		for (int i = 0; i < y; i++) m *= x;
	}
	else 
	{
		for (int i = 0; i < -y; i++) m *= x;
		m = 1.0 / m;
	}

	return m;
};

/// <summary>
/// this function returns result value floored as integer.
/// </summary>
int log_floor(double base, double anti_log)
{
	// unsupported values..
	if (base <= 1 || isinf(anti_log) || isnan(anti_log))
		return std::numeric_limits<int>::max();

	int index = 0;

	if(1.0 <= anti_log && anti_log < base)
	{ 
		/* nothing to do */ 
	}
	else if (base <= anti_log)
	{
		double div = 1.0;
		while (true)
		{
			div *= base;
			++index;

			if (anti_log / div < base)
				break;
		}
	}
	else // anti_log < 1.0
	{
		double mul = 1.0;
		while (true)
		{
			mul *= base;
			--index;

			if (anti_log * mul >= 1.0)
				break;
		}
	}

	return index;
}

//=====Å•ílÇÃí≤êÆ=======
void degAdj(pt4* pt2, pt4* pt1, char mode)
{
	if(mode==0 && (*pt2).w-(*pt1).w==0.0) (*pt2).w += pow(0.1, 100);	///Ç∆ÇËÇ†Ç¶Ç∑
	if(mode==1 && (*pt2).x-(*pt1).x==0.0) (*pt2).x += pow(0.1, 100);	///Ç∆ÇËÇ†Ç¶Ç∑
	if(mode==2 && (*pt2).y-(*pt1).y==0.0) (*pt2).y += pow(0.1, 100);	///Ç∆ÇËÇ†Ç¶Ç∑
	if(mode==3 && (*pt2).z-(*pt1).z==0.0) (*pt2).z += pow(0.1, 100);	///Ç∆ÇËÇ†Ç¶Ç∑
}
void degAdj(pt4* pt2, char mode)
{
	if(mode==0 && (*pt2).w==0.0) (*pt2).w += pow(0.1, 100);	///Ç∆ÇËÇ†Ç¶Ç∑
	if(mode==1 && (*pt2).x==0.0) (*pt2).x += pow(0.1, 100);	///Ç∆ÇËÇ†Ç¶Ç∑
	if(mode==2 && (*pt2).y==0.0) (*pt2).y += pow(0.1, 100);	///Ç∆ÇËÇ†Ç¶Ç∑
	if(mode==3 && (*pt2).z==0.0) (*pt2).z += pow(0.1, 100);	///Ç∆ÇËÇ†Ç¶Ç∑
}


//====4dÇ©ÇÁ3dÇ÷===
void pConv4to3(pt4* pts4, pt3* pts3, int len)
{
	for(int i=0;i<len;i++){ pts3[i].x = pts4[i].x; pts3[i].y = pts4[i].y; pts3[i].z = pts4[i].z; }
}

void fConv4to3(face4* fc4, face3* fc3, pt3* pts3)
{
	fc3->col = fc4->col;
	for(char i=0;i<3;i++) fc3->pts[i] = pts3 + fc4->pts[i];
}

//=================
void cubePts(uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, uint32_t* idxs, int len)
{
	for(uint32_t i=0;i<6;i++) idxs[i*5] = 1;
	idxs[1] = p1, idxs[2] = p2, idxs[3] = p3, idxs[4] = p4;
	idxs[6] = p4, idxs[7] = p3, idxs[8] = p3+len, idxs[9] = p4+len;
	idxs[11] = p4+len, idxs[12] = p3+len, idxs[13] = p2+len, idxs[14] = p1+len;
	idxs[16] = p1+len, idxs[17] = p2+len, idxs[18] = p2, idxs[19] = p1;
	idxs[21] = p2, idxs[22] = p2+len, idxs[23] = p3+len, idxs[24] = p3;
	idxs[26] = p1+len, idxs[27] = p1, idxs[28] = p4, idxs[29] = p4+len;

}
void tetraPts(uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, uint32_t* idxs)	//élñ ëÃ
{
	for(uint32_t i=0;i<4;i++) idxs[i*4] = 0;
	idxs[1] = p1, idxs[2] = p2, idxs[3] = p3;
	idxs[5] = p3, idxs[6] = p2, idxs[7] = p4;
	idxs[9] = p4, idxs[10] = p2, idxs[11] = p1;
	idxs[13] = p1, idxs[14] = p4, idxs[15] = p3;

}
void piraPts(uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4, uint32_t p5, uint32_t* idxs)	//éläpêç
{
	for(uint32_t i=0;i<4;i++) idxs[i*4] = 0; idxs[16] = 1;

	idxs[1] = p1, idxs[2] = p5, idxs[3] = p2;
	idxs[5] = p2, idxs[6] = p5, idxs[7] = p3;
	idxs[9] = p3, idxs[10] = p5, idxs[11] = p4;
	idxs[13] = p4, idxs[14] = p5, idxs[15] = p1;
	idxs[17] = p1, idxs[18] = p2, idxs[19] = p3, idxs[20] = p4;

}
void prismPts(uint32_t p1, uint32_t p2, uint32_t p3, uint32_t* idxs, int len)	//éläpíå
{
	for(uint32_t i=0;i<3;i++) idxs[i*5] = 1;
	idxs[15] = 0, idxs[19] = 0;
	
	idxs[1] = p1, idxs[2] = p1+len, idxs[3] = p2+len, idxs[4] = p2;
	idxs[6] = p2, idxs[7] = p2+len, idxs[8] = p3+len, idxs[9] = p3;
	idxs[11] = p3, idxs[12] = p3+len, idxs[13] = p1+len, idxs[14] = p1;
	idxs[16] = p1,		idxs[17] = p2,		idxs[18] = p3;
	idxs[20] = p3+len,	idxs[21] = p2+len,	idxs[22] = p1+len;

}

///-----on4d
void tudeRst(double* vec1, double* vec2, double locT, bool mode){//-- à‹,åo,ê[ÉäÉZÉbÉgâÒì]

	//// pt2ÇégópÇ∑ÇÈÇ∆(?)Ç»Ç∫Ç©íxÇ¢..
	//pt2 axs1(*vec1, *vec2);
	//pt2 axs2 = pt2::cross(axs1);
	//double asign = mode ? 1 : -1;

	//pt2 result = pt2()
	//	.pls(axs1.mtp(cos(asign * locT)))
	//	.pls(axs2.mtp(sin(asign * locT)));

	//*vec1 = result.x;
	//*vec2 = result.y;
	double asign = mode ? 1 : -1;

	double cosVal = cos(asign * locT);
	double sinVal = sin(asign * locT);
	double tVec1 = (*vec1) * cosVal + (*vec2) * sinVal;
	double tVec2 = (*vec2) * cosVal + (-*vec1) * sinVal;

	*vec1 = tVec1;
	*vec2 = tVec2;
}
void all_tudeRst(pt4* vec, pt3 locT, bool mode){//-- à‹,åo,ê[ÉäÉZÉbÉgâÒì]

	if(mode==0){//-- à‹ìx,åoìx,ê[ìxÇ0Ç…
		tudeRst(&vec->x, &vec->y, locT.x, 0);//-- X-Y âÒì]
		tudeRst(&vec->y, &vec->z, locT.y, 0);//-- Y-Z âÒì]
		tudeRst(&vec->z, &vec->w, locT.z, 0);//-- Z-W âÒì]
	}else if(mode==1){		//-- à‹ìx,åoìx,ê[ìxÇñﬂÇ∑
		tudeRst(&vec->z, &vec->w, locT.z, 1);//-- Z-W âÒì]
		tudeRst(&vec->y, &vec->z, locT.y, 1);//-- Y-Z âÒì]
		tudeRst(&vec->x, &vec->y, locT.x, 1);//-- X-Y âÒì]
	}
}
