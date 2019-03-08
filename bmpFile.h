#ifndef BMP_FILE_H
#define BMP_FILE_H

BYTE *RmwRead8BitBmpFile2Img(const char * filename,int *width,int *height);
bool RmwWriteByteImg2BmpFile(BYTE *pImg,int width,int height,const char * filename);

BYTE *RmwRead24BitBmpFile2Img(const char * filename,int *width,int *height);
bool RmwWrite24BitImg2BmpFile(BYTE *pImg,int width,int height,const char * filename);

void CalIntergal(BYTE* pImg,int width,int height,long long*Intergal);
void CalIntergal_3(BYTE* pImg, int width, int height, long long*Intergal);
void ZzcSubtract(BYTE*pImg1, BYTE*pImg2, int width, int height, BYTE*pResImg);
void ZzcColorCompete(BYTE*Color1_c, BYTE*Color2_c, BYTE*Color1_s, BYTE*Color2_s, int width, int height, BYTE*pResImg);
#endif
