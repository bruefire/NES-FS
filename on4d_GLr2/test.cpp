#include "stdafx.h"
//--
#include <iostream>
#include <complex>
#include "constants.h"
#include "points.h"

using namespace std;


int permute(int idx1, int idx2, int idx3)
{
	int idx4 = -1;
	for(int i=0;i<4;i++){
		if(idx1!=i && idx2!=i && idx3!=i)
			idx4 = i;
	}
	double idx[] = {idx1, idx2, idx3, idx4};
	int cnt = 0;

	for(int h=0;h<4;h++){
		if(idx[0]==0 && idx[1]==1 && idx[2]==2 && idx[3]==3){
			return (cnt%2==0) ? 1 : 0;
		}

		if(idx[h]!=h){
			int mc = -1;
			for(int i=h+1;i<4;i++){
				if(idx[i]==h){
					mc = i;
					break;
				}
			}
			idx[mc] = idx[h];
			idx[h] = h;
			++cnt;
		}
	}
	cout << "error" << endl;
	return -1;
}
double pyth2(double x, double y){ return sqrt(pow(x,2)+pow(y,2)); }
double pyth3(double x, double y, double z){ return sqrt(pow(x,2)+pow(y,2)+pow(z,2)); }
double pyth4(double w, double x, double y, double z){ return sqrt(pow(w,2)+pow(x,2)+pow(y,2)+pow(z,2)); }
double pyth3(pt3 pts){ return sqrt(pow(pts.x,2)+pow(pts.y,2)+pow(pts.z,2)); }
double pyth4(pt4 pts){ return sqrt(pow(pts.w,2)+pow(pts.x,2)+pow(pts.y,2)+pow(pts.z,2)); }

///-----on4d
void tudeRst(double* vec1, double* vec2, double locT, bool mode){//-- 緯,経,深リセット回転
	
	double tRot = atan2(*vec1, *vec2);

	double R = pyth2(*vec1, *vec2);
	if(!mode){
		*vec1 = R * sin(tRot - locT);
		*vec2 = R * cos(tRot - locT);
	}else{
		*vec1 = R * sin(tRot + locT);
		*vec2 = R * cos(tRot + locT);
	}

}
void all_tudeRst(pt4* vec, pt3 locT, bool mode){//-- 緯,経,深リセット回転

	if(mode==0){//-- 緯度,経度,深度を0に
		tudeRst(&vec->x, &vec->y, locT.x, 0);//-- X-Y 回転
		tudeRst(&vec->y, &vec->z, locT.y, 0);//-- Y-Z 回転
		tudeRst(&vec->z, &vec->w, locT.z, 0);//-- Z-W 回転
	}else if(mode==1){		//-- 緯度,経度,深度を戻す
		tudeRst(&vec->z, &vec->w, locT.z, 1);//-- Z-W 回転
		tudeRst(&vec->y, &vec->z, locT.y, 1);//-- Y-Z 回転
		tudeRst(&vec->x, &vec->y, locT.x, 1);//-- X-Y 回転
	}
}
pt3 eucToTude(pt4 vecT){	// [XYZ]W座標を[緯,経,深]座標に変換
	pt3 locT;
	locT.x = atan2(vecT.x, vecT.y);			
	locT.y = atan2(pyth2(vecT.x,vecT.y), vecT.z);	
	locT.z = atan2(pyth3(vecT.x,vecT.y,vecT.z), vecT.w);

	return locT;
}



int main(array<System::String ^> ^args)
{
	// トーラス
	int n = 32;
	pt4* torusPts = new pt4[n*n];

	pt4 vec = {0,0,0,1};
	tudeRst(&vec.y, &vec.z, PIE/4.0, 1);

	// 頂点
	for(int h=0; h<n; h++)
	{
		pt4 vecXY = vec;
		tudeRst(&vecXY.x, &vecXY.y, 2*PIE*(h*1.0/n), 1);

		for(int i=0; i<n; i++)
		{
			pt4 vecZW = vecXY;
			tudeRst(&vecZW.w, &vecZW.z, 2*PIE*(i*1.0/n), 1);

			torusPts[i + h*n] = vecZW;
		}
	}

	// 辺
	int* torusLines = new int[n*n*2*2];
	for(int h=0; h<n; h++)
	{
		for(int i=0; i<n; i++)
		{
			torusLines[0 + (i + h*n)*2] = h*n + i + 1;
			torusLines[1 + (i + h*n)*2] = h*n + (i+1)%n + 1;
		}
	}
	for(int h=0; h<n; h++)
	{
		for(int i=0; i<n; i++)
		{
			torusLines[0 + (i + (h+n)*n)*2] = h*n + i + 1;
			torusLines[1 + (i + (h+n)*n)*2] = ((h+1)%n)*n + i + 1;
		}
	}

	// .objファイル作成
	string result = "# on4D object\n";

	// 頂点
	for(int i=0; i<n*n; i++)
	{
		pt3 pts3 = eucToTude(torusPts[i]);

		result += "v "
				+ to_string((long double)pts3.x) + " "
				+ to_string((long double)pts3.y) + " "
				+ to_string((long double)pts3.z) + " "
				+"\n";
	}
	// 辺
	for(int i=0; i<n*n*2; i++)
	{
		result += "l "
				+ to_string((long long)torusLines[0+i*2]) + " "
				+ to_string((long long)torusLines[1+i*2]) + " "
				+ "\n";
	}



	delete[] torusPts;
	delete[] torusLines;
	
	//----- objファイル生成

	FILE* c = fopen("torus.obj", "w");
	fwrite(result.c_str(), 1, result.length(), c);

	fclose(c);
	
	int a;
    cout << "created: " <<endl;
	cin >> a;

    return 0;
}


// 120胞体
int cell_120()
{
	const double FAY =  (1.0+sqrt(5.0))/2;
	double p120[600][4] = {};
    
	///=-=-=-=-= (0, 0, ±2, ±2) =-=-=-=-=
	//-- 0,0,2,2
	int idx = 0;
	for(int h=0;h<3;h++){
		for(int i=h+1;i<4;i++){
			p120[idx][h] = 
			p120[idx][i] = 2;
			++idx;
		}
	}
	//-- 0,0,-2,-2
	for(int h=0;h<3;h++){
		for(int i=h+1;i<4;i++){
			p120[idx][h] = 
			p120[idx][i] = -2;
			++idx;
		}
	}
	//-- 0,0,2,-2
	for(int h=0;h<3;h++){
		for(int i=h+1;i<4;i++){
			p120[idx][h] = 2;
			p120[idx][i] = -2;
			++idx;
		}
	}
	//-- 0,0,-2,2
	for(int h=0;h<3;h++){
		for(int i=h+1;i<4;i++){
			p120[idx][h] = -2;
			p120[idx][i] = 2;
			++idx;
		}
	}
    /// (±1, ±1, ±1, ±√5)
	//-- +√5, (+1 *3)
	//-- +√5, (-1 *3)
	//-- -√5, (+1 *3)
	//-- -√5, (-1 *3)
	for(int g=0;g<4;g++){
		for(int h=0;h<4;h++){
			int sgn1 = (g==0 || g==1) ? 1 : -1;
			int sgn2 = (g==0 || g==2) ? 1 : -1;
				
			for(int i=0;i<4;i++) p120[idx][i] = 1 *sgn1;
			p120[idx][h] = sqrt(5.0) *sgn2;
			++idx;
		}
	}

	//-- +√5, +1, (-1 *2)
	//-- +√5, -1, (-1 *2)
	//-- -√5, +1, (-1 *2)
	//-- -√5, -1, (-1 *2)
	for(int g=0;g<4;g++){
		for(int h=0;h<4;h++){
			for(int i=0;i<4;i++){
				if(h==i) continue;
				int sgn1 = (g==0 || g==1) ? 1 : -1;
				int sgn2 = (g==0 || g==2) ? 1 : -1;
				
				for(int j=0;j<4;j++) p120[idx][j] = -1 *sgn2;
				p120[idx][h] = sqrt(5.0) *sgn1;
				p120[idx][i] = 1 *sgn2;

				++idx;
			}
		}
	}

    /// (±ϕ−2, ±ϕ, ±ϕ, ±ϕ)
	//(±1, ±1, ±1, ±√5)のパターンと同じ
	
	//-- a, b*3
	for(int g=0;g<4;g++){
		for(int h=0;h<4;h++){
			int sgn1 = (g==0 || g==1) ? 1 : -1;
			int sgn2 = (g==0 || g==2) ? 1 : -1;
				
			for(int i=0;i<4;i++) p120[idx][i] = FAY *sgn1;
			p120[idx][h] = pow(FAY, -2.0) *sgn2;
			++idx;
		}
	}

	//-- a, b, -b*2
	for(int g=0;g<4;g++){
		for(int h=0;h<4;h++){
			for(int i=0;i<4;i++){
				if(h==i) continue;
				int sgn1 = (g==0 || g==1) ? 1 : -1;
				int sgn2 = (g==0 || g==2) ? 1 : -1;
				
				for(int j=0;j<4;j++) p120[idx][j] = -1*FAY *sgn2;
				p120[idx][h] = pow(FAY, -2.0) *sgn1;
				p120[idx][i] = FAY *sgn2;

				++idx;
			}
		}
	}
	
    /// (±ϕ−1, ±ϕ−1, ±ϕ−1, ±ϕ2)
	//(±1, ±1, ±1, ±√5)のパターンと同じ
	
	//-- a, b*3
	for(int g=0;g<4;g++){
		for(int h=0;h<4;h++){
			int sgn1 = (g==0 || g==1) ? 1 : -1;
			int sgn2 = (g==0 || g==2) ? 1 : -1;
				
			for(int i=0;i<4;i++) p120[idx][i] = pow(FAY, -1.0) *sgn1;
			p120[idx][h] = pow(FAY, 2.0) *sgn2;
			++idx;
		}
	}
	//-- a, b, -b*2
	for(int g=0;g<4;g++){
		for(int h=0;h<4;h++){
			for(int i=0;i<4;i++){
				if(h==i) continue;
				int sgn1 = (g==0 || g==1) ? 1 : -1;
				int sgn2 = (g==0 || g==2) ? 1 : -1;
				
				for(int j=0;j<4;j++) p120[idx][j] = -1*pow(FAY, -1.0) *sgn2;
				p120[idx][h] = pow(FAY, 2.0) *sgn1;
				p120[idx][i] = pow(FAY, -1.0) *sgn2;

				++idx;
			}
		}
	}
    cout << idx <<endl;



	///(0, ±ϕ−2, ±1, ±ϕ2)
	double val[] = {0, pow(FAY, -2.0), 1, pow(FAY, 2.0)};

	for(int g=0;g<8; g++){//-- パターン
		for(int h=0;h<4;h++){
			for(int i=0;i<4;i++){
				if(i==h) continue;// 同一
				for(int j=0;j<4;j++){
					if(j==i || j==h) continue;// 同一
					if(permute(h,i,j)!=1) continue;
					for(int k=0;k<4;k++) p120[idx][k] = val[3] *(double)((g%2==0)?1:-1);

					p120[idx][h] = val[0];
					p120[idx][i] = val[1] *(double)((g%8<4)?1:-1);
					p120[idx][j] = val[2] *(double)((g%4<2)?1:-1);

					++idx;
				}
			}
		}
	}
    cout << idx << endl;


	///(0, ±ϕ−1, ±ϕ, ±√5)
	//(0, ±ϕ−2, ±1, ±ϕ2)のパターンと同じ
	val[0] = 0;
	val[1] = pow(FAY, -1.0);
	val[2] = FAY;
	val[3] = sqrt(5.0);

	for(int g=0;g<8; g++){//-- パターン
		for(int h=0;h<4;h++){
			for(int i=0;i<4;i++){
				if(i==h) continue;// 同一
				for(int j=0;j<4;j++){
					if(j==i || j==h) continue;// 同一
					if(permute(h,i,j)!=1) continue;
					for(int k=0;k<4;k++) p120[idx][k] = val[3] *(double)((g%2==0)?1:-1);

					p120[idx][h] = val[0];
					p120[idx][i] = val[1] *(double)((g%8<4)?1:-1);
					p120[idx][j] = val[2] *(double)((g%4<2)?1:-1);

					++idx;
				}
			}
		}
	}
    cout << idx <<endl;

	///(±ϕ−1, ±1, ±ϕ, ±2)
	//(0, ±ϕ−2, ±1, ±ϕ2)のパターンと同じ
	val[0] = pow(FAY, -1.0);
	val[1] = 1;
	val[2] = FAY;
	val[3] = 2;

	for(int g=0;g<16; g++){//-- パターン
		for(int h=0;h<4;h++){
			for(int i=0;i<4;i++){
				if(i==h) continue;// 同一
				for(int j=0;j<4;j++){
					if(j==i || j==h) continue;// 同一
					if(permute(h,i,j)!=1) continue;
					for(int k=0;k<4;k++) p120[idx][k] = val[3] *(double)((g%2==0)?1:-1);

					p120[idx][h] = val[0] *(double)((g%16<8)?1:-1);
					p120[idx][i] = val[1] *(double)((g% 8<4)?1:-1);
					p120[idx][j] = val[2] *(double)((g% 4<2)?1:-1);

					++idx;
				}
			}
		}
    //cout << idx <<endl;
	}
	
    cout << idx <<endl;
	char a;
	cin >> a;


	//for(int g=0;g<600; g++)
	//{
	//  cout	<< "No." << g << ": "
	//		<< p120[g][0] << ", "
	//		<< p120[g][1] << ", "
	//		<< p120[g][2] << ", "
	//		<< p120[g][3]
	//		<< endl;
	//}
	string result = "# on4D object\n";
	string fStr = "";
	/////---- 頂点データ作成
	//for(int i=0;i<600;i++){
	//	pt4 ptsE = {p120[i][0], p120[i][1], p120[i][2], p120[i][3]};
	//	pt3 ptsT = eucToTude( ptsE );

	//	result	+= "v "
	//			+ to_string((LDBL)ptsT.x) + " "
	//			+ to_string((LDBL)ptsT.y) + " "
	//			+ to_string((LDBL)ptsT.z) + " "
	//			+ "\n";
	//}

/*
	///---- 辺データ作成
	int cnt = 0;
	for(int h=0;h<600;h++){
		for(int i=h+1;i<600;i++){
			pt4 pts1 = {p120[h][0], p120[h][1], p120[h][2], p120[h][3]};
			pt4 pts2 = {p120[i][0], p120[i][1], p120[i][2], p120[i][3]};
			double dst = pyth4(pts1.mns(pts2));
			double edge = (3.0 - sqrt(5.0));
			if(edge*0.97 < dst && dst < edge*1.03)
			{
				result	+= "l "
						+ to_string((LLONG)(1+h)) + " " + to_string((LLONG)(1+i))
						+ "\n";
				++cnt;
			}
		}
	}*/
	double radius = sqrt(8.0);
	double edge = (3.0 - sqrt(5.0));
	double eDeg = asin( (0.5*edge / radius) )*2;
	//int p120b[120*N];
	///---- 頂点,面データ作成
	int cnt = 0;
	int vCnt = 0;
	for(int h=0;h<600;h++){
		for(int i=h+1;i<600;i++){
			pt4 pts1 = {p120[h][0], p120[h][1], p120[h][2], p120[h][3]};
			pt4 pts2 = {p120[i][0], p120[i][1], p120[i][2], p120[i][3]};

			double dst = pyth4(pts1.mns(pts2));
			if(edge*0.97 < dst && dst < edge*1.03)
			{
				//-- all_tudeRst
				pt3 loc1 = eucToTude(pts1);
				all_tudeRst(&pts2, loc1, 0);
				//-- 進行方向1,2の特定
				double rotOn[2] = {};
				rotOn[0] = atan2(pts2.x, pts2.y);
				rotOn[1] = atan2(pyth2(pts2.x, pts2.y), pts2.z);
				//-- N*2
				pt4 tmp = {1,0,0,0};
				tudeRst(&tmp.y, &tmp.w, 0.015, 1);
				pt4 bgn[N*2];
				for(int j=0;j<N;j++){
					pt4 tmp0 = tmp;
					tudeRst(&tmp0.x, &tmp0.y, (2*PIE/N *j), 1);
					bgn[j+0] = tmp0;
					tudeRst(&tmp0.z, &tmp0.w, eDeg, 1);
					bgn[j+N] = tmp0;
				}
				//-- 戻す
				for(int j=0;j<N*2;j++){
					tudeRst(&bgn[j].y, &bgn[j].z, rotOn[1], 1);
					tudeRst(&bgn[j].x, &bgn[j].y, rotOn[0], 1);
					all_tudeRst(&bgn[j], loc1, 1);
				}

				///-- 頂点
				for(int j=0;j<N*2;j++){
					pt3 ptsT = eucToTude( bgn[j] );
					result	+= "v "
							+ to_string((LDBL)ptsT.x) + " "
							+ to_string((LDBL)ptsT.y) + " "
							+ to_string((LDBL)ptsT.z) + " "
							+ "\n";
				}
				///-- 面
				for(int j=0;j<N;j++){
					int J = (j+1)%N;
					fStr	+= "f "
							+ to_string((LLONG)(1+vCnt+j)) + "//1 " 
							+ to_string((LLONG)(1+vCnt+N+j))  + "//1 " 
							+ to_string((LLONG)(1+vCnt+N+J)) + "//1 " 
							+ to_string((LLONG)(1+vCnt+J)) + "//1" 
							+ "\n";
				}
				vCnt += N*2;
				++cnt;
			}
		}
	}
	result += fStr;
	
	//----- objファイル生成

	FILE* c = fopen("p120c.obj", "w");
	fwrite(result.c_str(), 1, result.length(), c);

	fclose(c);
	
    cout << "lines: " << cnt <<endl;
	cin >> a;

	return 1;
}
