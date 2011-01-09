#include <malloc.h>
#include <string.h>

#include <GL/glew.h>

#include "stereo-gl.h"

#include "private/stereo-shader.h"

struct StereoImageGL {
    /* The stereogram program */
    GLuint shader, program;

    /* The background pattern to use */
    StereoPattern *pattern;

    /* The indice of the program uniform values */
    struct {
        GLint strength;
        GLint left_margin;
        GLint pattern;
        GLint zbuffer;
    } i;

    /* The resourcfes used during runtime */
    struct {
        struct {
            GLint pattern;
        } textures;
    } r;
};

StereoImageGL*
stereo_image_gl_create(StereoPattern *pattern)
{
    StereoImageGL *result = NULL;
    const GLchar *stereo_shader_source = stereo_shader;

    GLuint shader, program;
    GLint length;
    GLint compiled, linked;

    /* pattern cannot be NULL */
    if (!pattern) {
        return NULL;
    }

    /* Create the shader */
    shader = glCreateShader(GL_FRAGMENT_SHADER);
    if (!shader) {
        return NULL;
    }

    /* Compile it... */
    length = strlen(stereo_shader_source);
    glShaderSource(shader, 1, &stereo_shader_source, &length);
    glCompileShader(shader);
    glGetObjectParameterivARB(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLchar buffer[1024];
        glGetShaderInfoLog(shader, sizeof(buffer), NULL, buffer);
        printf("%s", buffer);

        glDeleteShader(shader);
        return NULL;
    }

    /* Create the program and attach the shader */
    program = glCreateProgram();
    if (!program) {
        glDeleteShader(shader);
        return NULL;
    }
    glAttachShader(program, shader);

    /* Finally link the program */
    glLinkProgram(program);
    glGetObjectParameterivARB(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        glDeleteShader(shader);
        glDeleteProgram(program);
        return NULL;
    }

    /* Create the result struct */
    result = malloc(sizeof(*result));
    memset(result, 0, sizeof(*result));
    result->shader = shader;
    result->program = program;
    result->pattern = pattern;
    result->i.strength = glGetUniformLocation(program, "strength");
    result->i.left_margin  = glGetUniformLocation(program, "left_margin");
    result->i.pattern  = glGetUniformLocation(program, "pattern");
    result->i.zbuffer  = glGetUniformLocation(program, "zbuffer");

    return result;
}

void
stereo_image_gl_free(StereoImageGL *stereo_gl)
{
    glDeleteShader(stereo_gl->shader);
    glDeleteProgram(stereo_gl->program);
}

int
stereo_image_gl_start(StereoImageGL *stereo_gl, GLfloat strength,
    GLuint zbuffer)
{
    glGenTextures(sizeof(stereo_gl->r.textures) / sizeof(GLuint),
        (GLuint*)&stereo_gl->r.textures);

    /* Create the pattern texture */
    glBindTexture(GL_TEXTURE_2D, stereo_gl->r.textures.pattern);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        stereo_gl->pattern->width, stereo_gl->pattern->height,
        0, GL_RGBA, GL_UNSIGNED_BYTE, stereo_gl->pattern->pixels);

    /* Set the program uniform values */
    glUniform1f(stereo_gl->i.strength, strength);
    glUniform1i(stereo_gl->i.left_margin, stereo_gl->pattern->width);
    glUniform1i(stereo_gl->i.pattern, stereo_gl->r.textures.pattern);
    glUniform1i(stereo_gl->i.zbuffer, zbuffer);

    glUseProgram(stereo_gl->program);

    return 0;
}

void
stereo_image_gl_end(StereoImageGL *stereo_gl)
{
    glDeleteTextures(sizeof(stereo_gl->r.textures) / sizeof(GLuint),
        (GLuint*)&stereo_gl->r.textures);
    memset(&stereo_gl->r.textures, 0, sizeof(stereo_gl->r.textures));
}
