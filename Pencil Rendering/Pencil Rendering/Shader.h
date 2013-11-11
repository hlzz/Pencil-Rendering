#pragma once
#include <GL/glew.h>
#include <GL/gl.h>
#include <cstdio>
class CShader
{
public:
	CShader(void);
	~CShader(void);
	void setShader(const char *, const char *);
	void setUniform(const GLchar *uniformName, GLint x);
	void setUniform(const GLchar *uniformName, GLfloat x);
	void setUniform(const GLchar *uniformName, GLboolean x);
	void setUniform(const GLchar *uniformName, GLfloat, GLfloat);
	void setUniform(const GLchar *uniformName, GLfloat, GLfloat, GLfloat);
	void setUniform(const GLchar *uniformName, GLfloat, GLfloat, GLfloat, GLfloat);
	void setUniform(const GLchar *uniformName, int n, GLfloat *x);
	void setUniform(const GLchar *uniformName, int n, bool t, GLfloat *x);
	void setAttribute(const GLchar *attributeName, GLfloat x, GLfloat y, GLfloat z);
	void enable();
	void disable();
	GLuint shaderProgram;

private:
	void printShaderInfoLog(GLuint obj, const char *shader);
	void printProgramInfoLog(GLuint obj);
	char *textFileRead(const char *fn);
	GLuint vertShader, fragShader;
};

