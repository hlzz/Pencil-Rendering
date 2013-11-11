#ifndef _MAIN_H
#define _MAIN_H

#include <windows.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
//#include <glfw.h>
#include <GL/gl.h>
#include <fstream>
#include <vector>
#include <GL/glaux.h>
#include <crtdbg.h>
#include <string.h>
#include "ObjReader.h"

#pragma comment(lib, "glew32.lib")

using namespace std;

#define SCREEN_WIDTH 512								
#define SCREEN_HEIGHT 512								
#define SCREEN_DEPTH 16									

#define MAX_TEXTURES 100								// 最大的纹理数目

// 定义3D点的类，用于保存模型中的顶点
class CVector3 
{
public:
	float x, y, z;
};

// 定义2D点类，用于保存模型的UV纹理坐标
class CVector2 
{
public:
	float x, y;
};

//  面的结构定义
struct tFace
{
	int vertIndex[3];			// 顶点索引
	int coordIndex[3];			// 纹理坐标索引
};

//  材质信息结构体
struct tMaterialInfo
{
	char  strName[255];			// 纹理名称
	char  strFile[255];			// 如果存在纹理映射，则表示纹理文件名称
	BYTE  color[3];				// 对象的RGB颜色
	int   texureId;				// 纹理ID
	float uTile;				// u 重复
	float vTile;				// v 重复
	float uOffset;			    // u 纹理偏移
	float vOffset;				// v 纹理偏移
} ;

//  对象信息结构体
struct t3DObject 
{
	int  numOfVerts;			// 模型中顶点的数目
	int  numOfFaces;			// 模型中面的数目
	int  numTexVertex;			// 模型中纹理坐标的数目
	int  materialID;			// 纹理ID
	bool bHasTexture;			// 是否具有纹理映射
	char strName[255];			// 对象的名称
	CVector3  *pVerts;			// 对象的顶点
	CVector3  *pNormals;		// 对象的法向量
	CVector2  *pTexVerts;		// 纹理UV坐标
	tFace *pFaces;				// 对象的面信息
};

//  模型信息结构体
struct t3DModel 
{
	int numOfObjects;					// 模型中对象的数目
	int numOfMaterials;					// 模型中材质的数目
	vector<tMaterialInfo> pMaterials;	// 材质链表信息
	vector<t3DObject> pObject;			// 模型中对象链表信息
};


extern bool  g_bFullScreen;									
extern HWND  g_hWnd;										
extern RECT  g_rRect;										
extern HDC   g_hDC;											
extern HGLRC g_hRC;											
extern HINSTANCE g_hInstance;								

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow);
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
WPARAM MainLoop();

// 生成纹理，并将其保存在纹理队列中
void CreateTexture(UINT textureArray[], LPSTR strFileName, int textureID);

// 改变显示为全屏模式
void ChangeToFullScreen();

// 生成用户窗口
HWND CreateMyWindow(LPSTR strWindowName, int width, int height, DWORD dwStyle, bool bFullScreen, HINSTANCE hInstance);

// 设置像素格式
bool bSetupPixelFormat(HDC hdc);

// 初始化投影变换
void SizeOpenGLScreen(int width, int height);

// 初始化OpenGL
void InitializeOpenGL(int width, int height);

// 初始化整个程序
void Init(HWND hWnd);

// 渲染场景
void RenderScene();

// 释放程序占用的内存空间
void DeInit();

#endif