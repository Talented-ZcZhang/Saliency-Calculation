#include <cmath>
#include <cstdio>
#include <windows.h>
#include "..//RmwCPP/bmpFile.h"
using namespace std;
const double pi = 3.1415926;
void ZzcGetGabor(int width, int height, double Theta, double sigma, double Gamma, double Lambda, double Phi, double*real, double*img)//获取对应Gabor滤波器模板
{
	int i, j;double x, y, _x, _y;
	double tmp1, tmp2, tmp3, CosT = cos(Theta),SinT=sin(Theta),_1_div_Lambda=1.0/Lambda;
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			x = j - width / 2;
			y = i - height / 2;

			_x = (double)x*CosT + (double)y*SinT;
			_y = (double)y*CosT - (double)x*SinT;

			tmp1 = exp(-(pow(_x, 2) + pow(_y*Gamma, 2)) / (2 * pow(sigma, 2)));
			tmp2 = cos(2 * pi*_x * _1_div_Lambda + Phi);
			tmp3 = sin(2 * pi*_x * _1_div_Lambda + Phi);

			tmp2 *= tmp1;
			tmp3 *= tmp1;
			*(real++) = tmp2;
			*(img++) = tmp3;
		}
	}
}
void ZzcGaborFilter(BYTE*pImg, int width, int height, double* real, double* img, int size, BYTE*pResImg)//Gabor滤波
{
	memset(pResImg, 0, width*height);
	int i, j, x, y,filtersize=(2*size+1)*(2*size+1);
	double *prel, *pimg, a, b, c;
	for (i = size,pResImg+=size; i < height - size; i++,pResImg+=(size+size))
	{
		for (j = size; j < width - size; j++)
		{
			a = b = 0;
			for (prel = real, pimg = img, y = i - size; y <= i + size; y++)
			{
				for (x = j - size; x <= j + size; x++)
				{
					a += *(pImg + y*width + x)*(*(prel++));
					b += *(pImg + y*width + x)*(*(pimg++));
					//printf("%d %d %d %d\n", i, j, y, x);
					//system("pause");
				}
			}
			c = sqrt(a*a+ b*b);
			c /= filtersize;
			//c = a / filtersize;
			*(pResImg++) = (BYTE)(int)c;
		}
	}
}
void ZzcGaborFilter2(BYTE*pImg, int width, int height, double* real, double* img, int size, BYTE*pResImg)//稍加修改的Gabor滤波
{
	memset(pResImg, 0, width*height);
	int i, j, x, y,yUp,yDown,xLeft,xRight,filterwidth=2*size+1;
	double *prel, *pimg, a, b, c, filtersize = 1.0 / ((2 * size + 1)*(2 * size + 1));
	for (i = 0; i < height ; i++)
	{
		for (j = 0; j < width ; j++)
		{
			a = b = 0;
			yUp = max(0, size - i); yDown = min(2 * size, size + height - i-1);
			xLeft = max(0, size - j); xRight = min(2 * size, size + width - j - 1);
			for (y = yUp; y <= yDown; y++)
			{
				for (x = xLeft; x <= xRight; x++)
				{
					
					a += *(pImg + (i - size + y)*width + j - size + x)*(*(real + y*filterwidth + x));
					b += *(pImg + (i - size + y)*width + j - size + x)*(*(img + y*filterwidth + x));
				}
			}
			//c = sqrt(a*a + b*b);
			if (a < 0)a = -a;
			if (b < 0)b = -b;
			c = a + b;
			c *= filtersize;
			//c = a / filtersize;
			*(pResImg++) = (BYTE)(int)c;
		}
	}
}
void ZzcGaussFilter(BYTE*pImg, int width, int height, int size,BYTE*pResImg)   //灰度图的高斯滤波
{
	memset(pResImg, 0, width*height);
	long long *Intergal = new long long[width*height];
	CalIntergal(pImg, width, height, Intergal);
	int i, j, filtersize = (2 * size + 1)*(2 * size + 1);
	for (i = 0 ; i < height ; i++)
	{
		for (j = 0; j < width ; j++)
		{
			if (j < size + 1&&i>=size+1&&i<height-size)
			{
				*(pResImg++)=(*(Intergal+(i+size)*width+j+size)-*(Intergal+(i-size-1)*width+j+size))/((j+size+1)*(2*size+1));
			}
			else if (j >= width - size&&i >= size + 1 && i<height - size)
			{
				*(pResImg++)=(*(Intergal+(i+size)*width+width-1)-*(Intergal+(i-size-1)*width+width-1)
					-*(Intergal+(i+size)*width+j-size-1)+*(Intergal+(i-size-1)*width+j-size-1))/((width-j+size)*(2*size+1));
			}
			else if (i < size + 1&&j>=size+1&&j<width-size)
			{
				*(pResImg++)=(*(Intergal+(i+size)*width+j+size)-*(Intergal+(i+size)*width+j-size-1))/((i+size+1)*(2*size+1));
			}
			else if (i >= height - size&&j >= size + 1 && j<width - size)
			{
				*(pResImg++)=(*(Intergal+(height-1)*width+j+size)-*(Intergal+(i-size-1)*width+j+size)
					-*(Intergal+(height-1)*width+j-size-1)+*(Intergal+(i-size-1)*width+j-size-1))/((height-i+size)*(2*size+1));
			}
			else if (i < size + 1 && j < size + 1)
			{
				*(pResImg++)=(*(Intergal+(i+size)*width+j+size))/((i+size+1)*(j+size+1));
			}
			else if (i < size + 1 && j >= width - size)
			{
				*(pResImg++)=(*(Intergal+(i+size)*width+width-1)-*(Intergal+(i+size)*width+j-size-1))/((i+size+1)*(width-j+size));
			}
			else if (i >= height - size&&j < size + 1)
			{
				*(pResImg++)=(*(Intergal+(height-1)*width+j+size)-*(Intergal+(i-size-1)*width+j+size))/((height-i+size)*(j+size+1));
			}
			else if (i >= height - size&&j >= width - size)
			{
				*(pResImg++)=(*(Intergal+(height-1)*width+width-1)-*(Intergal+(i-size-1)*width+width-1)
					-*(Intergal+(height-1)*width+j-size-1)+*(Intergal+(i-size-1)*width+j-size-1))/((height-i+size)*(width-j+size));
			}
			else
			*(pResImg++) = (*(Intergal + (i + size)*width + j + size) - *(Intergal + (i - size - 1)*width + j + size)
				- *(Intergal +(i+size)*width + j - size - 1) + *(Intergal + (i - size - 1)*width + j - size - 1))/filtersize;
		}
	}
}

void ZzcGaussFilter_3(BYTE*pImg, int width, int height, int size, BYTE*pResImg,long long*Intergal)  //彩色图的高斯滤波
{
	memset(pResImg, 0, width*height*3);
	//long long *Intergal = new long long[width*height*3];
	//CalIntergal_3(pImg, width, height, Intergal);
	int i, j, filtersize = (2 * size + 1)*(2 * size + 1);
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			if (j < size + 1 && i >= size + 1 && i<height - size)
			{
				*(pResImg++) = (*(Intergal + (i + size)*width * 3 + (j + size) * 3) - *(Intergal + (i - size - 1)*width * 3 + (j + size) * 3)) / ((j + size + 1)*(2 * size + 1));
				*(pResImg++) = (*(Intergal + (i + size)*width * 3 + (j + size) * 3 + 1) - *(Intergal + (i - size - 1)*width * 3 + (j + size) * 3 + 1)) / ((j + size + 1)*(2 * size + 1));
				*(pResImg++) = (*(Intergal + (i + size)*width * 3 + (j + size) * 3 + 2) - *(Intergal + (i - size - 1)*width * 3 + (j + size) * 3 + 2)) / ((j + size + 1)*(2 * size + 1));
			}
			else if (j >= width - size&&i >= size + 1 && i<height - size)
			{
				*(pResImg++) = (*(Intergal + (i + size)*width*3 + (width - 1)*3) - *(Intergal + (i - size - 1)*width*3 + (width - 1)*3)
					- *(Intergal + (i + size)*width*3 + (j - size - 1)*3) + *(Intergal + (i - size - 1)*width*3 + (j - size - 1)*3)) / ((width - j + size)*(2 * size + 1));
				*(pResImg++) = (*(Intergal + 1 + (i + size)*width * 3 + (width - 1) * 3) - *(Intergal + 1 + (i - size - 1)*width * 3 + (width - 1) * 3)
					- *(Intergal + 1 + (i + size)*width * 3 + (j - size - 1) * 3) + *(Intergal + 1 + (i - size - 1)*width * 3 + (j - size - 1) * 3)) / ((width - j + size)*(2 * size + 1));
				*(pResImg++) = (*(Intergal + 2 + (i + size)*width * 3 + (width - 1) * 3) - *(Intergal + 2 + (i - size - 1)*width * 3 + (width - 1) * 3)
					- *(Intergal + 2 + (i + size)*width * 3 + (j - size - 1) * 3) + *(Intergal + 2  + (i - size - 1)*width * 3 + (j - size - 1) * 3)) / ((width - j + size)*(2 * size + 1));
			}
			else if (i < size + 1 && j >= size + 1 && j<width - size)
			{
				*(pResImg++) = (*(Intergal + (i + size)*width * 3 + (j + size) * 3) - *(Intergal + (i + size)*width * 3 + (j - size - 1) * 3)) / ((i + size + 1)*(2 * size + 1));
				*(pResImg++) = (*(Intergal + 1 + (i + size)*width * 3 + (j + size) * 3) - *(Intergal + 1 + (i + size)*width * 3 + (j - size - 1) * 3)) / ((i + size + 1)*(2 * size + 1));
				*(pResImg++) = (*(Intergal + 2 + (i + size)*width * 3 + (j + size) * 3) - *(Intergal + 2 + (i + size)*width * 3 + (j - size - 1) * 3)) / ((i + size + 1)*(2 * size + 1));
			}
			else if (i >= height - size&&j >= size + 1 && j<width - size)
			{
				*(pResImg++) = (*(Intergal + (height - 1)*width * 3 + (j + size) * 3) - *(Intergal + (i - size - 1)*width * 3 + (j + size) * 3)
					- *(Intergal + (height - 1)*width * 3 + (j - size - 1) * 3) + *(Intergal + (i - size - 1)*width * 3 + (j - size - 1) * 3)) / ((height - i + size)*(2 * size + 1));
				*(pResImg++) = (*(Intergal + 1 + (height - 1)*width * 3 + (j + size) * 3) - *(Intergal + 1 + (i - size - 1)*width * 3 + (j + size) * 3)
					- *(Intergal + 1 + (height - 1)*width * 3 + (j - size - 1) * 3) + *(Intergal + 1 + (i - size - 1)*width * 3 + (j - size - 1) * 3)) / ((height - i + size)*(2 * size + 1));
				*(pResImg++) = (*(Intergal + 2 + (height - 1)*width * 3 + (j + size) * 3) - *(Intergal + 2 + (i - size - 1)*width * 3 + (j + size) * 3)
					- *(Intergal + 2 + (height - 1)*width * 3 + (j - size - 1) * 3) + *(Intergal + 2 + (i - size - 1)*width * 3 + (j - size - 1) * 3)) / ((height - i + size)*(2 * size + 1));
			}
			else if (i < size + 1 && j < size + 1)
			{
				*(pResImg++) = (*(Intergal + (i + size)*width * 3 + (j + size) * 3)) / ((i + size + 1)*(j + size + 1));
				*(pResImg++) = (*(Intergal + 1 + (i + size)*width * 3 + (j + size) * 3)) / ((i + size + 1)*(j + size + 1));
				*(pResImg++) = (*(Intergal + 2 + (i + size)*width * 3 + (j + size) * 3)) / ((i + size + 1)*(j + size + 1));
			}
			else if (i < size + 1 && j >= width - size)
			{
				*(pResImg++) = (*(Intergal + (i + size)*width * 3 + (width - 1) * 3) - *(Intergal + (i + size)*width * 3 + (j - size - 1) * 3)) / ((i + size + 1)*(width - j + size));
				*(pResImg++) = (*(Intergal + 1 + (i + size)*width * 3 + (width - 1) * 3) - *(Intergal + 1 + (i + size)*width * 3 + (j - size - 1) * 3)) / ((i + size + 1)*(width - j + size));
				*(pResImg++) = (*(Intergal + 2 + (i + size)*width * 3 + (width - 1) * 3) - *(Intergal + 2 + (i + size)*width * 3 + (j - size - 1) * 3)) / ((i + size + 1)*(width - j + size));
			}
			else if (i >= height - size&&j < size + 1)
			{
				*(pResImg++) = (*(Intergal + (height - 1)*width * 3 + (j + size) * 3) - *(Intergal + (i - size - 1)*width * 3 + (j + size) * 3)) / ((height - i + size)*(j + size + 1));
				*(pResImg++) = (*(Intergal + 1 + (height - 1)*width * 3 + (j + size) * 3) - *(Intergal + 1 + (i - size - 1)*width * 3 + (j + size) * 3)) / ((height - i + size)*(j + size + 1));
				*(pResImg++) = (*(Intergal + 2 + (height - 1)*width * 3 + (j + size) * 3) - *(Intergal + 2 + (i - size - 1)*width * 3 + (j + size) * 3)) / ((height - i + size)*(j + size + 1));
			}
			else if (i >= height - size&&j >= width - size)
			{
				*(pResImg++) = (*(Intergal + (height - 1)*width * 3 + (width - 1) * 3) - *(Intergal + (i - size - 1)*width * 3 + (width - 1) * 3)
					- *(Intergal + (height - 1)*width * 3 + (j - size - 1) * 3) + *(Intergal + (i - size - 1)*width * 3 + (j - size - 1) * 3)) / ((height - i + size)*(width - j + size));
				*(pResImg++) = (*(Intergal + 1 + (height - 1)*width * 3 + (width - 1) * 3) - *(Intergal + 1 + (i - size - 1)*width * 3 + (width - 1) * 3)
					- *(Intergal + 1 + (height - 1)*width * 3 + (j - size - 1) * 3) + *(Intergal + 1 + (i - size - 1)*width * 3 + (j - size - 1) * 3)) / ((height - i + size)*(width - j + size));
				*(pResImg++) = (*(Intergal + 2 + (height - 1)*width * 3 + (width - 1) * 3) - *(Intergal + 2 + (i - size - 1)*width * 3 + (width - 1) * 3)
					- *(Intergal + 2 + (height - 1)*width * 3 + (j - size - 1) * 3) + *(Intergal + 2 + (i - size - 1)*width * 3 + (j - size - 1) * 3)) / ((height - i + size)*(width - j + size));
			}
			else
			{
				*(pResImg++) = (*(Intergal + (i + size)*width * 3 + (j + size) * 3) - *(Intergal + (i - size - 1)*width * 3 + (j + size) * 3)
					- *(Intergal + (i + size)*width * 3 + (j - size - 1) * 3) + *(Intergal + (i - size - 1)*width * 3 + (j - size - 1) * 3)) / filtersize;
				*(pResImg++) = (*(Intergal + 1 + (i + size)*width * 3 + (j + size) * 3) - *(Intergal + 1 + (i - size - 1)*width * 3 + (j + size) * 3)
					- *(Intergal + 1 + (i + size)*width * 3 + (j - size - 1) * 3) + *(Intergal + 1 + (i - size - 1)*width * 3 + (j - size - 1) * 3)) / filtersize;
				*(pResImg++) = (*(Intergal + 2 + (i + size)*width * 3 + (j + size) * 3) - *(Intergal + 2 + (i - size - 1)*width * 3 + (j + size) * 3)
					- *(Intergal + 2 + (i + size)*width * 3 + (j - size - 1) * 3) + *(Intergal + 2 + (i - size - 1)*width * 3 + (j - size - 1) * 3)) / filtersize;
			}
		}
	}
}