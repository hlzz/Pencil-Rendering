#include "Shader.h"

extern FILE *fp;

CShader::CShader(void)
{
}

CShader::~CShader(void)
{
}

void CShader::setShader(const char *vertexFile, const char *fragmentFile)
{
	char *vs, *fs;

	vertShader = glCreateShader(GL_VERTEX_SHADER);
	fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	char tempVS[100] = {0}, tempFS[100] = {0};

	sprintf(tempVS, "shaders//%s.vert", vertexFile);
	sprintf(tempFS, "shaders//%s.frag", fragmentFile);

	vs = textFileRead(tempVS);
	fs = textFileRead(tempFS);

	const char *vv = vs;
	const char *ff = fs;

	glShaderSource(vertShader, 1, &vv, NULL);
	glShaderSource(fragShader, 1, &ff, NULL);

	delete vs;
	delete fs;

	glCompileShader(vertShader);
	glCompileShader(fragShader);

	printShaderInfoLog(vertShader, vertexFile);
	printShaderInfoLog(fragShader, fragmentFile);

	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertShader);
	glAttachShader(shaderProgram, fragShader);

	glLinkProgram(shaderProgram);
	printProgramInfoLog(shaderProgram);

}

void CShader::printProgramInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten = 0;
    char *infoLog;

    glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
    
    if (infologLength > 0)
    {
        infoLog = new char[infologLength];
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
        fprintf(fp, "program:\n%s\n", infoLog);
        delete infoLog;
    }
}

void CShader::printShaderInfoLog(GLuint obj, const char *shader)
{
    int infologLength = 0;
    int charsWritten = 0;
    char *infoLog;

    glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

    if (infologLength > 0)
    {
        infoLog = new char[infologLength];
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
        fprintf(fp, "%s:\n%s\n", shader, infoLog);
        delete infoLog;
    }
}

char *CShader::textFileRead(const char *fn)
{
    FILE *fp;
    char *content = NULL;

    int count = 0;

    if (fn != NULL)
    {
        fp = fopen(fn, "rt"); 
	if (fp != NULL)
        {
            fseek(fp, 0, SEEK_END);
            count = ftell(fp);
            rewind(fp);

            if (count > 0)
            {
                content = new char[count + 1];
                count = fread(content, sizeof(char), count, fp);
                content[count] = '\0';
            }
            fclose(fp);
        }
    }
    return content;
}

void CShader::setUniform(const GLchar *uniformName, GLint x)							  
{
    GLint location = glGetUniformLocation(shaderProgram, uniformName);
    glUniform1i(location, x);
}

void CShader::setUniform(const GLchar *uniformName, GLfloat x)							  
{
    GLint location = glGetUniformLocation(shaderProgram, uniformName);
    glUniform1f(location, x);
}

void CShader::setUniform(const GLchar *uniformName, GLboolean x)							  
{
    GLint location = glGetUniformLocation(shaderProgram, uniformName);
    glUniform1i(location, x);
}	

void CShader::setUniform(const GLchar *uniformName, GLfloat a, GLfloat b)							  
{
    GLint location = glGetUniformLocation(shaderProgram, uniformName);
    glUniform2f(location, a, b);
}

void CShader::setUniform(const GLchar *uniformName, GLfloat a, GLfloat b, GLfloat c)							  
{
    GLint location = glGetUniformLocation(shaderProgram, uniformName);
    glUniform3f(location, a, b, c);
}

void CShader::setUniform(const GLchar *uniformName, GLfloat a, GLfloat b, GLfloat c, GLfloat d)							  
{
    GLint location = glGetUniformLocation(shaderProgram, uniformName);
    glUniform4f(location, a, b, c, d);
}

void CShader::setUniform(const GLchar *uniformName, int n, GLfloat *x)
{
    GLint location = glGetUniformLocation(shaderProgram, uniformName);
    glUniform2fv(location, n, x);
}

void CShader::setUniform(const GLchar *uniformName, int n, bool t, GLfloat *x)
{
    GLint location = glGetUniformLocation(shaderProgram, uniformName);
    glUniformMatrix4fv(location, n, t, x);
}

void CShader::setAttribute(const GLchar *attributeName, GLfloat x, GLfloat y, GLfloat z)
{
	GLint location = glGetAttribLocation(shaderProgram, attributeName);
	glVertexAttrib3f(location,x,y,z);
}
void CShader::enable()
{
	glUseProgram(shaderProgram); 
}

void CShader::disable()
{
    glUseProgram(0); 
}