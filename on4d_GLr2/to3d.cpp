#include <Windows.h>
#include <Complex>
#include <algorithm>
#include "constants.h"
#include "engine3dWin.h"
#include "constants.h"
#include "functions.h"
using namespace std;



int engine3dWin::to3d()
{
  // 現状固定値
  bool dim4reset[2] = {};
  double dim4dom[4] = {};
  double dim4domR[6] = {};

  ///---カメラ---///
  empObj4 camera1;
  camera1.loc.asg(0,0,0,0);
  camera1.rot.asg(0,0,0,0,0,0);
  //=====カメラの範囲
  double cRangeX = tan((LDBL)80/2 *PIE/180);	
  double cRangeY = tan((LDBL)80/2 *PIE/180);
  double cRangeZ = tan((LDBL)80/2 *PIE/180);
  double scale = 3.0;
  

  UINT drawLen = 0;
  for(UINT h=0;h<polCnt;h++) drawLen += pols[h].pLen;
  pt4* drawPt = new pt4[drawLen];
  pt4* drawPt4D = new pt4[drawLen];
  drawLen = 0;
  double dst4[2] = {-1, -1};

	
  for(UINT h=0;h<polCnt;h++)	//==============ポリごとの処理==============//
  {

	///位置・速度の変更 (暫時用)
	if(1){
		pols[h].lsp.asg(dim4dom[0], dim4dom[1], dim4dom[2], dim4dom[3]);
		pols[h].rsp.asg(dim4domR[0], dim4domR[1], dim4domR[2], dim4domR[3], dim4domR[4], dim4domR[5]);
	}
	if(dim4reset[0]) pols[h].loc.asg(2,0,0,0),
					dim4dom[0] = dim4dom[1] = dim4dom[2] = dim4dom[3] = 0;
	if(dim4reset[1]) pols[h].rot.asg(0,0,0,0,0,0),
					dim4domR[0] = dim4domR[1] = dim4domR[2] = dim4domR[3] = dim4domR[4] = dim4domR[5] = 0;
	if(h==polCnt-1) dim4reset[0] = dim4reset[1] = false;
	


	if(obMove){///===位置・回転の変更適用
		pols[h].loc = pols[h].loc.pls(pols[h].lsp);
		pols[h].rot = pols[h].rot.pls(pols[h].rsp);
	}
	
	for(int i=0;i<pols[h].pLen;i++)
	{
		UINT idw = i+drawLen;
		double deg, minus, hanke;
		pt4 tmpPt;

		drawPt[idw].asg2(pols[h].pts[i]);
		if(1){
			tudeRst(&drawPt[idw].y, &drawPt[idw].z, 0.5*PIE, 1); //-- Y軸を奥行きに (互換のため)
			drawPt[idw].z *= -1;
		}
		///▼自身の回転
		degAdj(drawPt+idw, &pols[h].ctr, 1);//とりあえず3
		hanke = pyth2(drawPt[idw].x-pols[h].ctr.x, drawPt[idw].y-pols[h].ctr.y); /// xy
		deg = atan((drawPt[idw].y-pols[h].ctr.y) / (minus=drawPt[idw].x-pols[h].ctr.x)); if(minus<0) deg += PIE;
		tmpPt.x = cos((deg + pols[h].rot.xy - camera1.rot.xy))*hanke;
		tmpPt.y = sin((deg + pols[h].rot.xy - camera1.rot.xy))*hanke;
		degAdj(&tmpPt, 2);//とりあえず3
		hanke = pyth2(tmpPt.y, drawPt[idw].z - pols[h].ctr.z); /// yz
		deg = atan((drawPt[idw].z - pols[h].ctr.z) / tmpPt.y); if(tmpPt.y<0) deg += PIE;
		tmpPt.y = cos((deg + pols[h].rot.yz - camera1.rot.yz))*hanke;
		tmpPt.z = sin((deg + pols[h].rot.yz - camera1.rot.yz))*hanke;
		degAdj(&tmpPt, 1);//とりあえず3
		hanke = pyth2(tmpPt.x, tmpPt.z); /// xz
		deg = atan(tmpPt.z / tmpPt.x); if(tmpPt.x<0) deg += PIE;
		tmpPt.x = cos((deg + pols[h].rot.xz - camera1.rot.xz))*hanke;
		tmpPt.z = sin((deg + pols[h].rot.xz - camera1.rot.xz))*hanke;
		
		degAdj(&tmpPt, 1);//とりあえず3
		hanke = pyth2((drawPt[idw].w - pols[h].ctr.w), tmpPt.x); /// wx
		deg = atan((drawPt[idw].w - pols[h].ctr.w) / tmpPt.x); if(tmpPt.x<0) deg += PIE;
		tmpPt.x = cos((deg + pols[h].rot.wx - camera1.rot.wx))*hanke;
		tmpPt.w = sin((deg + pols[h].rot.wx - camera1.rot.wx))*hanke;
		degAdj(&tmpPt, 2);//とりあえず3
		hanke = pyth2(tmpPt.w, tmpPt.y); /// wy
		deg = atan(tmpPt.w / tmpPt.y); if(tmpPt.y<0) deg += PIE;
		tmpPt.y = cos((deg + pols[h].rot.wy - camera1.rot.wy))*hanke;
		tmpPt.w = sin((deg + pols[h].rot.wy - camera1.rot.wy))*hanke;
		degAdj(&tmpPt, 3);//とりあえず3
		hanke = pyth2(tmpPt.w, tmpPt.z); /// wz
		deg = atan(tmpPt.w / tmpPt.z); if(tmpPt.z<0) deg += PIE;
		tmpPt.z = cos((deg + pols[h].rot.wz - camera1.rot.wz))*hanke;
		tmpPt.w = sin((deg + pols[h].rot.wz - camera1.rot.wz))*hanke;


		drawPt[idw].x = tmpPt.x + pols[h].loc.x;
		drawPt[idw].y = tmpPt.y + pols[h].loc.y;
		drawPt[idw].z = tmpPt.z + pols[h].loc.z;
		drawPt[idw].w = tmpPt.w + pols[h].loc.w;
	}
	
	for(int i=0; i<pols[h].linLen; i+=2){ ///---計算のために値yを調整---///
		char tp1 = pols[h].lins[i]+drawLen, tp2 = pols[h].lins[i+1]+drawLen;
		adjW(drawPt+tp2, drawPt+tp1);	///とりあえず12
	}
	for(int i=0;i<pols[h].pLen;i++){
	  if(MIN_DST <= drawPt[i].w){
		UINT idw = i+drawLen;
		//-- 最短・最長距離
		double dst = drawPt[idw].w;
		if(dst < dst4[0] || dst4[0]<0) dst4[0] = dst;
		if(dst > dst4[1] || dst4[1]<0) dst4[1] = dst;

		drawPt4D[idw] = drawPt[idw];
		//-- 座標変換(投影)
		drawPt[idw].x = (drawPt[idw].x/drawPt[idw].w)/(cRangeX*2)*(scale);
		drawPt[idw].y = (drawPt[idw].y/drawPt[idw].w)/(cRangeX*2)*(scale);
		drawPt[idw].z = (drawPt[idw].z/drawPt[idw].w)/(cRangeY*2)*(scale);

	  }else{
		  delete[] drawPt;
		  delete[] drawPt4D;
		  return 0;
	  }
	}
	//for(int i=0; i<pols[h].linLen; i++){ ///---計算のために値yを調整again---///
	//  char tp1 = i, tp2 = (i+1)%pols[h].linLen;
	//  adjY4d(drawPt+drawLen +tp2, drawPt+drawLen +tp1);	///とりあえず12
	//}

	drawLen += pols[h].pLen;
  }///-----ポリごと終了------

  
  //--
  tMesh.pLen = 0;
  for(UINT h=0;h<polCnt;h++) tMesh.pLen += pols[h].pLen;
  tMesh.faceLen = 0;
  for(UINT h=0;h<polCnt;h++)
	for(UINT i=0;i<pols[h].objLen;i++) tMesh.faceLen += pols[h].objs[i].faceLen;
  tMesh.faces = new face3[tMesh.faceLen];
  tMesh.faceLen = 0;

  tMesh.pts = new pt3[drawLen];
  pConv4to3(drawPt, tMesh.pts, drawLen);
  


  UINT pppls = 0;//polLenPlus
  for(UINT h=0;h<polCnt;h++){
	for(UINT i=0;i<pols[h].objLen;i++){
	  for(UINT j=0;j<pols[h].objs[i].faceLen;j++)
	  {
		tMesh.faces[tMesh.faceLen].pts = new pt3*[3];
		fConv4to3(pols[h].objs[i].faces+j, tMesh.faces+tMesh.faceLen, tMesh.pts +pppls);
		++tMesh.faceLen;
	  }
	}
	pppls += pols[h].pLen;
  }
  delete[] drawPt;
  delete[] drawPt4D;
  return tMesh.faceLen;
}





///- - - - - - - - - - - - - - - - - - - - -

int engine3dWin::getOn4loc(short* dataS, double* dataD, UINT* bmp){

	double crX = dataD[0], crY = dataD[1];
	short width = dataS[0], height = dataS[1], w_rest = dataS[2];


  //=====カメラの範囲
  double cRangeX = tan((LDBL)80/2 *PIE/180);	
  double cRangeY = tan((LDBL)80/2 *PIE/180);
  double cRangeZ = tan((LDBL)80/2 *PIE/180);
  double scale = 3.0;
  
	
  for(UINT h=BWH_QTY;h<objCnt;h++)//==============on4オブジェごとの処理==============//
  {
	if(!objs[h].used) continue;
	else if(h==PLR_No) continue;
	else if(h<BWH_QTY+PLR_QTY && !VIEW_PLR) continue;
	pt4 tmpPt = object3d::tudeToEuc(objs[h].locr);

	tudeRst(&tmpPt.y, &tmpPt.z, 0.5*PIE, 1); //-- Y軸を奥行きに (互換のため)
	tmpPt.z *= -1;
	///▼自身の回転
	//tudeRst(&tmpPt.y, &tmpPt.x, pols[0].rot.xy, 1);
	//tudeRst(&tmpPt.z, &tmpPt.y, pols[0].rot.yz, 1);
	//tudeRst(&tmpPt.z, &tmpPt.x, pols[0].rot.xz, 1);

	//tudeRst(&tmpPt.w, &tmpPt.x, pols[0].rot.wx, 1);
	//tudeRst(&tmpPt.w, &tmpPt.y, pols[0].rot.wy, 1);
	//tudeRst(&tmpPt.w, &tmpPt.z, pols[0].rot.wz, 1);

	pt4 objLoc = tmpPt.pls(pols[0].loc);


	
	if(MIN_DST <= objLoc.w){
		//-- 座標変換(投影)
		objLoc.x = (objLoc.x/objLoc.w) / (cRangeX*2)*(scale);
		objLoc.y = (objLoc.y/objLoc.w) / (cRangeX*2)*(scale);
		objLoc.z = (objLoc.z/objLoc.w) / (cRangeY*2)*(scale);
	}else{
		return 0;
	}
	


	
	///---- 描画処理 ----///
	pt3 loc1 = objLoc.xyz().pls(tObj.loc);

	if(0<loc1.y){
		if(-1*crX < loc1.x/loc1.y && loc1.x/loc1.y < crX)
		if(-1*crY < loc1.z/loc1.y && loc1.z/loc1.y < crY)
		{
			short curPt[2];
			curPt[0] = (short)((loc1.x/loc1.y + crX)/(crX*2)*width);
			curPt[1] = (short)((loc1.z/loc1.y + crY)/(crY*2)*height);


			BYTE bof = (BYTE)(objs[h].locr.y / PIE * 255);
			UINT col = (objs[h].locr.y <= 0.5*PIE) ? 0xffff00 : 0xff0000;
			//UINT col = (objs[h].locr.z <= 0.5*PIE)
			//			? (0*256*256 + 255*256 + 0)
			//			: (255*256*256 + 0*256 + 0);
			int idx =  curPt[0]+curPt[1]*(width+w_rest);
			if(objs[h].locr.z <= 0.5*PIE){
				bmp[idx] = 
				bmp[idx+1] = 
				bmp[idx+2] = 
				bmp[idx-1] = 
				bmp[idx-2] = 
				bmp[idx-1*(width+w_rest)] = 
				bmp[idx+1*(width+w_rest)] = 
				bmp[idx-2*(width+w_rest)] = 
				bmp[idx+2*(width+w_rest)] =
				bmp[idx-1*(width+w_rest)+1] = 
				bmp[idx+1*(width+w_rest)+1] = 
				bmp[idx-1*(width+w_rest)-1] = 
				bmp[idx+1*(width+w_rest)-1] = col;
			}else{
				bmp[idx+1] = 
				bmp[idx-1] = 
				bmp[idx-1*(width+w_rest)] = 
				bmp[idx+1*(width+w_rest)] = 
				bmp[idx-1*(width+w_rest)+1] = 
				bmp[idx+1*(width+w_rest)+1] = 
				bmp[idx-1*(width+w_rest)-1] = 
				bmp[idx+1*(width+w_rest)-1] =
				bmp[idx-2*(width+w_rest)+2] = 
				bmp[idx+2*(width+w_rest)+2] = 
				bmp[idx-2*(width+w_rest)-2] = 
				bmp[idx+2*(width+w_rest)-2] =
				bmp[idx-3*(width+w_rest)+3] = 
				bmp[idx+3*(width+w_rest)+3] = 
				bmp[idx-3*(width+w_rest)-3] = 
				bmp[idx+3*(width+w_rest)-3] = col;
			}
		}
	}
  }


  
  return tMesh.faceLen;
}
