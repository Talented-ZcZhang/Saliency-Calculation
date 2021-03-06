///////////////////////////////////////////////////////////////////////////////////
//
//
//  Date: 2013.10.07
//
//
///////////////////////////////////////////////////////////////////////////////////
#define _CRT_SECURE_NO_WARNINGS
#define  _CRT_SECURE_NO_DEPRECATE
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

///////////////////////////////////////////////////////////////////////////////////
//
//
//  8 Bits
//
//
///////////////////////////////////////////////////////////////////////////////////
BYTE *RmwRead8BitBmpFile2Img(const char * filename,int *width,int *height)
{   //BmpHeader记录的真实的宽度,
	//读出的是真实的宽度,去掉了4对齐的扩展.
	FILE *fp;
	BITMAPFILEHEADER FileHeader;
	BITMAPINFOHEADER BmpHeader;
	BYTE *pImg;
	unsigned int size;
	int Suc=1,w,h;
	int k,extend,tmp;

	// Open File
	*width=*height=0;
	if((fp=fopen(filename,"rb"))==NULL) return NULL;
	// Read Struct Info
	if (fread((void *)&FileHeader,1,sizeof(FileHeader),fp)!=sizeof(FileHeader)) Suc=-1;
	if (fread((void *)&BmpHeader,1,sizeof(BmpHeader),fp)!=sizeof(BmpHeader)) Suc=-1;
    if ( (Suc==-1) || 
		 (FileHeader.bfOffBits<sizeof(FileHeader)+sizeof(BmpHeader) )
	   ) 
	{ 
		fclose(fp); 
		return NULL; 
	}
	// Read Image Data
	*width=w=BmpHeader.biWidth;
	//*width=w=(BmpHeader.biWidth+3)/4*4;
	*height=h=BmpHeader.biHeight;
    extend=(w+3)/4*4-w;
	size=BmpHeader.biWidth*BmpHeader.biHeight;
	fseek(fp,FileHeader.bfOffBits,SEEK_SET);
	if ( (pImg=new BYTE[size])!=NULL)
	{
		for(int i=0;i<h;i++)  // 0,1,2,3,4(5): 400-499
		{
			if ((int)fread(pImg+(h-1-i)*w,sizeof(BYTE),w,fp)!=w)
			{ 
				fclose(fp);
				delete pImg;
				pImg=NULL;
				return NULL;
			}
			//读掉扩充的数据
			for(k=0;k<extend;k++) 
			{ 
				if (fread(&tmp,1,1,fp)!=1) 
				{
					fclose(fp);
					delete pImg;
					pImg=NULL;
					return NULL;
					
				}
			}
		}
	}
	fclose(fp);
	return pImg;
}

bool RmwWriteByteImg2BmpFile(BYTE *pImg,int width,int height,const char * filename)
{   //BmpHeader记录的真实的宽度
	//当宽度不是4的倍数时自动添加成4的倍数,进行了4对齐的扩展.
	FILE * fp;
    BITMAPFILEHEADER FileHeader;
    BITMAPINFOHEADER BmpHeader;
    int i,extend;
	bool Suc=true;
	BYTE p[4],*pCur;

    // Open File
    if((fp=fopen(filename,"w+b"))==NULL) {  return false; }
	// Fill the FileHeader
	FileHeader.bfType= ((WORD) ('M' << 8) | 'B');
	FileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+256*4L;
    FileHeader.bfSize=FileHeader.bfOffBits+width*height ;
    FileHeader.bfReserved1=0;
    FileHeader.bfReserved2=0;
	if (fwrite((void *)&FileHeader,1,sizeof(FileHeader),fp)!=sizeof(FileHeader)) Suc=false;
	// Fill the ImgHeader
	BmpHeader.biSize = 40;
    BmpHeader.biWidth = width;
	BmpHeader.biHeight = height;
	BmpHeader.biPlanes = 1 ;
	BmpHeader.biBitCount = 8 ;
	BmpHeader.biCompression = 0 ;
	BmpHeader.biSizeImage = 0 ;
	BmpHeader.biXPelsPerMeter = 0;
	BmpHeader.biYPelsPerMeter = 0;
	BmpHeader.biClrUsed = 0;
	BmpHeader.biClrImportant = 0;
	if (fwrite((void *)&BmpHeader,1,sizeof(BmpHeader),fp)!=sizeof(BmpHeader)) Suc=false;
    // write Pallete
    for (i=0,p[3]=0;i<256;i++) 
    {  
	   p[3]=0;
	   p[0]=p[1]=p[2]=i; // blue,green,red;
       if (i==254)
	   {
		   p[0]=0;
		   p[1]=0;
		   p[2]=255;
	   }
	   if (fwrite((void *)p,1,4,fp)!=4) { Suc=false; break; }
	}
    // write image data
	extend=(width+3)/4*4-width;
	if (extend==0)
	{   
		for(pCur=pImg+(height-1)*width;pCur>=pImg;pCur-=width)
		{   
		    if (fwrite((void *)pCur,1,width,fp)!=(unsigned int)width) Suc=false; // 真实的数据
		}
	}
	else
	{   
		for(pCur=pImg+(height-1)*width;pCur>=pImg;pCur-=width)
		{   
		    if (fwrite((void *)pCur,1,width,fp)!=(unsigned int)width) Suc=false; // 真实的数据
			for(i=0;i<extend;i++) // 扩充的数据
		       if (fwrite((void *)(pCur+width-1),1,1,fp)!=1) Suc=false;
		}
	}
	// return;
	fclose(fp);
	return Suc;
}

bool RmwWrite8BitImg2BmpFileMark254Green255Red(BYTE *pImg,int width,int height,const char * filename)
{   //BmpHeader记录的真实的宽度
	//当宽度不是4的倍数时自动添加成4的倍数,进行了4对齐的扩展.
	FILE * fp;
    BITMAPFILEHEADER FileHeader;
    BITMAPINFOHEADER BmpHeader;
    int i,extend;
	bool Suc=true;
	BYTE p[4],*pCur;

    // Open File
    if((fp=fopen(filename,"w+b"))==NULL) {  return false; }
	// Fill the FileHeader
	FileHeader.bfType= ((WORD) ('M' << 8) | 'B');
	FileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+256*4L;
    FileHeader.bfSize=FileHeader.bfOffBits+width*height ;
    FileHeader.bfReserved1=0;
    FileHeader.bfReserved2=0;
	if (fwrite((void *)&FileHeader,1,sizeof(FileHeader),fp)!=sizeof(FileHeader)) Suc=false;
	// Fill the ImgHeader
	BmpHeader.biSize = 40;
    BmpHeader.biWidth = width;
	BmpHeader.biHeight = height;
	BmpHeader.biPlanes = 1 ;
	BmpHeader.biBitCount = 8 ;
	BmpHeader.biCompression = 0 ;
	BmpHeader.biSizeImage = 0 ;
	BmpHeader.biXPelsPerMeter = 0;
	BmpHeader.biYPelsPerMeter = 0;
	BmpHeader.biClrUsed = 0;
	BmpHeader.biClrImportant = 0;
	if (fwrite((void *)&BmpHeader,1,sizeof(BmpHeader),fp)!=sizeof(BmpHeader)) Suc=false;
    // write Pallete
    for (i=0,p[3]=0;i<256;i++) 
    {  
		p[3]=0;
		if (i==255)
		{
			p[0]=0;
			p[1]=0;
			p[2]=255;
		}
		else if (i==254)
		{
			p[0]=0;
			p[1]=255;
			p[2]=0;
		}
		else if (i==253)
		{
			p[0]=255;
			p[1]=0;
			p[2]=0;
		}
		else if (i==252)
		{
			p[0]=0;
			p[1]=255;
			p[2]=255;
		}
		else
		{
			p[0]=p[1]=p[2]=i; // blue,green,red;
		}
       if (fwrite((void *)p,1,4,fp)!=4) { Suc=false; break; }
	}
    // write image data
	extend=(width+3)/4*4-width;
	if (extend==0)
	{   
		for(pCur=pImg+(height-1)*width;pCur>=pImg;pCur-=width)
		{   
		    if (fwrite((void *)pCur,1,width,fp)!=(unsigned int)width) Suc=false; // 真实的数据
		}
	}
	else
	{   
		for(pCur=pImg+(height-1)*width;pCur>=pImg;pCur-=width)
		{   
		    if (fwrite((void *)pCur,1,width,fp)!=(unsigned int)width) Suc=false; // 真实的数据
			for(i=0;i<extend;i++) // 扩充的数据
		       if (fwrite((void *)(pCur+width-1),1,1,fp)!=1) Suc=false;
		}
	}
	// return;
	fclose(fp);
	return Suc;
}
///////////////////////////////////////////////////////////////////////////////////
//
//
//  24 Bits
//
//
///////////////////////////////////////////////////////////////////////////////////
BYTE *RmwRead24BitBmpFile2Img(const char * filename,int *width,int *height)
{   //BmpHeader记录的真实的宽度,
	//读出的是真实的宽度,去掉了4对齐的扩展.
	FILE *fp;
    BITMAPFILEHEADER FileHeader;
    BITMAPINFOHEADER BmpHeader;
	BYTE *pImg;
	unsigned int size;
    int Suc=1,w,h,k,extend,tmp;

	// Open File
	*width=*height=0;
	if((fp=fopen(filename,"rb"))==NULL) return NULL;
	// Read Struct Info
    if (fread((void *)&FileHeader,1,sizeof(FileHeader),fp)!=sizeof(FileHeader)) Suc=-1;
	if (fread((void *)&BmpHeader,1,sizeof(BmpHeader),fp)!=sizeof(BmpHeader)) Suc=-1;
	if ( (Suc==-1) || 
		 (FileHeader.bfOffBits<sizeof(FileHeader)+sizeof(BmpHeader) )
	   ) 
	{ 
		fclose(fp); 
		return NULL; 
	}
	// Read Image Data
	*width=w=BmpHeader.biWidth;
	*height=h=BmpHeader.biHeight;
	extend=((w*3+3)/4*4)-w*3;
	size=(*width)*(*height)*3;
	fseek(fp,FileHeader.bfOffBits,SEEK_SET);
    if ( (pImg=new BYTE[size])!=NULL)
    {   
		for(int i=0;i<h;i++)
		{
			if((int)fread(pImg+(h-1-i)*w*3,sizeof(BYTE),w*3,fp)!=w*3)
			{ 
				fclose(fp);
				delete pImg;
				pImg=NULL;
				return NULL;
			}
			for(k=0;k<extend;k++) // 扩充的数据
			{ 
			  if (fwrite((void *)&tmp,sizeof(BYTE),1,fp)!=1) Suc=false;
			}
		}
    }
    fclose(fp);
    return pImg;
}

bool RmwWrite24BitImg2BmpFile(BYTE *pImg,int width,int height,const char * filename)
{   //BmpHeader记录的真实的宽度
	//当宽度不是4的倍数时自动添加成4的倍数,进行了4对齐的扩展.
	FILE *fp;
    BITMAPFILEHEADER FileHeader;
    BITMAPINFOHEADER BmpHeader;
    bool Suc=true;
    int i,extend;
	BYTE *pCur;

    // Open File
    if((fp=fopen(filename,"w+b"))==NULL) {  return false; }
	// Fill the FileHeader
	FileHeader.bfType= ((WORD) ('M' << 8) | 'B');
	FileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
    FileHeader.bfSize=FileHeader.bfOffBits+width*height*3L ;
    FileHeader.bfReserved1=0;
    FileHeader.bfReserved2=0;
	if (fwrite((void *)&FileHeader,1,sizeof(BITMAPFILEHEADER),fp)!=sizeof(BITMAPFILEHEADER)) Suc=false;
	// Fill the ImgHeader
	BmpHeader.biSize = 40;
    BmpHeader.biWidth = width;
	BmpHeader.biHeight = height;
	BmpHeader.biPlanes = 1 ;
	BmpHeader.biBitCount = 24 ;
	BmpHeader.biCompression = 0 ;
	BmpHeader.biSizeImage = 0 ;
	BmpHeader.biXPelsPerMeter = 0;
	BmpHeader.biYPelsPerMeter = 0;
	BmpHeader.biClrUsed = 0;
	BmpHeader.biClrImportant = 0;
	if (fwrite((void *)&BmpHeader,1,sizeof(BITMAPINFOHEADER),fp)!=sizeof(BITMAPINFOHEADER)) Suc=false;
	// write image data
	extend=((width*3+3)/4*4)-width*3;
	if (extend==0)
	{   
		for(pCur=pImg+(height-1)*3*width;pCur>=pImg;pCur-=3*width)
		{   
		   if (fwrite((void *)pCur,1,width*3,fp)!=(unsigned int)(3*width)) Suc=false; // 真实的数据
		}
	}
	else
	{   
		for(pCur=pImg+(height-1)*3*width;pCur>=pImg;pCur-=3*width)
		{   
		    if (fwrite((void *)pCur,1,width*3,fp)!=(unsigned int)(3*width)) Suc=false; // 真实的数据
			for(i=0;i<extend;i++) // 扩充的数据
			{ 
			  if (fwrite((void *)(pCur+3*(width-1)+0),1,1,fp)!=1) Suc=false;
			}
		}
	}
	// return;
	fclose(fp);
	return Suc;
}
 
void CalIntergal(BYTE* pImg,int width,int height,long long*Intergal)   //对灰度图像做积分图
{
	if(width<=0||height<=0)return;
	BYTE*pCur,*pEnd;
	long long *pInte=Intergal;
	memset(Intergal,0,width*height*8);
	*pInte=*pImg;
	for(pInte=Intergal+1,pCur=pImg+1,pEnd=pImg+width;pCur<pEnd;)
	{
		*(pInte++)=*(pInte-1)+*(pCur++);
	}
	for(pInte=Intergal+width,pCur=pImg+width,pEnd=pImg+width*height;pCur<pEnd;pCur+=width,pInte+=width) 
	{
		*(pInte)=*(pInte-width)+*pCur;
	}

	int x=0,y=0;
	for(y=1,pInte=Intergal+width+1,pCur=pImg+width+1;y<height;y++,pInte++,pCur++)//jump the first column
	{
		for(x=1;x<width;x++)
		{
			*(pInte++)=*(pInte-1)+*(pInte-width)-*(pInte-width-1)+*(pCur++);
		}
	}
}
void CalIntergal_3(BYTE* pImg, int width, int height, long long*Intergal) //对彩色图像做积分图
{
	if (width <= 0 || height <= 0)return;
	BYTE*pCur, *pEnd;
	long long *pInte = Intergal;
	memset(Intergal, 0, width*height * 8 * 3 );
	*pInte = *pImg; *(pInte + 1) = *(pImg + 1); *(pInte + 2) = *(pImg + 2);
	for (pInte = Intergal + 3, pCur = pImg + 3, pEnd = pImg + width*3; pCur<pEnd;)
	{
		*(pInte++) = *(pInte - 3) + *(pCur++);
		*(pInte++) = *(pInte - 3) + *(pCur++);
		*(pInte++) = *(pInte - 3) + *(pCur++);
	}
	for (pInte = Intergal + width*3, pCur = pImg + width*3, pEnd = pImg + width*height*3; pCur<pEnd; pCur += width*3, pInte += width*3)
	{
		*(pInte) = *(pInte - width*3) + *pCur;
		*(pInte + 1) = *(pInte + 1 - width * 3) + *(pCur + 1);
		*(pInte + 2) = *(pInte + 2 - width * 3) + *(pCur + 2);
	}
	int x = 0, y = 0;
	for (y = 1, pInte = Intergal + width*3 + 3, pCur = pImg + width*3 + 3; y<height; y++, pInte+=3, pCur+=3)//jump the first column
	{
		for (x = 1; x<width; x++)
		{
			*(pInte++) = *(pInte - 3) + *(pInte - width*3) - *(pInte - width*3 - 3) + *(pCur++);
			*(pInte++) = *(pInte - 3) + *(pInte - width*3) - *(pInte - width*3 - 3) + *(pCur++);
			*(pInte++) = *(pInte - 3) + *(pInte - width*3) - *(pInte - width*3 - 3) + *(pCur++);
		}
	}
}
void ZzcSubtract(BYTE*pImg1, BYTE*pImg2, int width, int height, BYTE*pResImg)  //对图像对应像素作减法
{
	BYTE*pEnd = pImg1 + width*height;
	while (pImg1 < pEnd)
	{
		*(pResImg++) = abs(*(pImg1++)-*(pImg2++));
	}
}
void ZzcColorCompete(BYTE*Color1_c, BYTE*Color2_c,BYTE*Color1_s,BYTE*Color2_s, int width, int height, BYTE*pResImg) //颜色竞争
{
	BYTE*pEnd = Color1_c + width*height;
	while (Color1_c < pEnd)
	{
		*(pResImg++) = abs(((int)*(Color1_c++)-(int)*(Color2_c++)) - ((int)*(Color2_s++)-(int)*(Color1_s++))) / 2;
	}
}