#include <string.h>

#include <GL/glew.h>

#include "stereo-gl.h"

#include <GL/glext.h>

#include "private/stereo-shader.h"

GLuint
stereo_gl_program_create(void)
{
    const GLchar *stereo_shader_source = stereo_shader;

    GLuint shader, program;
    GLint length;
    GLint compiled, linked;

    /* Create the shader */
    shader = glCreateShader(GL_FRAGMENT_SHADER);
    if (!shader) {
        return 0;
    }

    /* Compile it... */
    length = strlen(stereo_shader_source);
    glShaderSource(shader, 1, &stereo_shader_source, &length);
    glCompileShader(shader);
    glGetObjectParameterivARB(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        glDeleteShader(shader);
        return 0;
    }

    /* Create the program and attach the shader */
    program = glCreateProgram();
    if (!program) {
        glDeleteShader(shader);
        return 0;
    }
    glAttachShader(program, shader);

    /* Finally link the program */
    glLinkProgram(program);
    glGetObjectParameterivARB(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        glDeleteProgram(program);
        return 0;
    }

    return program;
}
