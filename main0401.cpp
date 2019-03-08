#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "bmpFile.h"
#include "RmwEdgeDetect.h"
#include "..//ZzcCPP/ZzcFilter.h"
#include "..//ZzcCPP/ZzcColor.h"
const double pi = 3.1415926;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                            视觉显著度IT算法
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//             归一化操作
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ZzcNormalization(BYTE *pImg, int width, int height)
{
	long long sum = 0; double aver;
	for (int i = 0; i < 6; i++)
	{
		sum = 0;
		BYTE  *pCur = pImg + i*width*height,*pEnd = pCur + width*height;
		while (pCur < pEnd)
		{
			sum += *(pCur++);
		}
		aver = (double)sum / ((long long)width*height * 255);
		pCur = pImg + i*width*height;
		while (pCur < pEnd)
		{
			*(pCur++) = min(*(pCur)*(0.15 / aver),255);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//           求和
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ZzcITsum(BYTE *pIntensity_CenterSurround, BYTE *pRGCompete, BYTE *pBYCompete, BYTE *pGabor_0, BYTE *pGabor_45, BYTE *pGabor_90, BYTE *pGabor_135, int width,int height, BYTE *pRes)
{
	BYTE *pEnd = pRes + width*height;int sum = 0,i,imgsize=width*height;
	while (pRes < pEnd)
	{
		sum = 0;
		for (i = 0; i < 6; i++)
		{
			
			sum += (*(pIntensity_CenterSurround + i*imgsize) + *(pRGCompete + i*imgsize) + *(pBYCompete + i*imgsize) +
				*(pGabor_0 + i*imgsize) + *(pGabor_45 + i*imgsize) + *(pGabor_90 + i*imgsize) + *(pGabor_90 + i*imgsize) + *(pGabor_135 + i*imgsize));
		}
		*(pRes++) = sum / 42 ;
		pIntensity_CenterSurround++;
		pRGCompete++;
		pBYCompete++;
		pGabor_0++;
		pGabor_45++;
		pGabor_90++;
		pGabor_135++;
	}
}
void IT(char *name)
{
	int width, height;
	clock_t s, e;
	BYTE *pRGBImg = RmwRead24BitBmpFile2Img(name, &width, &height);
	BYTE *pRGBImg_Gauss = new BYTE[width*height*3 * 8];
	long long *Intergal = new long long[width*height * 3];
	s = clock();
	CalIntergal_3(pRGBImg, width, height, Intergal);
	for (int i = 1; i < 8; i++)
	{
		ZzcGaussFilter_3(pRGBImg,width,height,(int)pow(2,i),pRGBImg_Gauss+i*width*height*3,Intergal);
	}
	e = clock();
	printf("%ld\n", e - s);
	/*RmwWrite24BitImg2BmpFile(pRGBImg_Gauss, width, height, "..//BmpResFile/Dune_Gauss_1.bmp");
	RmwWrite24BitImg2BmpFile(pRGBImg_Gauss + width*height * 3, width, height, "..//BmpResFile/Dune_Gauss_2.bmp");
	RmwWrite24BitImg2BmpFile(pRGBImg_Gauss + 2*width*height * 3, width, height, "..//BmpResFile/Dune_Gauss_3.bmp");
	RmwWrite24BitImg2BmpFile(pRGBImg_Gauss + 3*width*height * 3, width, height, "..//BmpResFile/Dune_Gauss_4.bmp");
	RmwWrite24BitImg2BmpFile(pRGBImg_Gauss + 4*width*height * 3, width, height, "..//BmpResFile/Dune_Gauss_5.bmp");
	RmwWrite24BitImg2BmpFile(pRGBImg_Gauss + 5*width*height * 3, width, height, "..//BmpResFile/Dune_Gauss_6.bmp");
	RmwWrite24BitImg2BmpFile(pRGBImg_Gauss + 6*width*height * 3, width, height, "..//BmpResFile/Dune_Gauss_7.bmp");
	RmwWrite24BitImg2BmpFile(pRGBImg_Gauss + 7*width*height * 3, width, height, "..//BmpResFile/Dune_Gauss_8.bmp");*/
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	//      亮度提取
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	BYTE *pIntensityImg = new BYTE[width*height * 8];
	s = clock();
	for (int i = 1; i < 8; i++)
	{
		ZzcIntensityExtract(pRGBImg_Gauss + i*width*height * 3, width, height, pIntensityImg+i*width*height);
	}
	e = clock();
	printf("IntensityImg : %ld ms .\n", e - s);
	/*RmwWriteByteImg2BmpFile(pIntensityImg, width, height, "..//BmpResFile/Dune_Intensity_1.bmp");
	RmwWriteByteImg2BmpFile(pIntensityImg + width*height , width, height, "..//BmpResFile/Dune_Intensity_2.bmp");
	RmwWriteByteImg2BmpFile(pIntensityImg + 2 * width*height , width, height, "..//BmpResFile/Dune_Intensity_3.bmp");
	RmwWriteByteImg2BmpFile(pIntensityImg + 3 * width*height , width, height, "..//BmpResFile/Dune_Intensity_4.bmp");
	RmwWriteByteImg2BmpFile(pIntensityImg + 4 * width*height , width, height, "..//BmpResFile/Dune_Intensity_5.bmp");
	RmwWriteByteImg2BmpFile(pIntensityImg + 5 * width*height , width, height, "..//BmpResFile/Dune_Intensity_6.bmp");
	RmwWriteByteImg2BmpFile(pIntensityImg + 6 * width*height , width, height, "..//BmpResFile/Dune_Intensity_7.bmp");
	RmwWriteByteImg2BmpFile(pIntensityImg + 7 * width*height , width, height, "..//BmpResFile/Dune_Intensity_8.bmp");*/
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//     颜色提取
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	BYTE *pColorImg = new BYTE[width*height*4 * 8];
	BYTE *pRImg = new BYTE[width*height * 8],*pGImg=new BYTE[width*height*8],*pBImg=new BYTE[width*height*8],*pYImg=new BYTE[width*height*8];
	s = clock();
	for (int i = 1; i < 8; i++)
	{
		Zzcrgb2RGBY(pRGBImg_Gauss + i*width*height * 3, width, height, pRImg + i*width*height, pGImg + i*width*height, pBImg + i*width*height, pYImg + i*width*height);
		//Zzcrgb2RGBY(pRGBImg_Gauss + i*width*height * 3, width, height, pColorImg + i*width*height * 4);
		//ZzcSeparateRGBY(pColorImg+i*width*height*4,width,height,pRImg+i*width*height,pGImg+i*width*height,pBImg+i*width*height,pYImg+i*width*height);
	}
	e = clock();
	printf("rgb2RGBY : %ld ms. \n", e - s);
	/*RmwWriteByteImg2BmpFile(pRImg , width, height, "..//BmpResFile/Dune_ColorR_1.bmp");
	RmwWriteByteImg2BmpFile(pGImg , width, height, "..//BmpResFile/Dune_ColorG_1.bmp");
	RmwWriteByteImg2BmpFile(pBImg , width, height, "..//BmpResFile/Dune_ColorB_1.bmp");
	RmwWriteByteImg2BmpFile(pYImg + width*height, width, height, "..//BmpResFile/Dune_ColorY_1.bmp");
	RmwWriteByteImg2BmpFile(pRImg + width*height , width, height, "..//BmpResFile/Dune_ColorR_2.bmp");
	RmwWriteByteImg2BmpFile(pGImg + width*height , width, height, "..//BmpResFile/Dune_ColorG_2.bmp");
	RmwWriteByteImg2BmpFile(pBImg + width*height , width, height, "..//BmpResFile/Dune_ColorB_2.bmp");
	RmwWriteByteImg2BmpFile(pYImg + width*height , width, height, "..//BmpResFile/Dune_ColorY_2.bmp");
	RmwWriteByteImg2BmpFile(pRImg + width*height * 2, width, height, "..//BmpResFile/Dune_ColorR_3.bmp");
	RmwWriteByteImg2BmpFile(pGImg + width*height * 2, width, height, "..//BmpResFile/Dune_ColorG_3.bmp");
	RmwWriteByteImg2BmpFile(pBImg + width*height * 2, width, height, "..//BmpResFile/Dune_ColorB_3.bmp");
	RmwWriteByteImg2BmpFile(pYImg + width*height * 2, width, height, "..//BmpResFile/Dune_ColorY_3.bmp");*/
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//        0
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int size = 2;
	BYTE *pGaborImg_0 = new BYTE[width*height * 8];
	double *real = new double[(size * 2 + 1)*(size * 2 + 1)], *img = new double[(size * 2 + 1)*(size * 2 + 1)];
	s = clock();
	ZzcGetGabor(2 * size + 1, size * 2 + 1, 0, 100, 1, 5, 0, real, img);
	for (int i = 1; i < 8; i++)
	{
		ZzcGaborFilter2(pIntensityImg + i*width*height, width, height, real, img, size, pGaborImg_0 + i*width*height);
	}
	e = clock();
	printf("Gabor Filter 0 : %ld ms. \n", e - s);
	/*RmwWriteByteImg2BmpFile(pGaborImg_0, width, height, "..//BmpResFile/Dune_Gabor_0_1.bmp");
	RmwWriteByteImg2BmpFile(pGaborImg_0 + width*height, width, height, "..//BmpResFile/Dune_Gabor_0_2.bmp");
	RmwWriteByteImg2BmpFile(pGaborImg_0 + 2 * width*height, width, height, "..//BmpResFile/Dune_Gabor_0_3.bmp");
	RmwWriteByteImg2BmpFile(pGaborImg_0 + 3 * width*height, width, height, "..//BmpResFile/Dune_Gabor_0_4.bmp");
	RmwWriteByteImg2BmpFile(pGaborImg_0 + 4 * width*height, width, height, "..//BmpResFile/Dune_Gabor_0_5.bmp");
	RmwWriteByteImg2BmpFile(pGaborImg_0 + 5 * width*height, width, height, "..//BmpResFile/Dune_Gabor_0_6.bmp");
	RmwWriteByteImg2BmpFile(pGaborImg_0 + 6 * width*height, width, height, "..//BmpResFile/Dune_Gabor_0_7.bmp");
	RmwWriteByteImg2BmpFile(pGaborImg_0 + 7 * width*height, width, height, "..//BmpResFile/Dune_Gabor_0_8.bmp");*/
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//        45
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	BYTE *pGaborImg_45 = new BYTE[width*height * 8];
	s = clock();
	ZzcGetGabor(2 * size + 1, size * 2 + 1, pi / 4, 100, 1, 5, 0, real, img);
	for (int i = 1; i < 8; i++)
	{
		ZzcGaborFilter2(pIntensityImg + i*width*height, width, height, real, img, size, pGaborImg_45 + i*width*height);
	}
	e = clock();
	printf("Gabor Filter 45 : %ld ms. \n", e - s);
	/*RmwWriteByteImg2BmpFile(pGaborImg_45, width, height, "..//BmpResFile/Dune_Gabor_45_1.bmp");
	RmwWriteByteImg2BmpFile(pGaborImg_45 + width*height, width, height, "..//BmpResFile/Dune_Gabor_45_2.bmp");
	RmwWriteByteImg2BmpFile(pGaborImg_45 + 2 * width*height, width, height, "..//BmpResFile/Dune_Gabor_45_3.bmp");
	RmwWriteByteImg2BmpFile(pGaborImg_45 + 3 * width*height, width, height, "..//BmpResFile/Dune_Gabor_45_4.bmp");
	RmwWriteByteImg2BmpFile(pGaborImg_45 + 4 * width*height, width, height, "..//BmpResFile/Dune_Gabor_45_5.bmp");
	RmwWriteByteImg2BmpFile(pGaborImg_45 + 5 * width*height, width, height, "..//BmpResFile/Dune_Gabor_45_6.bmp");
	RmwWriteByteImg2BmpFile(pGaborImg_45 + 6 * width*height, width, height, "..//BmpResFile/Dune_Gabor_45_7.bmp");
	RmwWriteByteImg2BmpFile(pGaborImg_45 + 7 * width*height, width, height, "..//BmpResFile/Dune_Gabor_45_8.bmp");*/
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//        90
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	BYTE *pGaborImg_90 = new BYTE[width*height * 8];
	s = clock();
	ZzcGetGabor(2 * size + 1, size * 2 + 1, pi / 2, 100, 1, 5, 0, real, img);
	for (int i = 1; i < 8; i++)
	{
		ZzcGaborFilter2(pIntensityImg + i*width*height, width, height, real, img, size, pGaborImg_90 + i*width*height);
	}
	e = clock();
	printf("Gabor Filter 90: %ld ms. \n", e - s);
	/*	RmwWriteByteImg2BmpFile(pGaborImg_90, width, height, "..//BmpResFile/Dune_Gabor_90_1.bmp");
		RmwWriteByteImg2BmpFile(pGaborImg_90 + width*height, width, height, "..//BmpResFile/Dune_Gabor_90_2.bmp");
		RmwWriteByteImg2BmpFile(pGaborImg_90 + 2 * width*height, width, height, "..//BmpResFile/Dune_Gabor_90_3.bmp");
		RmwWriteByteImg2BmpFile(pGaborImg_90 + 3 * width*height, width, height, "..//BmpResFile/Dune_Gabor_90_4.bmp");
		RmwWriteByteImg2BmpFile(pGaborImg_90 + 4 * width*height, width, height, "..//BmpResFile/Dune_Gabor_90_5.bmp");
		RmwWriteByteImg2BmpFile(pGaborImg_90 + 5 * width*height, width, height, "..//BmpResFile/Dune_Gabor_90_6.bmp");
		RmwWriteByteImg2BmpFile(pGaborImg_90 + 6 * width*height, width, height, "..//BmpResFile/Dune_Gabor_90_7.bmp");
		RmwWriteByteImg2BmpFile(pGaborImg_90 + 7 * width*height, width, height, "..//BmpResFile/Dune_Gabor_90_8.bmp");*/
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//        135
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//int size = 2;
	BYTE *pGaborImg_135 = new BYTE[width*height * 8];
	//double *real = new double[(size * 2 + 1)*(size * 2 + 1)], *img = new double[(size * 2 + 1)*(size * 2 + 1)];
	s = clock();
	ZzcGetGabor(2 * size + 1, size * 2 + 1, 3 * pi / 4, 100, 1, 5, 0, real, img);
	for (int i = 1; i < 8; i++)
	{
		ZzcGaborFilter2(pIntensityImg + i*width*height, width, height, real, img, size, pGaborImg_135 + i*width*height);
	}
	e = clock();
	printf("Gabor Filter 135: %ld ms. \n", e - s);
	/*RmwWriteByteImg2BmpFile(pGaborImg_135, width, height, "..//BmpResFile/Dune_Gabor_135_1.bmp");
	RmwWriteByteImg2BmpFile(pGaborImg_135 + width*height, width, height, "..//BmpResFile/Dune_Gabor_135_2.bmp");
	RmwWriteByteImg2BmpFile(pGaborImg_135 + 2 * width*height, width, height, "..//BmpResFile/Dune_Gabor_135_3.bmp");
	RmwWriteByteImg2BmpFile(pGaborImg_135 + 3 * width*height, width, height, "..//BmpResFile/Dune_Gabor_135_4.bmp");
	RmwWriteByteImg2BmpFile(pGaborImg_135 + 4 * width*height, width, height, "..//BmpResFile/Dune_Gabor_135_5.bmp");
	RmwWriteByteImg2BmpFile(pGaborImg_135 + 5 * width*height, width, height, "..//BmpResFile/Dune_Gabor_135_6.bmp");
	RmwWriteByteImg2BmpFile(pGaborImg_135 + 6 * width*height, width, height, "..//BmpResFile/Dune_Gabor_135_7.bmp");
	RmwWriteByteImg2BmpFile(pGaborImg_135 + 7 * width*height, width, height, "..//BmpResFile/Dune_Gabor_135_8.bmp");*/
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//         亮度中央周边差
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	BYTE *pIntensity_CenterSurround = new BYTE[width*height * 6];
	s = clock();
	ZzcSubtract(pIntensityImg + width*height, pIntensityImg + 4 * width*height, width, height, pIntensity_CenterSurround);
	ZzcSubtract(pIntensityImg + width*height, pIntensityImg + 5 * width*height, width, height, pIntensity_CenterSurround + width*height);
	ZzcSubtract(pIntensityImg + 2 * width*height, pIntensityImg + 5 * width*height, width, height, pIntensity_CenterSurround + 2 * width*height);
	ZzcSubtract(pIntensityImg + 2 * width*height, pIntensityImg + 6 * width*height, width, height, pIntensity_CenterSurround + 3 * width*height);
	ZzcSubtract(pIntensityImg + 3 * width*height, pIntensityImg + 6 * width*height, width, height, pIntensity_CenterSurround + 4 * width*height);
	ZzcSubtract(pIntensityImg + 3 * width*height, pIntensityImg + 7 * width*height, width, height, pIntensity_CenterSurround + 5 * width*height);
	ZzcNormalization(pIntensity_CenterSurround, width, height);
	e = clock();
	printf("Intensity CenterSurround : %ld ms . \n", e - s);
	RmwWriteByteImg2BmpFile(pIntensity_CenterSurround, width, height, "..//BmpResFile/Dune_Intensity_Centersurround_1.bmp");
	RmwWriteByteImg2BmpFile(pIntensity_CenterSurround + width*height, width, height, "..//BmpResFile/Dune_Intensity_Centersurround_2.bmp");
	RmwWriteByteImg2BmpFile(pIntensity_CenterSurround + 2 * width*height, width, height, "..//BmpResFile/Dune_Intensity_Centersurround_3.bmp");
	RmwWriteByteImg2BmpFile(pIntensity_CenterSurround + 3 * width*height, width, height, "..//BmpResFile/Dune_Intensity_Centersurround_4.bmp");
	RmwWriteByteImg2BmpFile(pIntensity_CenterSurround + 4 * width*height, width, height, "..//BmpResFile/Dune_Intensity_Centersurround_5.bmp");
	RmwWriteByteImg2BmpFile(pIntensity_CenterSurround + 5 * width*height, width, height, "..//BmpResFile/Dune_Intensity_Centersurround_6.bmp");
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//              RG颜色竞争
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	BYTE *pRGCompete = new BYTE[width*height * 6];
	s = clock();
	ZzcColorCompete(pRImg + width*height, pGImg + width*height, pRImg + 4 * width*height, pGImg + 4 * width*height, width, height, pRGCompete);
	ZzcColorCompete(pRImg + width*height, pGImg + width*height, pRImg + 5 * width*height, pGImg + 5 * width*height, width, height, pRGCompete + width*height);
	ZzcColorCompete(pRImg + 2 * width*height, pGImg + 2 * width*height, pRImg + 5 * width*height, pGImg + 5 * width*height, width, height, pRGCompete + 2 * width*height);
	ZzcColorCompete(pRImg + 2 * width*height, pGImg + 2 * width*height, pRImg + 6 * width*height, pGImg + 6 * width*height, width, height, pRGCompete + 3 * width*height);
	ZzcColorCompete(pRImg + 3 * width*height, pGImg + 3 * width*height, pRImg + 6 * width*height, pGImg + 6 * width*height, width, height, pRGCompete + 4 * width*height);
	ZzcColorCompete(pRImg + 3 * width*height, pGImg + 3 * width*height, pRImg + 7 * width*height, pGImg + 7 * width*height, width, height, pRGCompete + 5 * width*height);
	ZzcNormalization(pRGCompete, width, height);
	e = clock();
	printf("pRGCompete : %ld ms.\n", e - s);
	RmwWriteByteImg2BmpFile(pRGCompete, width, height, "..//BmpResFile/Dune_Compete_RG_1.bmp");
	RmwWriteByteImg2BmpFile(pRGCompete + width*height, width, height, "..//BmpResFile/Dune_Compete_RG_2.bmp");
	RmwWriteByteImg2BmpFile(pRGCompete +2 * width*height, width, height, "..//BmpResFile/Dune_Compete_RG_3.bmp");
	RmwWriteByteImg2BmpFile(pRGCompete +3 * width*height, width, height, "..//BmpResFile/Dune_Compete_RG_4.bmp");
	RmwWriteByteImg2BmpFile(pRGCompete +4 * width*height, width, height, "..//BmpResFile/Dune_Compete_RG_5.bmp");
	RmwWriteByteImg2BmpFile(pRGCompete +5 * width*height, width, height, "..//BmpResFile/Dune_Compete_RG_6.bmp");
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//              BY颜色竞争
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	BYTE *pBYCompete = new BYTE[width*height * 6];
	s = clock();
	ZzcColorCompete(pBImg + width*height, pYImg + width*height, pBImg + 4 * width*height, pYImg + 4 * width*height, width, height, pBYCompete);
	ZzcColorCompete(pBImg + width*height, pYImg + width*height, pBImg + 5 * width*height, pYImg + 5 * width*height, width, height, pBYCompete + width*height);
	ZzcColorCompete(pBImg + 2 * width*height, pYImg + 2 * width*height, pBImg + 5 * width*height, pYImg + 5 * width*height, width, height, pBYCompete + 2 * width*height);
	ZzcColorCompete(pBImg + 2 * width*height, pYImg + 2 * width*height, pBImg + 6 * width*height, pYImg + 6 * width*height, width, height, pBYCompete + 3 * width*height);
	ZzcColorCompete(pBImg + 3 * width*height, pYImg + 3 * width*height, pBImg + 6 * width*height, pYImg + 6 * width*height, width, height, pBYCompete + 4 * width*height);
	ZzcColorCompete(pBImg + 3 * width*height, pYImg + 3 * width*height, pBImg + 7 * width*height, pYImg + 7 * width*height, width, height, pBYCompete + 5 * width*height);
	ZzcNormalization(pBYCompete, width, height);
	e = clock();
	printf("pBYCompete : %ld ms.\n",e-s);
	RmwWriteByteImg2BmpFile(pBYCompete, width, height, "..//BmpResFile/Dune_Compete_BY_1.bmp");
	RmwWriteByteImg2BmpFile(pBYCompete + width*height, width, height, "..//BmpResFile/Dune_Compete_BY_2.bmp");
	RmwWriteByteImg2BmpFile(pBYCompete + 2 * width*height, width, height, "..//BmpResFile/Dune_Compete_BY_3.bmp");
	RmwWriteByteImg2BmpFile(pBYCompete + 3 * width*height, width, height, "..//BmpResFile/Dune_Compete_BY_4.bmp");
	RmwWriteByteImg2BmpFile(pBYCompete + 4 * width*height, width, height, "..//BmpResFile/Dune_Compete_BY_5.bmp");
	RmwWriteByteImg2BmpFile(pBYCompete + 5 * width*height, width, height, "..//BmpResFile/Dune_Compete_BY_6.bmp");
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//              0度方向特征
	///////////////////////////////////////////////// ////////////////////////////////////////////////////////////
	BYTE *pGabor_0 = new BYTE[width*height * 6];
	s = clock();
	ZzcSubtract(pGaborImg_0 + width*height, pGaborImg_0 + 4 * width*height, width, height, pGabor_0);
	ZzcSubtract(pGaborImg_0 + width*height, pGaborImg_0 + 5 * width*height, width, height, pGabor_0 + width*height);
	ZzcSubtract(pGaborImg_0 + 2 * width*height, pGaborImg_0 + 5 * width*height, width, height, pGabor_0 + 2 * width*height);
	ZzcSubtract(pGaborImg_0 + 2 * width*height, pGaborImg_0 + 6 * width*height, width, height, pGabor_0 + 3 * width*height);
	ZzcSubtract(pGaborImg_0 + 3 * width*height, pGaborImg_0 + 6 * width*height, width, height, pGabor_0 + 4 * width*height);
	ZzcSubtract(pGaborImg_0 + 3 * width*height, pGaborImg_0 + 7 * width*height, width, height, pGabor_0 + 5 * width*height);
	ZzcNormalization(pGabor_0, width, height);
	e = clock();
	printf("0Gabor : %ld ms.\n", e - s);
	RmwWriteByteImg2BmpFile(pGabor_0, width, height, "..//BmpResFile/Dune_Direction_0_1.bmp");
	RmwWriteByteImg2BmpFile(pGabor_0 + width*height, width, height, "..//BmpResFile/Dune_Direction_0_2.bmp");
	RmwWriteByteImg2BmpFile(pGabor_0 + 2 * width*height, width, height, "..//BmpResFile/Dune_Direction_0_3.bmp");
	RmwWriteByteImg2BmpFile(pGabor_0 + 3 * width*height, width, height, "..//BmpResFile/Dune_Direction_0_4.bmp");
	RmwWriteByteImg2BmpFile(pGabor_0 + 4 * width*height, width, height, "..//BmpResFile/Dune_Direction_0_5.bmp");
	RmwWriteByteImg2BmpFile(pGabor_0 + 5 * width*height, width, height, "..//BmpResFile/Dune_Direction_0_6.bmp");
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//              45度方向特征
	///////////////////////////////////////////////// ///////////////////////////////////////////////////////////////////////////
	BYTE *pGabor_45 = new BYTE[width*height * 6];
	s = clock();
	ZzcSubtract(pGaborImg_45 + width*height, pGaborImg_45 + 4 * width*height, width, height, pGabor_45);
	ZzcSubtract(pGaborImg_45 + width*height, pGaborImg_45 + 5 * width*height, width, height, pGabor_45 + width*height);
	ZzcSubtract(pGaborImg_45 + 2 * width*height, pGaborImg_45 + 5 * width*height, width, height, pGabor_45 + 2 * width*height);
	ZzcSubtract(pGaborImg_45 + 2 * width*height, pGaborImg_45 + 6 * width*height, width, height, pGabor_45 + 3 * width*height);
	ZzcSubtract(pGaborImg_45 + 3 * width*height, pGaborImg_45 + 6 * width*height, width, height, pGabor_45 + 4 * width*height);
	ZzcSubtract(pGaborImg_45 + 3 * width*height, pGaborImg_45 + 7 * width*height, width, height, pGabor_45 + 5 * width*height);
	ZzcNormalization(pGabor_45, width, height);
	e = clock();
	printf("45Gabor : %ld ms.\n", e - s);
	RmwWriteByteImg2BmpFile(pGabor_45, width, height, "..//BmpResFile/Dune_Direction_45_1.bmp");
	RmwWriteByteImg2BmpFile(pGabor_45 + width*height, width, height, "..//BmpResFile/Dune_Direction_45_2.bmp");
	RmwWriteByteImg2BmpFile(pGabor_45 + 2 * width*height, width, height, "..//BmpResFile/Dune_Direction_45_3.bmp");
	RmwWriteByteImg2BmpFile(pGabor_45 + 3 * width*height, width, height, "..//BmpResFile/Dune_Direction_45_4.bmp");
	RmwWriteByteImg2BmpFile(pGabor_45 + 4 * width*height, width, height, "..//BmpResFile/Dune_Direction_45_5.bmp");
	RmwWriteByteImg2BmpFile(pGabor_45 + 5 * width*height, width, height, "..//BmpResFile/Dune_Direction_45_6.bmp");
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//              90度方向特征
	///////////////////////////////////////////////// ///////////////////////////////////////////////////////////////////////////
	BYTE *pGabor_90 = new BYTE[width*height * 6];
	s = clock();
	ZzcSubtract(pGaborImg_90 + width*height, pGaborImg_90 + 4 * width*height, width, height, pGabor_90);
	ZzcSubtract(pGaborImg_90 + width*height, pGaborImg_90 + 5 * width*height, width, height, pGabor_90 + width*height);
	ZzcSubtract(pGaborImg_90 + 2 * width*height, pGaborImg_90 + 5 * width*height, width, height, pGabor_90 + 2 * width*height);
	ZzcSubtract(pGaborImg_90 + 2 * width*height, pGaborImg_90 + 6 * width*height, width, height, pGabor_90 + 3 * width*height);
	ZzcSubtract(pGaborImg_90 + 3 * width*height, pGaborImg_90 + 6 * width*height, width, height, pGabor_90 + 4 * width*height);
	ZzcSubtract(pGaborImg_90 + 3 * width*height, pGaborImg_90 + 7 * width*height, width, height, pGabor_90 + 5 * width*height);
	ZzcNormalization(pGabor_90, width, height);
	e = clock();
	printf("90Gabor : %ld ms.\n", e - s);
	RmwWriteByteImg2BmpFile(pGabor_90, width, height, "..//BmpResFile/Dune_Direction_90_1.bmp");
	RmwWriteByteImg2BmpFile(pGabor_90 + width*height, width, height, "..//BmpResFile/Dune_Direction_90_2.bmp");
	RmwWriteByteImg2BmpFile(pGabor_90 + 2 * width*height, width, height, "..//BmpResFile/Dune_Direction_90_3.bmp");
	RmwWriteByteImg2BmpFile(pGabor_90 + 3 * width*height, width, height, "..//BmpResFile/Dune_Direction_90_4.bmp");
	RmwWriteByteImg2BmpFile(pGabor_90 + 4 * width*height, width, height, "..//BmpResFile/Dune_Direction_90_5.bmp");
	RmwWriteByteImg2BmpFile(pGabor_90 + 5 * width*height, width, height, "..//BmpResFile/Dune_Direction_90_6.bmp");
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//              135度方向特征
	///////////////////////////////////////////////// ////////////////////////////////////////////////////////////////////////////
	BYTE *pGabor_135 = new BYTE[width*height * 6];
	s = clock();
	ZzcSubtract(pGaborImg_135 + width*height, pGaborImg_135 + 4 * width*height, width, height, pGabor_135);
	ZzcSubtract(pGaborImg_135 + width*height, pGaborImg_135 + 5 * width*height, width, height, pGabor_135 + width*height);
	ZzcSubtract(pGaborImg_135 + 2 * width*height, pGaborImg_135 + 5 * width*height, width, height, pGabor_135 + 2 * width*height);
	ZzcSubtract(pGaborImg_135 + 2 * width*height, pGaborImg_135 + 6 * width*height, width, height, pGabor_135 + 3 * width*height);
	ZzcSubtract(pGaborImg_135 + 3 * width*height, pGaborImg_135 + 6 * width*height, width, height, pGabor_135 + 4 * width*height);
	ZzcSubtract(pGaborImg_135 + 3 * width*height, pGaborImg_135 + 7 * width*height, width, height, pGabor_135 + 5 * width*height);
	ZzcNormalization(pGabor_135, width, height);
	e = clock();
	printf("135Gabor ; %ld ms.\n", e - s);
	RmwWriteByteImg2BmpFile(pGabor_135, width, height, "..//BmpResFile/Dune_Direction_135_1.bmp");
	RmwWriteByteImg2BmpFile(pGabor_135 + width*height, width, height, "..//BmpResFile/Dune_Direction_135_2.bmp");
	RmwWriteByteImg2BmpFile(pGabor_135 + 2 * width*height, width, height, "..//BmpResFile/Dune_Direction_135_3.bmp");
	RmwWriteByteImg2BmpFile(pGabor_135 + 3 * width*height, width, height, "..//BmpResFile/Dune_Direction_135_4.bmp");
	RmwWriteByteImg2BmpFile(pGabor_135 + 4 * width*height, width, height, "..//BmpResFile/Dune_Direction_135_5.bmp");
	RmwWriteByteImg2BmpFile(pGabor_135 + 5 * width*height, width, height, "..//BmpResFile/Dune_Direction_135_6.bmp");
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//        求和
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	BYTE *pSumImg = new BYTE[width*height];
	ZzcITsum(pIntensity_CenterSurround, pRGCompete, pBYCompete, pGabor_0, pGabor_45, pGabor_90, pGabor_135,width,height,pSumImg);
	RmwWriteByteImg2BmpFile(pSumImg, width, height, "..//BmpResFile/Dune_Sum.bmp");
	delete pGabor_135;
	delete pGabor_90;
	delete pGabor_45;
	delete pGabor_0;
	delete pRGCompete;
	delete pBYCompete;
	delete pSumImg;

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//     测试
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void main(int argc,char *argv[])
{
	clock_t s,e;
	s = clock();
	IT("..//BmpOrgFile/23.bmp");
	e = clock();
	printf("%ld\n", e - s);
	getchar();
	//delete real;
	//delete img;
	//delete pGryImg;
	//delete pResImg;
 	return;
}

