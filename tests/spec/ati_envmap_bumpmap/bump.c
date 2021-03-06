/*
 * Copyright © 2010 Intel Corporation
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
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

/** @file bump.c
 *
 * Test of ATI_envmap_bumpmap texture combiners.
 */

#include "piglit-util-gl.h"

#define TEXSIZE 32

PIGLIT_GL_TEST_CONFIG_BEGIN

	config.supports_gl_compat_version = 10;

	config.window_width = TEXSIZE*2;
	config.window_height = TEXSIZE*2;
	config.window_visual = PIGLIT_GL_VISUAL_RGBA | PIGLIT_GL_VISUAL_DOUBLE;
	config.khr_no_error_support = PIGLIT_NO_ERRORS;

PIGLIT_GL_TEST_CONFIG_END

static GLenum tex_unit, bump_unit;


GLvoid
draw_rect_multitex(float x, float y, float w, float h,
		   float tx, float ty, float tw, float th)
{
	float verts[4][4];
	float tex[4][2];

	verts[0][0] = x;
	verts[0][1] = y;
	verts[0][2] = 0.0;
	verts[0][3] = 1.0;
	tex[0][0] = tx;
	tex[0][1] = ty;
	verts[1][0] = x + w;
	verts[1][1] = y;
	verts[1][2] = 0.0;
	verts[1][3] = 1.0;
	tex[1][0] = tx + tw;
	tex[1][1] = ty;
	verts[2][0] = x + w;
	verts[2][1] = y + h;
	verts[2][2] = 0.0;
	verts[2][3] = 1.0;
	tex[2][0] = tx + tw;
	tex[2][1] = ty + th;
	verts[3][0] = x;
	verts[3][1] = y + h;
	verts[3][2] = 0.0;
	verts[3][3] = 1.0;
	tex[3][0] = tx;
	tex[3][1] = ty + th;

	glVertexPointer(4, GL_FLOAT, 0, verts);
	glEnableClientState(GL_VERTEX_ARRAY);

	glClientActiveTexture(GL_TEXTURE0);
	glTexCoordPointer(2, GL_FLOAT, 0, tex);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glClientActiveTexture(GL_TEXTURE1);
	glTexCoordPointer(2, GL_FLOAT, 0, tex);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glDrawArrays(GL_QUADS, 0, 4);

	glDisableClientState(GL_VERTEX_ARRAY);

	glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

enum piglit_result
piglit_display(void)
{
	GLboolean pass = GL_TRUE;
	GLuint tex, bump;
	int x, y;
	float bump_matrix[4] = {1.0, 0.0, 0.0, 1.0};
	float red[4]   = {1.0, 0.0, 0.0, 1.0};
	float green[4] = {0.0, 1.0, 0.0, 1.0};
	float blue[4]  = {0.0, 0.0, 1.0, 1.0};
	float white[4] = {1.0, 1.0, 1.0, 1.0};
	float bumpdata[TEXSIZE][TEXSIZE][2];

	/* First: the base texture. */
	tex = piglit_rgbw_texture(GL_RGBA, TEXSIZE, TEXSIZE,
				  GL_FALSE, GL_FALSE, GL_UNSIGNED_NORMALIZED);
	glActiveTexture(tex_unit);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
	glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);

	/* Second: the bumpmap. */
	glGenTextures(1, &bump);
	glActiveTexture(bump_unit);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, bump);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_BUMP_ENVMAP_ATI);
	glTexEnvf(GL_TEXTURE_ENV, GL_BUMP_TARGET_ATI, tex_unit);

	/* The bump map we use is going to end up rotating texels CCW. */
	for (y = 0; y < TEXSIZE; y++) {
		for (x = 0; x < TEXSIZE; x++) {
			if (y < TEXSIZE / 2) {
				if (x < TEXSIZE / 2) {
					bumpdata[y][x][0] = 0.0;
					bumpdata[y][x][1] = 0.5;
				} else {
					bumpdata[y][x][0] = -0.5;
					bumpdata[y][x][1] = 0.0;
				}
			} else {
				if (x < TEXSIZE / 2) {
					bumpdata[y][x][0] = 0.5;
					bumpdata[y][x][1] = 0.0;
				} else {
					bumpdata[y][x][0] = 0.0;
					bumpdata[y][x][1] = -0.5;
				}
			}
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DU8DV8_ATI, TEXSIZE, TEXSIZE, 0,
		     GL_DUDV_ATI, GL_FLOAT, bumpdata);

	glTexBumpParameterfvATI(GL_BUMP_ROT_MATRIX_ATI, bump_matrix);

	draw_rect_multitex(-1, -1, 2, 2,
			   0, 0, 1, 1);

	pass = pass && piglit_probe_rect_rgba(0,
					      0,
					      piglit_width / 2,
					      piglit_height / 2,
					      blue);
	pass = pass && piglit_probe_rect_rgba(piglit_width / 2,
					      0,
					      piglit_width / 2,
					      piglit_height / 2,
					      red);
	pass = pass && piglit_probe_rect_rgba(0,
					      piglit_height / 2,
					      piglit_width / 2,
					      piglit_height / 2,
					      white);
	pass = pass && piglit_probe_rect_rgba(piglit_width / 2,
					      piglit_height / 2,
					      piglit_width / 2,
					      piglit_height / 2,
					      green);

	piglit_present_results();

	return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}

void
piglit_init(int argc, char **argv)
{
	int size, num_units, *units;
	int i;
	GLboolean found_unit_1;

	piglit_require_extension("GL_ATI_envmap_bumpmap");

	glGetTexBumpParameterivATI(GL_BUMP_ROT_MATRIX_SIZE_ATI, &size);
	if (size < 4) {
		printf("GL_BUMP_ROT_MATRIX_SIZE_ATI %d < 4\n", size);
		piglit_report_result(PIGLIT_FAIL);
	}
	if (size != 4) {
		printf("What does GL_BUMP_ROT_MATRIX_SIZE_ATI = %d even mean?\n",
		       size);
		piglit_report_result(PIGLIT_SKIP);
	}

	glGetTexBumpParameterivATI(GL_BUMP_NUM_TEX_UNITS_ATI, &num_units);
	if (num_units < 1) {
		printf("GL_BUMP_NUM_TEX_UNITS_ATI %d < 4\n", num_units);
		piglit_report_result(PIGLIT_FAIL);
	}

	units = malloc(num_units * sizeof(int));
	glGetTexBumpParameterivATI(GL_BUMP_TEX_UNITS_ATI, units);
	found_unit_1 = GL_FALSE;
	for (i = 0; i < num_units; i++) {
		if (units[i] < GL_TEXTURE0) {
			printf("Bad unit in GL_BUMP_TEX_UNITS_ATI: 0x%x\n",
			       units[i]);
			piglit_report_result(PIGLIT_FAIL);
		}
		if (units[i] == GL_TEXTURE1)
			found_unit_1 = GL_TRUE;
	}
	free(units);

	if (!found_unit_1) {
		printf("Implementation doesn't support bumpmapping unit 1.\n");
		piglit_report_result(PIGLIT_SKIP);
	}

	tex_unit = GL_TEXTURE0;
	bump_unit = GL_TEXTURE1;
}
