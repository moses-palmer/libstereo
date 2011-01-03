#ifndef STEREO_GL_H
#define STEREO_GL_H

#include <GL/gl.h>

/**
 * Creates an OpenGL fragment shader program.
 *
 * The program requires three uniform values:
 *   1. float strength: The strength of the stereo effect. This is the same as
 *      the strength parameter to stereo_image_create.
 *   2. sampler2D pattern: The background pattern to use.
 *   3. sampler2DShadow zbuffer: The z-buffer that is rendered.
 *
 * @return a fragment shader program, or 0 if an error occurs.
 */
GLuint
stereo_gl_program_create(void);

#endif
