#include "main.h"
#include <cstdio>
#include "Texture.h"
#include "FBO.h"


extern FILE *fp;
extern int windowWidth, windowHeight;
extern CTexture textureGBuffer;
extern CFBO *fboGBuffer;
extern float texCoordOffsets[18];
//  从文件中创建纹理
void CreateTexture(UINT textureArray[], LPSTR strFileName, int textureID)
{
	AUX_RGBImageRec *pBitmap = NULL;
	
	if(!strFileName)									// 如果无此文件，则直接返回
		return;

	pBitmap = auxDIBImageLoad(strFileName);				// 装入位图，并保存数据
	
	if(pBitmap == NULL)									// 如果装入位图失败，则退出
		exit(0);

	// 生成纹理
	glGenTextures(1, &textureArray[textureID]);

	// 设置像素对齐格式
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

	glBindTexture(GL_TEXTURE_2D, textureArray[textureID]);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pBitmap->sizeX, pBitmap->sizeY, GL_RGB, GL_UNSIGNED_BYTE, pBitmap->data);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);

	if (pBitmap)										// 释放位图占用的资源
	{
		if (pBitmap->data)						
		{
			free(pBitmap->data);				
		}

		free(pBitmap);					
	}
}

void ChangeToFullScreen()
{
	DEVMODE dmSettings;								

	memset(&dmSettings,0,sizeof(dmSettings));		

	if(!EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&dmSettings))
	{
		MessageBox(NULL, "Could Not Enum Display Settings", "Error", MB_OK);
		return;
	}

	dmSettings.dmPelsWidth	= SCREEN_WIDTH;		
	dmSettings.dmPelsHeight	= SCREEN_HEIGHT;	
	
	int result = ChangeDisplaySettings(&dmSettings,CDS_FULLSCREEN);	

	if(result != DISP_CHANGE_SUCCESSFUL)
	{

		MessageBox(NULL, "Display Mode Not Compatible", "Error", MB_OK);
		PostQuitMessage(0);
	}
}

HWND CreateMyWindow(LPSTR strWindowName, int width, int height, DWORD dwStyle, bool bFullScreen, HINSTANCE hInstance)
{
	HWND hWnd;
	WNDCLASS wndclass;
	
	memset(&wndclass, 0, sizeof(WNDCLASS));			
	wndclass.style = CS_HREDRAW | CS_VREDRAW;		
	wndclass.lpfnWndProc = WinProc;				
	wndclass.hInstance = hInstance;				
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);	
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);	
	wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);
	wndclass.lpszClassName = "GameTutorials";		

	RegisterClass(&wndclass);						
	
	if(bFullScreen && !dwStyle) 					
	{												
		dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		ChangeToFullScreen();					
		ShowCursor(FALSE);						
	}
	else if(!dwStyle)							
		dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	
	g_hInstance = hInstance;						

	RECT rWindow;
	rWindow.left	= 0;				
	rWindow.right	= width;			
	rWindow.top	    = 0;					
	rWindow.bottom	= height;					
	AdjustWindowRect( &rWindow, dwStyle, false);	

												
	hWnd = CreateWindow("GameTutorials", strWindowName, dwStyle, 0, 0,
						rWindow.right  - rWindow.left, rWindow.bottom - rWindow.top, 
						NULL, NULL, hInstance, NULL);

	if(!hWnd) return NULL;					

	ShowWindow(hWnd, SW_SHOWNORMAL);	
	UpdateWindow(hWnd);					

	SetFocus(hWnd);					

	return hWnd;
}

bool bSetupPixelFormat(HDC hdc) 
{ 
    PIXELFORMATDESCRIPTOR pfd; 
    int pixelformat; 
 
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);			
    pfd.nVersion = 1;								
													
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; 
    pfd.dwLayerMask = PFD_MAIN_PLANE;			
    pfd.iPixelType = PFD_TYPE_RGBA;				
    pfd.cColorBits = SCREEN_DEPTH;				
    pfd.cDepthBits = SCREEN_DEPTH;				
    pfd.cAccumBits = 0;						
    pfd.cStencilBits = 0;				
 
    if ( (pixelformat = ChoosePixelFormat(hdc, &pfd)) == FALSE ) 
    { 
        MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK); 
        return FALSE; 
    } 
 
    if (SetPixelFormat(hdc, pixelformat, &pfd) == FALSE) 
    { 
        MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK); 
        return FALSE; 
    } 
 
    return TRUE;							
}

void SizeOpenGLScreen(int width, int height)		
{
	if (height==0)										
	{
		height=1;										
	}

	glViewport(0,0,width,height);						

	glMatrixMode(GL_PROJECTION);		
	glLoadIdentity();						
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height, .5f ,150.0f);

	glMatrixMode(GL_MODELVIEW);						
	glLoadIdentity();

	windowWidth = width;
	windowHeight = height;

	if (textureGBuffer.isActive())
	{
		textureGBuffer.update(width, height);
		fboGBuffer->updateFBO(GL_COLOR_ATTACHMENT0, textureGBuffer.getID());
		fboGBuffer->updateRBO(width, height);
	}

    GLfloat dx = 0.3f / (GLfloat)windowWidth;
    GLfloat dy = 0.3f / (GLfloat)windowHeight;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            texCoordOffsets[(((i*3)+j)*2)+0] = (-1.0f * dx) + ((GLfloat)i * dx);
            texCoordOffsets[(((i*3)+j)*2)+1] = (-1.0f * dy) + ((GLfloat)j * dy);
        }
    }
}


void InitializeOpenGL(int width, int height) 
{
	g_hDC = GetDC(g_hWnd);							
											
	if (!bSetupPixelFormat(g_hDC))			
		PostQuitMessage (0);					

	g_hRC = wglCreateContext(g_hDC);			
	wglMakeCurrent(g_hDC, g_hRC);			

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_3D);
	glEnable(GL_DEPTH_TEST);

	glClearColor(1.0, 1.0, 1.0, 1.0);

	SizeOpenGLScreen(width, height);	
}

void DeInit()
{
	if (g_hRC)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(g_hRC);
	}
	
	if (g_hDC) 
		ReleaseDC(g_hWnd, g_hDC);
		
	if(g_bFullScreen)
	{
		ChangeDisplaySettings(NULL,0);
		ShowCursor(TRUE);
	}

	UnregisterClass("GameTutorials", g_hInstance);	

	PostQuitMessage (0);							
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{	
	HWND hWnd;
	fp = fopen("test.txt", "w");
	// 判断用户是否需要全屏显示
	//if(MessageBox(NULL, "Click Yes to go to full screen (Recommended)", "Options", MB_YESNO | MB_ICONQUESTION) == IDNO)
		g_bFullScreen = false;
	
	hWnd = CreateMyWindow(" Pencil Rendering", SCREEN_WIDTH, SCREEN_HEIGHT, 0, g_bFullScreen, hInstance);

	if(hWnd == NULL) return true;

	// 初始化OpenGL
	Init(hWnd);
	return MainLoop();
}