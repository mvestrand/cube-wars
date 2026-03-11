#ifndef GLFW_GAME_HPP_
#define GLFW_GAME_HPP_

#define GLFW_APP_DEBUG
#define WINDOW_NAME_STRING "Game"

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#include "glfw_app.hpp"
#include "simple_cam.hpp"
#include "game_cam.hpp"
#include "vertex_arrays.hpp"
#include "world.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/transform.hpp"
#include <algorithm>

#include <stddef.h>

class world;

class glfw_game: public glfw_app {
public:
	void init();
	void draw();
	void cleanup();
	void update();
	void events();
	void on_mouse_button(GLFWwindow *window,
			     int button, int action,
			     int mods);
	void on_cursor_pos(GLFWwindow *window,
			   double xpos, double ypos);
	void on_cursor_enter(GLFWwindow *window,
			     int entered);
	void on_scroll(GLFWwindow *window,
		       double xoffset, double yoffset);
	void on_key(GLFWwindow *window,
		    int key, int scancode,
		    int action, int mods);
	void on_char(GLFWwindow *window,
		     unsigned int codepoint);

	void set_draw_location(glm::vec3 pos, glm::quat rot, float scale);

	// Keyboard functions
	void start_keyboard();
	char *get_current_keyboard();
	char *get_keyboard_line();
	void delete_keyboard_char();
	void put_keyboard_char(char c);
	void stop_keyboard();

	GLuint rend_prog;
	GLuint vao;

	// Locations of uniforms in default block
	GLuint rot_location;
	GLuint mv_location;
	GLuint proj_location;
	GLuint light_location;

	// For managing keyboard input
	bool get_text = false;
	char char_buffer[1024] = {0};
	int next_char = 0;

	game_cam camera;
	world *game_world;

	glm::mat4 rot_matrix;
};

#endif
