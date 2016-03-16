/*
 * Copyright © 2016 Karol Herbst
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/** @file test.c
 *
 * From the GL_ARB_shading_language_include spec:
 *
 *     "blah blah"
 *
 *      For example
 *
 *          ...
 */

#include "piglit-util-gl.h"
#include "shading_language_include_common.h"

PIGLIT_GL_TEST_CONFIG_BEGIN
	config.supports_gl_compat_version = 20;
PIGLIT_GL_TEST_CONFIG_END

static const char *name   = "/piglit/test.shader";
static const char *source =
	"#extension GL_ARB_shading_language_include: enable\n";

void
piglit_init(int argc, char **argv)
{
	bool pass = true;
	GLuint err;

	piglit_require_extension("GL_ARB_shading_language_include");

	glNamedStringARB(SHADER_INCLUDE_ARB, strlen(name), name, strlen(source), source);
	err = glGetError();
	if (err != GL_NO_ERROR) {
		printf("glNamedStringARB failed with valid parameters\n");
		pass = false;
	}

	/* 	An INVALID_VALUE error will be generated under any of the following
		conditions:

			- Either <name> or <string> is NULL. */

	glNamedStringARB(SHADER_INCLUDE_ARB, 0, NULL, strlen(source), source);
	err = glGetError();
	if (err != GL_INVALID_VALUE) {
		printf("glNamedStringARB with name == NULL has to return GL_INVALID_VALUE\n");
		pass = false;
	}

	glNamedStringARB(SHADER_INCLUDE_ARB, strlen(name), name, 0, NULL);
	err = glGetError();
	if (err != GL_INVALID_VALUE) {
		printf("glNamedStringARB with string == NULL has to return GL_INVALID_VALUE\n");
		pass = false;
	}

	glNamedStringARB(SHADER_INCLUDE_ARB, 0, NULL, 0, NULL);
	err = glGetError();
	if (err != GL_INVALID_VALUE) {
		printf("glNamedStringARB with name == NULL and string == NULL has to return GL_INVALID_VALUE\n");
		pass = false;
	}

	/*		- <name> is not a valid pathname beginning with '/'. */
	glNamedStringARB(SHADER_INCLUDE_ARB, strlen(&name[1]), &name[1], strlen(source), source);
	err = glGetError();
	if (err != GL_INVALID_VALUE) {
		printf("glNamedStringARB with valid pathname not accepted\n");
		pass = false;
	}


	piglit_report_result(pass ? PIGLIT_PASS : PIGLIT_FAIL);
}

enum piglit_result piglit_display(void)
{
	/* UNREACHED */
	return PIGLIT_FAIL;
}
