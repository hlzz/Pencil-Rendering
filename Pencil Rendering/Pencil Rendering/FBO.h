#pragma once
#include <GL/glew.h>
#include <GL/gl.h>

class CFBO
{
public:
	CFBO(GLenum, GLenum, GLuint);
	~CFBO(void);
    void updateFBO(GLuint textureID);
    void updateFBO(GLenum, GLuint);
    void updateRBO(GLint, GLint);
    void bindFBO();
    void unBindFBO();
    void bindRBO();
    void unBindRBO();
	bool inline isActive()
	{
		return active;
	}

private:
    bool activeRBO;
	bool active;
    GLuint frameBufferID;
    GLuint renderBufferID;
    GLenum attachment;
    GLenum kind;
};

