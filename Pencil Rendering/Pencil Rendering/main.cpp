#include "main.h"
#include "3ds.h"
#include <cstdio>
#include "Shader.h"
#include "FBO.h"
#include "Texture.h"
#include "bmp.h"
ObjReader * myObject;
bool  g_bFullScreen = true;
HWND  g_hWnd;
RECT  g_rRect;
HDC   g_hDC;
HGLRC g_hRC;
HINSTANCE g_hInstance;
GLubyte _3dimage[32][512][512][4];						//3D纹理的图像数据域
FILE *fp;
int windowWidth = 512, windowHeight = 512;

#define FILE_NAME  "granade.obj"

UINT g_Texture[MAX_TEXTURES] = {0};

CLoad3DS g_Load3ds;
t3DModel g_3DModel;

int   g_ViewMode	  = GL_TRIANGLES;
bool  g_bLighting     = true;
float g_RotateX		  = 0.0f;
float g_RotationSpeed = 0.0001f;

CShader shaderGBuffer, shaderNPR, shaderCurvature;
CTexture textureGBuffer, texturePencil, textureCurvature;
CFBO *fboGBuffer, *fboCurvature;
float texCoordOffsets[18];

//检查是否支持Extension
bool IsExtensionSupported( char* szTargetExtension )
{
	const unsigned char *pszExtensions = NULL;
	const unsigned char *pszStart;
	unsigned char *pszWhere, *pszTerminator;

	// Extension names should not have spaces
	pszWhere = (unsigned char *) strchr( szTargetExtension, ' ' );
	if( pszWhere || *szTargetExtension == '\0' )
		return false;

	// Get Extensions String
	pszExtensions = glGetString( GL_EXTENSIONS );

	// Search The Extensions String For An Exact Copy
	pszStart = pszExtensions;
	for(;;)
	{
		pszWhere = (unsigned char *) strstr( (const char *) pszStart, szTargetExtension );
		if( !pszWhere )
			break;
		pszTerminator = pszWhere + strlen( szTargetExtension );
		if( pszWhere == pszStart || *( pszWhere - 1 ) == ' ' )
			if( *pszTerminator == ' ' || *pszTerminator == '\0' )
				return true;
		pszStart = pszTerminator;
	}
	return false;
}
GLfloat write_position[4]={ 0.0, 0.0, 1.0, 0.0 };
void Init(HWND hWnd)
{
	g_hWnd = hWnd;
	GetClientRect(g_hWnd, &g_rRect);
	InitializeOpenGL(g_rRect.right, g_rRect.bottom);
	//initialize glew
	glewInit();

	if (!IsExtensionSupported("GL_EXT_framebuffer_object"))//如果不支持framebuffer,则退出
	{
		exit(0);
	}

	/*g_Load3ds.Import3DS(&g_3DModel, FILE_NAME);			// 将3ds文件装入到模型结构体中
	// 遍历所有的材质
	for(int i = 0; i < g_3DModel.numOfMaterials; i++)
	{
		// 判断是否是一个文件名
		if(strlen(g_3DModel.pMaterials[i].strFile) > 0)
		{
			//使用纹理文件名称来装入位图
			CreateTexture(g_Texture, g_3DModel.pMaterials[i].strFile, i);			
		}

		// 设置材质的纹理ID
		g_3DModel.pMaterials[i].texureId = i;
	}*/
	//define light ambient
	float Al[4] = {0.1f, 0.1f, 0.1f, 1.0f };
	glLightfv( GL_LIGHT0, GL_AMBIENT, Al );

	float Dl[4] = {1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv( GL_LIGHT0, GL_DIFFUSE, Dl );
	
	float Sl[4] = {1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv( GL_LIGHT0, GL_SPECULAR, Sl );

	//define material
	float Am[4] = {0.3f, 0.3f, 0.3f, 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, Am );

	float Dm[4] = {0.9f, 0.5f, 0.5f, 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, Dm );

	float Sm[4] = {0.6f, 0.6f, 0.6f, 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Sm );

	float f = 60.0f;
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, f );
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = {16.0};

	//x轴负方向		//设置光源位置
	GLfloat write_light[] = { 1.0, 1.0, 1.0, 1.0 };

	GLfloat lmodel_ambient[] = { 0.1,0.1,0.1,1.0 };
	glShadeModel( GL_SMOOTH );
	glLightfv( GL_LIGHT0, GL_POSITION, write_position );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, write_light );

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);

	shaderGBuffer.setShader("gbuffer", "gbuffer");	//初始化两个过程的shader
	shaderNPR.setShader("npr", "npr");
	shaderCurvature.setShader("curvature","curvature");
	textureGBuffer.init(GL_RGBA16F, windowWidth, windowHeight);			//初始化用于fbo的texture
	textureCurvature.init(GL_RGBA16F,windowWidth,windowHeight);
	fboCurvature = new CFBO(GL_COLOR_ATTACHMENT1,GL_TEXTURE_2D,	textureCurvature.getID());
	fboCurvature->updateFBO(windowWidth, windowHeight);
	fboGBuffer = new CFBO(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureGBuffer.getID());		//初始化fbo
	fboGBuffer->updateRBO(windowWidth, windowHeight);					//初始化render buffer

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

	CBMPLoader *bmpLoader = new CBMPLoader;
	
	//将32张纹理载入，然后存入_3dimage[32][512][512][4];
	for(int i = 1; i <= 32; i++)
	{
		char filename[100];
		sprintf(filename,"texture\\texture%02d.bmp",i);
		FILE * bmpin = fopen(filename,"rb");
		fseek(bmpin,0x436L,SEEK_SET);//定位到数据区
		for(int j = 0; j < 512; j++)
			for(int k = 0; k < 512 ; k++)
			{
				fread(&_3dimage[i-1][j][k][0],1,1,bmpin);
				_3dimage[i-1][j][k][1] = _3dimage[i-1][j][k][0];			//bmp文件中只存了一位灰度
				_3dimage[i-1][j][k][2] = _3dimage[i-1][j][k][0];
				_3dimage[i-1][j][k][3] = 0;
			}
	}
	texturePencil.init(GL_RGB8, 512, 512, 32, &_3dimage[0][0][0][0]);
	myObject = new ObjReader(FILE_NAME);
	//glTexImage3D(GL_TEXTURE_3D,0,GL_RGBA,512,512,32,0,GL_RGBA,GL_UNSIGNED_BYTE,_3dimage);
}

WPARAM MainLoop()
{
	MSG msg;
	
	while(1)											
	{													
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{
			if(msg.message == WM_QUIT)					
				break;
			TranslateMessage(&msg);						
			DispatchMessage(&msg);					
		}
		else									
		{
			
			RenderScene();						
		}
	}

	/*// 遍历场景中所有的对象
	for(int i = 0; i < g_3DModel.numOfObjects; i++)
	{
		// 删除所有的变量
		delete [] g_3DModel.pObject[i].pFaces;
		delete [] g_3DModel.pObject[i].pNormals;
		delete [] g_3DModel.pObject[i].pVerts;
		delete [] g_3DModel.pObject[i].pTexVerts;
	}*/

	DeInit();
	return(msg.wParam);
}

void checkfbo(void)				//检查fbo是否能用
{
	/* lots of booring errorchecking stuff, this will display an error code and then kill the app if it failed*/
	switch(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT)) {                                          
          case GL_FRAMEBUFFER_COMPLETE_EXT: 
			  	return;
			break; 
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
				MessageBox (HWND_DESKTOP, "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT", "Error", MB_OK | MB_ICONEXCLAMATION);
				PostQuitMessage(0);					
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
				MessageBox (HWND_DESKTOP, "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT", "Error", MB_OK | MB_ICONEXCLAMATION);
				PostQuitMessage(0);					
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
				MessageBox (HWND_DESKTOP, "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT", "Error", MB_OK | MB_ICONEXCLAMATION);
				PostQuitMessage(0);					
				 break;
			case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
				MessageBox (HWND_DESKTOP, "GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT", "Error", MB_OK | MB_ICONEXCLAMATION);
				PostQuitMessage(0);					
				 break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
				MessageBox (HWND_DESKTOP, "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT", "Error", MB_OK | MB_ICONEXCLAMATION);
				PostQuitMessage(0);					
				 break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
				MessageBox (HWND_DESKTOP, "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT", "Error", MB_OK | MB_ICONEXCLAMATION);
				PostQuitMessage(0);					
				 break;
			case GL_FRAMEBUFFER_UNSUPPORTED_EXT: 
					MessageBox (HWND_DESKTOP, "GL_FRAMEBUFFER_UNSUPPORTED_EXT", "Error", MB_OK | MB_ICONEXCLAMATION);
				PostQuitMessage(0);					
				break;
			default:
				PostQuitMessage(0);					
	}
}

//draw a cube
void drawBox(void)
{ // this func just draws a perfectly normal box with some texture coordinates
	glBegin(GL_QUADS);
		// Front Face
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Top Left Of The Texture and Quad
		// Back Face
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
		// Top Face
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
		// Bottom Face
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Top Right Of The Texture and Quad
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Top Left Of The Texture and Quad
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
		// Right face
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Top Left Of The Texture and Quad
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
		// Left Face
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
	glEnd();
}
float lookatz = 20;
float lookaty = 6;
float lookatx = 0;
void RenderScene() 
{
//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();									

	gluLookAt(lookatx,lookaty, lookatz,	lookatx, lookaty, 0.0f,  0, 1,0);
	glRotatef(g_RotateX, 0, 1.0f, 0);
	g_RotateX += g_RotationSpeed;
	if (!IsExtensionSupported("GL_EXT_framebuffer_object"))
	{
		exit(0);
	}
	//第一个pass
	shaderGBuffer.enable();					//启用GBuffer并绑定fbo
	fboGBuffer->bindFBO();
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	//关闭纹理映射
	glDisable(GL_TEXTURE_2D);
	glColor3ub(255, 255, 255);

	// 开始以g_ViewMode模式绘制
	
	glBegin(g_ViewMode); //这里g_ViewMode是GL_TRIANGLES，点击之后变为连线
	// 遍历所有的面
	for(int j = 0; j < myObject->NumOfFaces; j++)
	{
		//遍历三角形的所有点
		for(int whichVertex = 0; whichVertex < 3; whichVertex++)
		{
			// 获得面对每个点的索引
			int index = myObject->FacesPoint[j][whichVertex];
			int indexfaxiang = myObject->FacesNormal[j][whichVertex];
			// 给出法向量
			glNormal3f(myObject->Normal[ indexfaxiang ][0], myObject->Normal[ indexfaxiang ][1], myObject->Normal[ indexfaxiang ][2]); 
			//设定点的坐标 
			glVertex3f(myObject->Point[ index ][0], myObject->Point[ index ][1], myObject->Point[ index ][2]);
			//在shader中给出主曲率方向
			//shaderGBuffer.setAttribute("curvatureDirection", myObject->Principle[index][0], myObject->Principle[index][1], myObject->Principle[index][2]);
		}
	}
	glEnd(); // 绘制结束
	fboGBuffer->unBindFBO();
	shaderGBuffer.disable();

	//第二个pass
	shaderCurvature.enable();					//启用GBuffer并绑定fbo
	fboCurvature->bindFBO();
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	//关闭纹理映射
	glDisable(GL_TEXTURE_2D);
	glColor3ub(255, 255, 255);

	// 开始以g_ViewMode模式绘制
	
	glBegin(g_ViewMode); //这里g_ViewMode是GL_TRIANGLES，点击之后变为连线
	// 遍历所有的面
	for(int j = 0; j < myObject->NumOfFaces; j++)
	{
		//遍历三角形的所有点
		for(int whichVertex = 0; whichVertex < 3; whichVertex++)
		{
			// 获得面对每个点的索引
			int index = myObject->FacesPoint[j][whichVertex];
			int indexfaxiang = myObject->FacesNormal[j][whichVertex];
			// 给出法向量
			glNormal3f(myObject->Normal[ indexfaxiang ][0], myObject->Normal[ indexfaxiang ][1], myObject->Normal[ indexfaxiang ][2]); 
			//设定点的坐标 
			glVertex3f(myObject->Point[ index ][0], myObject->Point[ index ][1], myObject->Point[ index ][2]);
			//在shader中给出主曲率方向
			shaderGBuffer.setAttribute("curvatureDirection", myObject->Principle[index][0], myObject->Principle[index][1], myObject->Principle[index][2]);
			shaderGBuffer.setAttribute("minCurvatureDirection", myObject->ExternPrinciple[index][0], myObject->ExternPrinciple[index][1], myObject->ExternPrinciple[index][2]);
		}
	}
	glEnd(); // 绘制结束
	fboCurvature->unBindFBO();
	shaderCurvature.disable();

	//第三个pass
	glActiveTexture(GL_TEXTURE2);							//GL_TEXTURE2位置设置成第二次render得到的主曲率方向
	glBindTexture(GL_TEXTURE_2D, textureGBuffer.getID());
	glActiveTexture(GL_TEXTURE0);							//GL_TEXTURE0位置设置成第一次render得到的texture
	glBindTexture(GL_TEXTURE_2D, textureGBuffer.getID());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, texturePencil.getID());	//GL_TEXTURE1位置设置成要贴的铅笔画
	shaderNPR.enable();
	shaderNPR.setUniform("texSrc", 0);
	shaderNPR.setUniform("offset", 9, texCoordOffsets);
	shaderNPR.setUniform("texPencil", 1);
	shaderNPR.setUniform("texCur",2);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(-1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(1.0f, 1.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(-1.0f, 1.0f);
	glEnd();
	shaderNPR.disable();
	
	SwapBuffers(g_hDC);									// 交换缓冲区
}

LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LONG    lRet = 0; 
    PAINTSTRUCT    ps;

    switch (uMsg)
	{ 
    case WM_SIZE:							
		if(!g_bFullScreen)				
		{
			SizeOpenGLScreen(LOWORD(lParam),HIWORD(lParam));
			GetClientRect(hWnd, &g_rRect);				
		}
        break; 
 
	case WM_PAINT:									
		BeginPaint(hWnd, &ps);						
		EndPaint(hWnd, &ps);					
		break;

	case WM_LBUTTONDOWN:								// 按下鼠标左键，改变绘制模式
		
		if(g_ViewMode == GL_TRIANGLES) {		
			g_ViewMode = GL_LINE_STRIP;		
		} else {
			g_ViewMode = GL_TRIANGLES;	
		}
		break;

	case WM_RBUTTONDOWN:								// 按下鼠标右键，改变光照模式
		
		g_bLighting = !g_bLighting;		

		if(g_bLighting) {					
			glEnable(GL_LIGHTING);			
		} else {
			glDisable(GL_LIGHTING);			
		}
		break;

	case WM_KEYDOWN:									// 键盘响应

		switch(wParam) {								
			case VK_ESCAPE:								// 按下ESC键
				PostQuitMessage(0);					
				break;

			case VK_LEFT:								// 按下向左键
				lookatx -= 0.3;	
				break;

			case VK_RIGHT:								// 按下向右键
				lookatx += 0.3;			
				break;
			case '1':									//zoom in
				lookatz -= 0.3;
				break;
			case '2':									//zoom out
				lookatz += 0.3;
				break;
			case VK_UP:								// 按下向上键
				lookaty += 0.3;
				break;
			case VK_DOWN:							// 按下向下键
				lookaty -= 0.3;
				break;
			case '3':
				g_RotationSpeed -= 0.1;
				break;
			case '4':
				g_RotationSpeed += 0.1;
				break;
			case 'D':
				write_position[0]+=0.5;
				glLightfv( GL_LIGHT0, GL_POSITION, write_position );
				break;
			case 'A':
				write_position[0]-=0.5;
				glLightfv( GL_LIGHT0, GL_POSITION, write_position );
				break;
			case 'W':
				write_position[1]+=0.5;
				glLightfv( GL_LIGHT0, GL_POSITION, write_position );
				break;
			case 'S':
				write_position[1]-=0.5;
				glLightfv( GL_LIGHT0, GL_POSITION, write_position );
				break;
			case 'Q':
				write_position[2]-=0.5;
				glLightfv( GL_LIGHT0, GL_POSITION, write_position );
				break;
			case 'E':
				write_position[2]+=0.5;
				glLightfv( GL_LIGHT0, GL_POSITION, write_position );
				break;

		}
		break;

	case WM_CLOSE:									
		PostQuitMessage(0);						
		break; 

	default:										
		lRet = DefWindowProc (hWnd, uMsg, wParam, lParam); 
		break; 
	} 
 
	return lRet;										
}
