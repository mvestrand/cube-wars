#ifndef GLFW_GAME_PLAYER_HPP_
#define GLFW_GAME_PLAYER_HPP_

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

#include "world.hpp"

class character;

class game_player {
public:
	void init();
	void cleanup();
	void draw(double time);
	void update(double time);
	void events(double time);

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


	world *world;

	int player_num;
	int score = 0;
	double jump_charge = 0.0;
	double jump_charge_time = 0.01;
	double throw_charge = 0.0;
	double throw_charge_time = .7;
	character *avatar;
};

#endif
