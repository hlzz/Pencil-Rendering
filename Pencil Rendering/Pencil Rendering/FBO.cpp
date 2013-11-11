#include "FBO.h"
#include <cstdio>

extern FILE *fp;

CFBO::~CFBO(void)
{
}

CFBO::CFBO(GLenum tempAttachment, GLenum tempKind, GLuint textureID)
{

    activeRBO = false;
    attachment = tempAttachment;
    kind = tempKind;
    
    glGenFramebuffers(1, &frameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);

    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, kind, textureID, 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch (status)
    {
        case GL_FRAMEBUFFER_COMPLETE:
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            fprintf(fp, "unsupport FBO\n");
            break;
        default:
            fprintf(fp, "support FBO\n");
            break;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CFBO::updateFBO(GLenum tempAttachment, GLuint textureID)
{

    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, tempAttachment, kind, textureID, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CFBO::updateFBO(GLuint textureID)
{

    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, kind, textureID, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CFBO::updateRBO(GLint width, GLint height)
{
    glGenRenderbuffers(1, &renderBufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    activeRBO = true;
}

void CFBO::bindFBO()
{
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
}

void CFBO::unBindFBO()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CFBO::bindRBO()
{
    glBindRenderbuffer(GL_RENDERBUFFER, renderBufferID);
}

void CFBO::unBindRBO()
{
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}