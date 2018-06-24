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
	config.supports_gl_compat_version = 11;
	config.window_width = 600;
	config.window_height = 200;
	config.window_visual = PIGLIT_GL_VISUAL_RGBA;
PIGLIT_GL_TEST_CONFIG_END

enum piglit_result
piglit_display(void)
{
	enum piglit_result result = PIGLIT_PASS;
	static GLuint texture_id;
	uint32_t pixels_in[7*3*4] = {
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x24924940, 0x1b6db700, 0x124924a0, 0x09249250,
		0x49249280, 0x36db6e00, 0x24924940, 0x124924a0,
		0x6db6db80, 0x52492480, 0x36db6dc0, 0x1b6db6e0,
		0x92492500, 0x6db6dc00, 0x49249280, 0x24924940,
		0xb6db6e00, 0x89249200, 0x5b6db700, 0x2db6db80,
		0xdb6db700, 0xa4924900, 0x6db6db80, 0x36db6dc0,

		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x24924940, 0x1b6db700, 0x124924a0, 0x09249250,
		0x49249280, 0x36db6e00, 0x24924940, 0x124924a0,
		0x6db6db80, 0x52492480, 0x36db6dc0, 0x1b6db6e0,
		0x92492500, 0x6db6dc00, 0x49249280, 0x24924940,
		0xb6db6e00, 0x89249200, 0x5b6db700, 0x2db6db80,
		0xdb6db700, 0xa4924900, 0x6db6db80, 0x36db6dc0,

		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x24924940, 0x1b6db700, 0x124924a0, 0x09249250,
		0x49249280, 0x36db6e00, 0x24924940, 0x124924a0,
		0x6db6db80, 0x52492480, 0x36db6dc0, 0x1b6db6e0,
		0x92492500, 0x6db6dc00, 0x49249280, 0x24924940,
		0xb6db6e00, 0x89249200, 0x5b6db700, 0x2db6db80,
		0xdb6db700, 0xa4924900, 0x6db6db80, 0x36db6dc0,
	};

	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32UI, 7, 3, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, pixels_in);

	int8_t pixels_out[7*3] = { };

	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_BYTE, pixels_out);

	for (unsigned i = 0; i < sizeof(pixels_out) / sizeof(*pixels_out); ++i) {
		uint32_t in = pixels_in[i*4];
		in = MIN2(0x7f, in);

		if (in != pixels_out[i]) {
			printf("(%i,%i: original: 0x%x) 0x%x != 0x%x\n", i / 7, i % 7, pixels_in[i*4], in, pixels_out[i]);
			result = PIGLIT_FAIL;
		}
	}

	piglit_present_results();

	return result;
}

void
piglit_init(int argc, char **argv)
{
	if (piglit_get_gl_version() < 11) {
		printf("Requires GL 1.1");
		piglit_report_result(PIGLIT_SKIP);
	}

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}
