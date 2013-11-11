#ifndef _3DS_H
#define _3DS_H

//  基本块(Primary Chunk)，位于文件的开始
#define PRIMARY       0x4D4D

//  主块(Main Chunks)
#define OBJECTINFO    0x3D3D				// 网格对象的版本号
#define VERSION       0x0002				// .3ds文件的版本
#define EDITKEYFRAME  0xB000				// 所有关键帧信息的头部

//  对象的次级定义(包括对象的材质和对象）
#define MATERIAL	  0xAFFF				// 保存纹理信息
#define OBJECT		  0x4000				// 保存对象的面、顶点等信息

//  材质的次级定义
#define MATNAME       0xA000				// 保存材质名称
#define MATDIFFUSE    0xA020				// 对象/材质的颜色
#define MATMAP        0xA200				// 新材质的头部
#define MATMAPFILE    0xA300				// 保存纹理的文件名
#define OBJECT_MESH   0x4100				// 新的网格对象

//  OBJECT_MESH的次级定义
#define OBJECT_VERTICES     0x4110			// 对象顶点
#define OBJECT_FACES		0x4120			// 对象的面
#define OBJECT_MATERIAL		0x4130			// 对象的材质
#define OBJECT_UV			0x4140			// 对象的UV纹理坐标

struct tIndices 
{							
	unsigned short a, b, c, bVisible;	
};

// 保存块信息的结构
struct tChunk
{
	unsigned short int ID;					// 块的ID		
	unsigned int length;					// 块的长度
	unsigned int bytesRead;					// 需要读的块数据的字节数
};

// CLoad3DS类处理所有的装入代码
class CLoad3DS
{
public:
	CLoad3DS();								// 初始化数据成员
	// 装入3ds文件到模型结构中
	bool Import3DS(t3DModel *pModel, char *strFileName);

private:
	// 读一个字符串
	int GetString(char *);
	// 读下一个块
	void ReadChunk(tChunk *);
	// 读下一个块
	void ProcessNextChunk(t3DModel *pModel, tChunk *);
	// 读下一个对象块
	void ProcessNextObjectChunk(t3DModel *pModel, t3DObject *pObject, tChunk *);
	// 读下一个材质块
	void ProcessNextMaterialChunk(t3DModel *pModel, tChunk *);
	// 读对象颜色的RGB值
	void ReadColorChunk(tMaterialInfo *pMaterial, tChunk *pChunk);
	// 读对象的顶点
	void ReadVertices(t3DObject *pObject, tChunk *);
	// 读对象的面信息
	void ReadVertexIndices(t3DObject *pObject, tChunk *);
	// 读对象的纹理坐标
	void ReadUVCoordinates(t3DObject *pObject, tChunk *);
	// 读赋予对象的材质名称
	void ReadObjectMaterial(t3DModel *pModel, t3DObject *pObject, tChunk *pPreviousChunk);
	// 计算对象顶点的法向量
	void ComputeNormals(t3DModel *pModel);
	// 关闭文件，释放内存空间
	void CleanUp();
	// 文件指针
	FILE *m_FilePointer;

	tChunk *m_CurrentChunk;
	tChunk *m_TempChunk;
};


#endif