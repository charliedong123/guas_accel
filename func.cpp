#include<math.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "gaus_accel.hpp"
using namespace std;

//void gusReadOriImage(float Value[16], float *d_origImage, int YNum, int i, int j );
//void calculateGaus(float temImg[16][25],float temImg_1[16][25],float resultImg[16][25], float gauslut[325], float Value[16], int Offset_X, int Offset_Y );
//void gusWriteImage(float temImg_1[16][25], float *D_Image_R, int YNum,  int Xn, int Yn);
//
//void gusReadImage(float resultImg[16][25], float *D_Image_R, int YNum, int Xn, int Yn)
//{
//#pragma HLS  interface m_axi port=D_Image_R
//	int Resize_factor = 5;
//	gusRowLoop_1:for(int Rsj=0;Rsj<Resize_factor;Rsj++)
//	{
//		gusColoumLoop_1:for(int Rsi=0;Rsi<Resize_factor;Rsi++)
//		{
//
//			gusZLoop_1:for(int z=0;z<16;z++)
//			{
//				resultImg[z][Rsj * Resize_factor + Rsi] = D_Image_R[ (Xn + Rsj -2 ) * YNum * 16 + 16 * (Yn + Rsi - 2) + z];
//			}
//		}
//	}
//
//}
//
//void gusReadOriImage(float Value[16], float *d_origImage, int YNum, int i, int j )
//{
//	int Subpixels = 65;
//	int Resize_factor = 5;
//	gusGetValue:for(int z=0;z<16;z++)
//	{
//		Value[z] = d_origImage[i * YNum * 16 + z + j * 16];
//	}
//
//}
//
//void calculateGaus(float temImg[16][25],float temImg_1[16][25],float resultImg[16][25], float gauslut[325], float Value[16], int Offset_X, int Offset_Y )
//{
//	int Subpixels = 65;
//	int Resize_factor = 5;
//	gusRowLoop_2:for(int Rsj=0;Rsj<Resize_factor;Rsj++)
//	{
//		gusColoumLoop_2:for(int Rsi=0;Rsi<Resize_factor;Rsi++)
//		{
//
//			gusZLoop_2:for(int z=0;z<16;z++)
//			{
//#pragma HLS unroll
//				temImg[z][ Rsi * Resize_factor + Rsj ] = gauslut[  Rsi * Subpixels + Offset_X ] * Value[z];
//				temImg_1[z][ Rsj * Resize_factor + Rsi ] = resultImg[z][Rsj * Resize_factor + Rsi] + gauslut[  Rsi * Subpixels + Offset_Y ] *  temImg[z][ Rsj * Resize_factor + Rsi ];
//			}
//		}
//	}
//
//}
//
//void gusWriteImage(float temImg_1[16][25], float *D_Image_R, int YNum,  int Xn, int Yn)
//{
//	int Resize_factor = 5;
//	gusRowLoop_3:for(int Rsj=0;Rsj<Resize_factor;Rsj++)
//	{
//		gusColoumLoop_3:for(int Rsi=0;Rsi<Resize_factor;Rsi++)
//		{
//
//			gusZLoop_3:for(int z=0;z<16;z++)
//			{
//				D_Image_R[ (Xn + Rsj -2 ) * YNum * 16 + 16 * (Yn + Rsi - 2) + z] = temImg_1[z][Rsj * Resize_factor + Rsi];
//			}
//		}
//	}
//}


//void Gaussian_ImageRotation(float* d_origImage, float fi, int XNum, int YNum, int ZNum, int iZOffset, float* d_gausLut, float* D_Image_R)
void Gaussian_ImageRotation(float* d_origImage, float fi, float* d_gausLut, float* D_Image_R)

{
 #pragma HLS  interface m_axi port=d_origImage
#pragma HLS  interface m_axi port=d_gausLut
#pragma HLS  interface m_axi port=D_Image_R

	float gauslut[325];
#pragma HLS array_partition variable=gauslut complete dim=0
	float x,y,xn,yn;
	int Xn, Yn, Offset_X, Offset_Y, X_half, Y_half;

	float Value[672];
#pragma HLS array_partition variable=Value complete dim=0
	float COS,SIN;

	int Resize_factor = 5;
	float resultImg[672];
#pragma HLS array_partition variable=resultImg complete dim=1
	float temImg[672];
#pragma HLS array_partition variable=temImg complete dim=1
	float temImg_1[672];
#pragma HLS array_partition variable=temImg_1 complete dim=1
	float temvalue[16][25];
#pragma HLS array_partition variable=temvalue complete dim=1
	float temvalue_1[16][25];
#pragma HLS array_partition variable=temvalue_1 complete dim=1
	int Rsi, Rsj;
	int Subpixels = 65;

	COS = cosf(fi);
	SIN = sinf(fi);
	X_half = (XNum-1)*0.5;
	Y_half = (YNum-1)*0.5;
    for( int j = 0; j < 325; j++){
    	gauslut[j] = *(d_gausLut +j);
    }


	for(int i = 0; i < XNum; i++)
	{
#pragma HLS LOOP_TRIPCOUNT min= 24 max= 256
		for(int j = 0; j < YNum; j++)
		{
#pragma HLS LOOP_TRIPCOUNT min= 24 max= 256
//				Value = d_origImage[i * YNum * 16 + z + j * 16];//取原始图像一个�??
			x = i - X_half;
			y = j - Y_half;
			xn = x * COS - SIN * y;
			yn = SIN * x + COS * y;
			Xn = floor(xn+0.5);
			Offset_X = floor((xn - Xn)*Subpixels + 0.5) + (Subpixels-1)*0.5;//计算x方向Guass查找表的坐标
			Yn = floor(yn+0.5);
			Offset_Y = floor((yn - Yn)*Subpixels + 0.5) + (Subpixels-1)*0.5;//计算y方向Guass查找表的坐标

			Xn = Xn + X_half;
			Yn = Yn + Y_half;

//			regionDataflow: {
//#pragma HLS pipeline
//				gusReadImage(resultImg, D_Image_R, YNum, Xn, Yn);
//				gusReadOriImage(Value, d_origImage, YNum, i, j);
//				calculateGaus(temImg,temImg_1,resultImg, gauslut, Value,Offset_X,Offset_Y);
//				gusWriteImage(temImg_1,D_Image_R,YNum, Xn,Yn);
//			}


// read the original picture pixel value based z axis from 0 -- 15
			gusGetValue:for(int z=0;z<ZNum;z++)
			{
#pragma HLS LOOP_TRIPCOUNT min= 24 max= 672
				Value[z] = d_origImage[i * YNum * ZNum + z + j * ZNum];
			}
			regionDataflow: {
			//read the result picture image 5*5 value
				gusRowLoop_1:for(Rsj=0;Rsj<Resize_factor;Rsj++)
				{
					gusColoumLoop_1:for(Rsi=0;Rsi<Resize_factor;Rsi++)
					{
#pragma HLS pipeline
						gusZLoop_1:for(int z=0;z<672;z++)
						{

#pragma HLS LOOP_TRIPCOUNT min= 24 max= 672
							resultImg[z] = D_Image_R[ (Xn + Rsj -2 ) * YNum * ZNum + ZNum * (Yn + Rsi - 2) + z];
//							temImg[z] = gauslut[  Rsi * Subpixels + Offset_X ] * Value[z];
//							temImg_1[z] = resultImg[z] + gauslut[  Rsi * Subpixels + Offset_Y ] *  temImg[z];
//							D_Image_R[ (Xn + Rsj -2 ) * YNum * 16 + 16 * (Yn + Rsi - 2) + z] = temImg_1[z];
						}
//						gusZLoop_2:for(int z=0;z<672;z++)
//						{
//#pragma HLS LOOP_TRIPCOUNT min= 24 max= 672
//							temImg[z] = gauslut[  Rsi * Subpixels + Offset_X ] * Value[z];
//							temImg_1[z] = resultImg[z] + gauslut[  Rsi * Subpixels + Offset_Y ] *  temImg[z];
//						}
						gusZLoop_3:for(int z=0;z<672;z++)
						{
#pragma HLS LOOP_TRIPCOUNT min= 24 max= 672
							D_Image_R[ (Xn + Rsj -2 ) * YNum * ZNum + ZNum * (Yn + Rsi - 2) + z] = resultImg[z] + 1;
						}


					}
				}
			}
		}
	}

}

