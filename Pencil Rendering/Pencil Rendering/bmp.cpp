#include "bmp.h"
#include <cstdlib>
   
//** 构造函数 */
CBMPLoader::CBMPLoader()   
{   
   //** 初始化成员值为0 */
    image = 0;   
    imageWidth = 0;   
    imageHeight = 0;   
}   
   
//** 析构函数 */
CBMPLoader::~CBMPLoader()   
{
	FreeImage(); //**< 释放图像数据占据的内存 */
}

//** 装载一个位图文件 */
bool CBMPLoader::LoadBMP(const char *file)
{
	GLint totalBytes,
		lineBytes;

	FILE *pFile = fopen(file, "rb");
	if (!pFile)
		return 0;

	fseek(pFile, 0x0012, SEEK_SET);
	fread(&imageWidth, 4, 1, pFile);
	fread(&imageHeight, 4, 1, pFile);
	fseek(pFile, 54, SEEK_SET);

	lineBytes = imageWidth * 3;
	while (lineBytes % 4 != 0)
		++lineBytes;
	totalBytes = lineBytes * imageHeight;

	image = new GLubyte[totalBytes];
	if (!image)
	{
		fclose(pFile);
		return 0;
	}

	if (fread(image, totalBytes, 1, pFile) <= 0)
	{
		delete image;
		fclose(pFile);
		return 0;
	}

	unsigned char textureColors;

    //** 将图像颜色数据格式进行交换,由BGR转换为RGB */
    for(int index = 0; index < totalBytes; index+=3)   
       {   
           textureColors = image[index];
           image[index] = image[index + 2];
           image[index + 2] = textureColors;
       }   
     
    fclose(pFile);       //**< 关闭文件 */
    return true;         //**< 成功返回 */
}

//** 释放内存 */
void CBMPLoader::FreeImage()   
{
    //** 释放分配的内存 */
	if(image)
		{
			delete[] image;   
			image = 0;   
		}
}

bool CBMPLoader::SaveBMP(const char *fileName, GLint width, GLint height, GLint size, GLubyte *data)
{
    FILE *pFile;
    
    pFile = fopen(fileName, "wb");
    
    if (!pFile)
        exit(0);
    
    unsigned char bmpHeader[54] = {0};
    bmpHeader[0] = 0x42;
    bmpHeader[1] = 0x4D;
    bmpHeader[2] = 0x3A;
    bmpHeader[10] = 0x36;
    bmpHeader[14] = 0x28;
    bmpHeader[18] = 0x01;
    bmpHeader[22] = 0x01;
    bmpHeader[26] = 0x01;
    bmpHeader[28] = 0x18;
    bmpHeader[35] = 0x04;
    bmpHeader[39] = 0x76;
    bmpHeader[43] = 0x76;
    
    fwrite(bmpHeader, 54, 1, pFile);
    fseek(pFile, 0x0012, SEEK_SET);

    fwrite(&width, sizeof(width), 1, pFile);
    fwrite(&height, sizeof(height), 1, pFile);

    fseek(pFile, 0, SEEK_END);
    fwrite(data, size, 1, pFile);

    fclose(pFile);
}

bool CBMPLoader::SaveBMP(const char *fileName, GLint width, GLint height, GLfloat *data)
{
    FILE *pFile;
    
    pFile = fopen(fileName, "wb");
    
    if (!pFile)
        exit(0);
    
    unsigned char bmpHeader[54] = {0};
    bmpHeader[0] = 0x42;
    bmpHeader[1] = 0x4D;
    bmpHeader[2] = 0x3A;
    bmpHeader[10] = 0x36;
    bmpHeader[14] = 0x28;
    bmpHeader[18] = 0x01;
    bmpHeader[22] = 0x01;
    bmpHeader[26] = 0x01;
    bmpHeader[28] = 0x18;
    bmpHeader[35] = 0x04;
    bmpHeader[39] = 0x76;
    bmpHeader[43] = 0x76;
    
    fwrite(bmpHeader, 54, 1, pFile);
    fseek(pFile, 0x0012, SEEK_SET);

    fwrite(&width, sizeof(width), 1, pFile);
    fwrite(&height, sizeof(height), 1, pFile);
    
    int i, j, size;
    i = width * 3;
    while (i % 4 != 0)
        ++i;
    size = i * height;
    
    GLubyte *tempData = new GLubyte[size];
    int widthBytes = i;
    GLfloat temp;
    
    for (int i = 0; i < width; i++)
    for (int j = 0; j < height; j++)
    {
        temp = data[j * width * 3 + i * 3];
        temp /= (1 + temp);
        tempData[j * widthBytes + i * 3] = (GLubyte)(temp * 255);
        
        temp = data[j * width * 3 + i * 3 + 1];
        temp /= (1 + temp);
        tempData[j * widthBytes + i * 3 + 1] = (GLubyte)(temp * 255);
        
        temp = data[j * width * 3 + i * 3 + 2];
        temp /= (1 + temp);
        tempData[j * widthBytes + i * 3 + 2] = (GLubyte)(temp * 255);
    //    printf("%f ", temp);
    }

    fseek(pFile, 0, SEEK_END);
    fwrite(tempData, size, 1, pFile);

    fclose(pFile);
}
