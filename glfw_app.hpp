#ifndef GLFW_APP_HPP_
#define GLFW_APP_HPP_

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <stdio.h>
#include <stdlib.h>


#ifdef GLFW_APP_DEBUG
#define DEBUG(msg) (fprintf(stderr, msg))
#else
#define DEBUG(msg)
#endif


#ifndef GLFW_APP_SUPPRESS_ERROR
#define ERROR(msg) (fprintf(stderr, msg))
#else
#define ERROR(msg)
#endif

#define GLSL(version, shader) {"#version " #version "\n" #shader}

#ifndef MAX_GLSL_LOG
#define MAX_GLSL_LOG 1 << 15
#endif
#ifndef WINDOW_WIDTH
#define WINDOW_WIDTH 800
#endif
#ifndef WINDOW_HEIGHT
#define WINDOW_HEIGHT 640
#endif
#ifndef WINDOW_NAME_STRING
#define WINDOW_NAME_STRING "Glfw App"
#endif

void check_gl_error();
void check_glsl_status(GLuint obj, const char *success_msg, const char *error_msg);

class glfw_app {
public:
	GLFWwindow *window;
	int width, height;
	double current_time;

	int run() {
		while (!glfwWindowShouldClose(window)) {
			current_time = glfwGetTime();
			glfwGetFramebufferSize(window, &width, &height);
			glfwPollEvents();
			events();
			update();
			draw();
			glfwSwapBuffers(window);
		}

		return 0;

	}

	void init_app() {
		ERROR("Starting app\n");

		if (!glfwInit()) {
			ERROR("Failed to initialize glfw\n");
			exit(1);
		}

		window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
					  WINDOW_NAME_STRING, NULL, NULL);
		if (!window) {
			ERROR("Failed to create window\n");
			glfwTerminate();
			exit(1);
		}
      		glfwMakeContextCurrent(window);
		#ifndef GLFW_APP_NO_VSYNC
		glfwSwapInterval(1);
		#endif

		if (glewInit() != GLEW_OK) {
			ERROR("Glew failed to initialize\n");
			glfwTerminate();
			exit(1);
		}

		init();
	}
	void cleanup_app() {
		cleanup();
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	virtual void init() {}
	virtual void draw() {}
	virtual void events() {}
	virtual void update() {}
	virtual void cleanup() {}
	virtual void on_mouse_button(GLFWwindow *window,
				     int button, int action,
				     int mods) {}
	virtual void on_cursor_pos(GLFWwindow *window,
				   double xpos, double ypos) {}
	virtual void on_cursor_enter(GLFWwindow *window,
				     int entered) {}
	virtual void on_scroll(GLFWwindow *window,
			       double xoffset, double yoffset) {}
	virtual void on_key(GLFWwindow *window,
			    int key, int scancode,
			    int action, int mods) {}
	virtual void on_char(GLFWwindow *window,
			     unsigned int codepoint) {}
};

#define GLFW_APP_DECLARE_MAIN(classname) \
classname app_inst;\
\
void _glfw_app_mouse_btn_func(GLFWwindow *win, int btn, int act, int mod)\
{app_inst.on_mouse_button(win, btn, act, mod);}\
void _glfw_app_cursor_pos_func(GLFWwindow *win, double x, double y)\
{app_inst.on_cursor_pos(win, x, y);}\
void _glfw_app_cursor_enter_func(GLFWwindow *win, int ent)\
{app_inst.on_cursor_enter(win, ent);}\
void _glfw_app_scroll_func(GLFWwindow *win, double xoff, double yoff)\
{app_inst.on_scroll(win, xoff, yoff);}\
void _glfw_app_key_func(GLFWwindow *win, int k, int sc, int act, int mod)\
{app_inst.on_key(win, k, sc, act, mod);}\
void _glfw_app_char_func(GLFWwindow *win, unsigned int cp)\
{app_inst.on_char(win, cp);}\
\
int main(void)\
{\
	app_inst.init_app();\
	glfwSetMouseButtonCallback(app_inst.window, (GLFWmousebuttonfun)&_glfw_app_mouse_btn_func);\
	glfwSetCursorPosCallback(app_inst.window, (GLFWcursorposfun)&_glfw_app_cursor_pos_func);\
	glfwSetCursorEnterCallback(app_inst.window, (GLFWcursorenterfun)&_glfw_app_cursor_enter_func);\
	glfwSetScrollCallback(app_inst.window, (GLFWscrollfun)&_glfw_app_scroll_func);\
	glfwSetKeyCallback(app_inst.window, (GLFWkeyfun)&_glfw_app_key_func);\
	glfwSetCharCallback(app_inst.window, (GLFWcharfun)&_glfw_app_char_func);\
	app_inst.run();\
	app_inst.cleanup_app();\
	return 0;\
}\


#endif
