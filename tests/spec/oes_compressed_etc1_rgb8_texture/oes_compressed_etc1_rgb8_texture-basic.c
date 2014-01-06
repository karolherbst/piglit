/*
 * Copyright © 2011 LunarG, Inc.
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

#include "piglit-util-gl-common.h"

#ifndef GL_OES_compressed_ETC1_RGB8_texture
#define GL_ETC1_RGB8_OES                                        0x8D64
#endif

PIGLIT_GL_TEST_CONFIG_BEGIN

	config.supports_gl_es_version = 10;

	config.window_width = 100;
	config.window_height = 100;
	config.window_visual = PIGLIT_GL_VISUAL_RGB | PIGLIT_GL_VISUAL_DOUBLE;

PIGLIT_GL_TEST_CONFIG_END

enum piglit_result
piglit_display(void)
{
    const GLenum format = GL_ETC1_RGB8_OES;
    const GLsizei width = 8, height = 8;

    /* Each ETC1 block encodes 4x4 pixels and is 8 bytes. Therefore this
     * image size is 32 bytes.
     */
    const GLubyte fake_tex_data[32];

    GLuint t;
    int pass = GL_TRUE;

    glGenTextures(1, &t);
    glBindTexture(GL_TEXTURE_2D, t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glClear(GL_COLOR_BUFFER_BIT);

    /* GL_ETC1_RGB8_OES is not a valid format for glTexImage2D().
     *
     * From page 73 OpenGL ES 1.1 spec:
     *
     *     Specifying a value for internalformat that is not one of the above
     *     values generates the error INVALID VALUE.
     */
    glTexImage2D(GL_TEXTURE_2D, 0, format,
            width, height, 0, format, GL_UNSIGNED_BYTE, fake_tex_data);
    pass = piglit_check_gl_error(GL_INVALID_VALUE) && pass;

    glCopyTexImage2D(GL_TEXTURE_2D, 0, format, 0, 0, width, height, 0);
#if defined(PIGLIT_USE_OPENGL) || defined(PIGLIT_USE_OPENGL_ES3)
    pass = piglit_check_gl_error(GL_INVALID_VALUE) && pass;
#else  /* defined(PIGLIT_USE_OPENGL_ES1) || defined(PIGLIT_USE_OPENGL_ES2) */
    pass = piglit_check_gl_error(GL_INVALID_ENUM) && pass;
#endif

    /* From the GL_OES_compressed_ETC1_RGB8_texture spec:
     *
     *     INVALID_OPERATION is generated by CompressedTexSubImage2D,
     *     TexSubImage2D, or CopyTexSubImage2D if the texture image <level>
     *     bound to <target> has internal format ETC1_RGB8_OES.
     */
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
            width, height, GL_RGB, GL_UNSIGNED_BYTE, fake_tex_data);
    pass = piglit_check_gl_error(GL_INVALID_OPERATION) && pass;

    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height);
    pass = piglit_check_gl_error(GL_INVALID_OPERATION) && pass;

    glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format,
                              sizeof(fake_tex_data), fake_tex_data);
    pass = piglit_check_gl_error(GL_INVALID_OPERATION) && pass;

    glDeleteTextures(1, &t);

    return (pass) ? PIGLIT_PASS : PIGLIT_FAIL;;
}

void
piglit_init(int argc, char **argv)
{
    piglit_require_extension("GL_OES_compressed_ETC1_RGB8_texture");
}
