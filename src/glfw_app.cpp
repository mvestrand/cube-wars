#include "glfw_app.hpp"

void check_gl_error()
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		ERROR((char *)gluErrorString(err));
		while ((err = glGetError()) != GL_NO_ERROR) {
			ERROR((char *)gluErrorString(err));
		}
	}
}

void check_glsl_status(GLuint obj, const char *success_msg, const char *error_msg)
{
	GLint status = GL_FALSE;
	if (glIsShader(obj))
		glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
	if (glIsProgram(obj))
		glGetProgramiv(obj, GL_LINK_STATUS, &status);
	if (status == GL_TRUE) {
		DEBUG(success_msg);
		return;
	}
	char log[MAX_GLSL_LOG] = {0};
	if (glIsShader(obj))
		glGetShaderInfoLog(obj, sizeof(log), NULL, log);
	if (glIsProgram(obj))
		glGetProgramInfoLog(obj, sizeof(log), NULL, log);
	ERROR(error_msg);
	ERROR(log);
	exit(-1);
}
