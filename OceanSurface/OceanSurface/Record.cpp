#include "stdafx.h"

#include <cstdio>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Record.h"
#include "SOIL.h"

GLuint FRAMEBUFFER_WIDTH = 1920, FRAMEBUFFER_HEIGHT = 1080;

GLuint multisample_fb, multisample_color_rb, multisample_depth_rb, blit_fb, blit_color_rb, blit_depth_rb;

int frame = 0;

void InitiatlizeRecordBuffer()
{
	// Multisample buffers
	glGenFramebuffersEXT(1, &multisample_fb);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, multisample_fb);
	//Create and attach a color buffer
	glGenRenderbuffersEXT(1, &multisample_color_rb);
	//We must bind color_rb before we call glRenderbufferStorageEXT
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, multisample_color_rb);
	//The storage format is RGBA8
	glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, 16, GL_RGBA8, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	//Attach color buffer to FBO
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, multisample_color_rb);
	//-------------------------
	glGenRenderbuffersEXT(1, &multisample_depth_rb);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, multisample_depth_rb);
	glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, 16, GL_DEPTH_COMPONENT24, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	//-------------------------
	//Attach depth buffer to FBO
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, multisample_depth_rb);


	// Combined buffers
	glGenFramebuffersEXT(1, &blit_fb);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, blit_fb);
	//Create and attach a color buffer
	glGenRenderbuffersEXT(1, &blit_color_rb);
	//We must bind color_rb before we call glRenderbufferStorageEXT
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, blit_color_rb);
	//The storage format is RGBA8
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGBA8, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	//Attach color buffer to FBO
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, blit_color_rb);
	//-------------------------
	glGenRenderbuffersEXT(1, &blit_depth_rb);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, blit_depth_rb);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	//-------------------------
	//Attach depth buffer to FBO
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, blit_depth_rb);


	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, multisample_fb);
	// Define the viewport dimensions
	glViewport(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
}


int RecordFrame()
{
	// Disable view angle switch

	char fileName[128];
	frame++;
	sprintf(fileName, ".\\pic\\%d.bmp", frame);
	printf("%s\n", fileName);

	glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, multisample_fb);
	glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, blit_fb);
	glBlitFramebufferEXT(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, 0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	//--------------------
	//Bind the standard FBO for reading
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, blit_fb);
	SOIL_save_screenshot(fileName, SOIL_SAVE_TYPE_BMP, 0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, multisample_fb);

	if (frame == 450) return RECORD_FINISHED;
	return RECORD_CONTINUE;	
}

void CleanUpRecord()
{
	//Delete resources
	glDeleteRenderbuffersEXT(1, &multisample_color_rb);
	glDeleteRenderbuffersEXT(1, &multisample_depth_rb);
	//Bind 0, which means render to back buffer, as a result, fb is unbound
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glDeleteFramebuffersEXT(1, &multisample_fb);

	//Delete resources
	glDeleteRenderbuffersEXT(1, &blit_color_rb);
	glDeleteRenderbuffersEXT(1, &blit_depth_rb);
	//Bind 0, which means render to back buffer, as a result, fb is unbound
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glDeleteFramebuffersEXT(1, &blit_fb);
}