#include <cmath>
#include <cstdio>
#include <windows.h>
using namespace std;
void ZzcGetGabor(int width, int height, double Theta, double sigma, double Gamma, double Lambda, double Phi, double*real, double*img);
void ZzcGaborFilter(BYTE*pImg, int width, int height, double* real, double* img, int size, BYTE*pResImg);
void ZzcGaborFilter2(BYTE*pImg, int width, int height, double* real, double* img, int size, BYTE*pResImg);
void ZzcGaussFilter(BYTE*pImg, int width, int height, int size,BYTE *pResImg);
void ZzcGaussFilter_3(BYTE*pImg, int width, int height, int size, BYTE*pResImg,long long*Intergal);