#include <windows.h>
#include <cmath>
void ZzcIntensityExtract(BYTE*pImg, int width, int height, BYTE*pResImg)//������ȡ
{
	BYTE*pEnd = pImg + width*height * 3;
	for (; pImg < pEnd; pImg += 3)
	{
		*(pResImg++) = ((int)*(pImg)+*(pImg + 1) + *(pImg + 2)) * (1.0/3);
	}
}
void Zzcrgb2RGBY(BYTE*pImg, int width, int height, BYTE*pResR, BYTE*pResG, BYTE*pResB, BYTE*pResY)  //��rgbת����RGBY
{
	BYTE*pEnd = pImg + width*height * 3;
	for (; pImg < pEnd; pImg += 3)
	{
		*(pResR++) = max((int)*(pImg + 2) - ((int)*(pImg)+*(pImg + 1)) *0.5, 0);
		*(pResG++) = max((int)*(pImg + 1) - ((int)*(pImg)+*(pImg + 2)) *0.5, 0);
		*(pResB++) = max((int)*(pImg)-((int)*(pImg + 2) + *(pImg + 1)) *0.5, 0);
		*(pResY++) = max(((int)*(pImg + 2) + *(pImg + 1)) *0.5 - abs((int)*(pImg + 2) - *(pImg + 1)) *0.5 - *(pImg), 0);
	}
}
void ZzcSeparateRGBY(BYTE*pImg, int width, int height, BYTE*pResR, BYTE*pResG, BYTE*pResB, BYTE*pResY) //��һ��32λRGBY����Ϊ4��8λ��R,G,B,Yͼ
{
	BYTE*pEnd = pImg + width*height * 4;
	while (pImg < pEnd)
	{
		*(pResR++) = *(pImg++);
		*(pResG++) = *(pImg++);
		*(pResB++) = *(pImg++);
		*(pResY++) = *(pImg++);
	}
}