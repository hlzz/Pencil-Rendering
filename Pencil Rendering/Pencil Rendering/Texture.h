#pragma once
#include <GL/glew.h>
#include <GL/gl.h>
class CTexture
{
public:
	CTexture(void);
	~CTexture(void);
    void init(GLenum tempFormat, GLint width, GLubyte *data);
    void init(GLenum tempFormat, GLint width, GLint height);
    void init(GLenum tempFormat, GLint width, GLint height, GLubyte *data);
    void init(GLenum tempFormat, GLint width, GLint height, GLfloat *data);
    void init(GLenum tempFormat, GLint width, GLint height, GLint depth, unsigned char *data);
    void update(GLint width, GLint height);
    GLuint inline getID()
    {
        return textureID;
    }
    bool inline isActive()
    {
        return active;
    }

private:
    GLuint textureID;
    bool active;
//    CBMPLoader *bmpLoader;
//    CHDRLoader *hdrLoader;
    GLenum format;
};

