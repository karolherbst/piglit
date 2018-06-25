/*
 * Copyright (c) 2011 VMware, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * on the rights to use, copy, modify, merge, publish, distribute, sub
 * license, and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT.  IN NO EVENT SHALL VMWARE AND/OR THEIR SUPPLIERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


/**
 * @file getteximage-formats.c
 *
 * Test glGetTexImage with a variety of formats.
 * Brian Paul
 * Sep 2011
 */


#include "piglit-util-gl.h"

PIGLIT_GL_TEST_CONFIG_BEGIN
	config.supports_gl_core_version = 32;
	config.window_width = 600;
	config.window_height = 200;
	config.window_visual = PIGLIT_GL_VISUAL_RGBA;
PIGLIT_GL_TEST_CONFIG_END

enum DrawMode
{
	DEFAULT,
	DEPTH_SPAN,
};

#define TOLERANCE_LOW 0.48
#define TOLERANCE_HIGH 0.52
#define TEX_SIZE 256

static void
setDrawReadBuffer(GLenum draw, GLenum read, GLint *drawBuffer, GLint *readBuffer)
{
	glGetIntegerv(GL_DRAW_BUFFER0, drawBuffer);
	glGetIntegerv(GL_READ_BUFFER, readBuffer);
	glDrawBuffers(1, &draw);
	glReadBuffer(read);
}

static void
restoreDrawReadBuffer(GLint drawBuffer, GLint readBuffer)
{
	glDrawBuffers(1, (GLenum *)&drawBuffer);
	glReadBuffer(readBuffer);
}

static void
drawQuad(enum DrawMode drawMode, GLuint program)
{
	static const GLfloat verticesDefault[] = {
		-1.0f, -1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
	};

	static const GLfloat verticesDepthSpan1[] = {
		-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	};

	static const uint16_t quadIndices[] = { 0, 1, 2, 2, 1, 3 };

	GLuint array;
	GLuint buffer[2];

	glGenVertexArrays(1, &array);
	glBindVertexArray(array);
	GLint pos = glGetAttribLocation(program, "pos");

	glGenBuffers(1, &buffer[0]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);

	if (drawMode == DEFAULT)
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticesDefault), &verticesDefault, GL_STATIC_DRAW);
	else
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticesDepthSpan1), &verticesDepthSpan1, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(pos, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &buffer[1]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 12, &quadIndices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[1]);

	glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_SHORT, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDeleteBuffers(2, buffer);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &array);
}

static const GLchar *vertShader = "#version 150\n"
	"precision highp float;\n"
	"in vec4 pos;\n"
	"void main() {\n"
	"	gl_Position = pos;\n"
	"}\n";

static const GLchar *fragShader = "#version 150\n"
	"precision highp float;\n"
	"out vec4 color;\n"
	"uniform vec4 uColor;\n"
	"void main() {\n"
	"	color = uColor;\n"
	"}\n";

enum piglit_result
piglit_display(void)
{
	// setupColorProgram()

	enum piglit_result result = PIGLIT_PASS;
	GLint sourceSizes[] = { strlen(vertShader), strlen(fragShader) };

	GLuint shader1 = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shader1, 1, &vertShader, &sourceSizes[0]);
	glCompileShader(shader1);

	GLuint shader2 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(shader2, 1, &fragShader, &sourceSizes[1]);
	glCompileShader(shader2);

	GLuint colorProgram = glCreateProgram();

	glAttachShader(colorProgram, shader1);
	glAttachShader(colorProgram, shader2);
	glBindAttribLocation(colorProgram, 0, "pos");
	glLinkProgram(colorProgram);

	glDeleteShader(shader1);
	glDeleteShader(shader2);
	glUseProgram(colorProgram);
	glUniform4f(glGetUniformLocation(colorProgram, "uColor"), 1, 1, 1, 1);

	GLuint fbo[3]; // source, dest, downsample
	GLuint rbo[4]; // source D/S, dest D/S, dest color, downsample color
	GLuint data[TEX_SIZE * TEX_SIZE] = {};

	// Create FBO/RBO
	glGenFramebuffers(3, fbo);
	glGenRenderbuffers(4, rbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo[0]);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo[0]);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 0, GL_DEPTH32F_STENCIL8, TEX_SIZE, TEX_SIZE);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo[0]);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo[1]);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo[1]);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 3, GL_DEPTH32F_STENCIL8, TEX_SIZE, TEX_SIZE);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo[1]);

	glBindRenderbuffer(GL_RENDERBUFFER, rbo[2]);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 3, GL_RGBA8, TEX_SIZE, TEX_SIZE);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo[2]);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo[2]);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo[3]);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 0, GL_RGBA8, TEX_SIZE, TEX_SIZE);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo[3]);

	GLint drawBuffer;
	GLint readBuffer;

	// render
	glBindFramebuffer(GL_FRAMEBUFFER, fbo[0]);
	setDrawReadBuffer(GL_NONE, GL_NONE, &drawBuffer, &readBuffer);

	glViewport(0, 0, TEX_SIZE, TEX_SIZE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 0x1, 0xff);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	glClearStencil(0);
	glClearColor(0.8f, 0.8f, 0.8f, 0.8f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	drawQuad(DEPTH_SPAN, colorProgram);
	restoreDrawReadBuffer(drawBuffer, readBuffer);

	// blit
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo[0]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo[1]);
	setDrawReadBuffer(GL_NONE, GL_NONE, &drawBuffer, &readBuffer);
	glBlitFramebuffer(0, 0, TEX_SIZE, TEX_SIZE, 0, 0, TEX_SIZE, TEX_SIZE, GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
	restoreDrawReadBuffer(drawBuffer, readBuffer);

	// verify
	glBindFramebuffer(GL_FRAMEBUFFER, fbo[1]);
	setDrawReadBuffer(GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT0, &drawBuffer, &readBuffer);
	glStencilFunc(GL_EQUAL, 0x1, 0xff);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glClear(GL_COLOR_BUFFER_BIT);
	drawQuad(DEFAULT, colorProgram);
	restoreDrawReadBuffer(drawBuffer, readBuffer);

	// Downsample blit
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo[1]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo[2]);
	setDrawReadBuffer(GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT0, &drawBuffer, &readBuffer);
	glBlitFramebuffer(0, 0, TEX_SIZE, TEX_SIZE, 0, 0, TEX_SIZE, TEX_SIZE, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	restoreDrawReadBuffer(drawBuffer, readBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo[2]);
	glReadPixels(0, 0, TEX_SIZE, TEX_SIZE, GL_RGBA, GL_UNSIGNED_BYTE, data);

	int count = 0;
	for (unsigned j = 0; j < TEX_SIZE; j++) {
		for (unsigned i = 0; i < TEX_SIZE; i++) {
			GLuint color = ((GLuint*)data)[i + j * TEX_SIZE];

			GLuint colorref;
			if (j > TEX_SIZE * TOLERANCE_HIGH)
				colorref = 0xffffffff;
			else if (j < TEX_SIZE * TOLERANCE_LOW)
				colorref = 0xcccccccc;
			else
				continue;

			if (color != colorref) {
				printf("(%u %u was 0x%x should be 0x%x\n", i, j, color, colorref);
				result = false;
				count++;
			}
		}
	}

	if (count)
		result = PIGLIT_FAIL;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glDeleteRenderbuffers(4, rbo);
	glDeleteFramebuffers(3, fbo);

	return result;
}

void
piglit_init(int argc, char **argv)
{
	if (piglit_get_gl_version() < 32) {
		printf("Requires GL 3.2");
		piglit_report_result(PIGLIT_SKIP);
	}

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}
