#ifndef STEREO_GL_H
#define STEREO_GL_H

#include <GL/gl.h>

#include "stereo.h"

typedef struct StereoImageGL StereoImageGL;

/**
 * Creates an OpenGL fragment shader program.
 *
 * The program requires three uniform values:
 *   1. float strength: The strength of the stereo effect. This is the same as
 *      the strength parameter to stereo_image_create.
 *   2. sampler2D pattern: The background pattern to use.
 *   3. sampler2DShadow zbuffer: The z-buffer that is rendered.
 *
 * @param pattern
 *     The background pattern to use.
 * @return a stereogram that may be used as an OpenGL shader program.
 */
StereoImageGL*
stereo_image_gl_create(StereoPattern *pattern);

/**
 * Frees a stereogram and all resources allocated by it.
 *
 * @param stereo_gl
 *     The stereogram to free.
 */
void
stereo_image_gl_free(StereoImageGL *stereo_gl);

/**
 * Sets the stereogram algorithm as the current fragments shader.
 *
 * stereo_image_gl_end must be called when this function
 *
 * @param stereo_gl
 *     The stereogram.
 * @param strength
 *     The strength to use when applying the effect. The greater the strength,
 *     the deeper the image appears.
 * @param zbuffer
 *     The z-buffer to use as source.
 * @return 0 if the fragment shader was sucessfully set, otherise an error code
 */
int
stereo_image_gl_start(StereoImageGL *stereo_gl, GLfloat strength,
    GLuint zbuffer);

/**
 * Sets the stereogram algorithm as the current fragments shader.
 *
 * @param stereo_gl
 *     The stereogram.
 * @param zbuffer
 *     The z-buffer to use as source.
 */
void
stereo_image_gl_end(StereoImageGL *stereo_gl);

#endif
