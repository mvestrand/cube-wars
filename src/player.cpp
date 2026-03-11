#include "player.hpp"
#include "character.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"

#define PLAYER_1_UP    GLFW_KEY_W
#define PLAYER_1_DOWN  GLFW_KEY_S
#define PLAYER_1_LEFT  GLFW_KEY_A
#define PLAYER_1_RIGHT GLFW_KEY_D
#define PLAYER_1_B1    GLFW_KEY_SPACE
#define PLAYER_1_B2    GLFW_KEY_F
#define PLAYER_1_B3    GLFW_KEY_G

#define PLAYER_2_UP    GLFW_KEY_UP
#define PLAYER_2_DOWN  GLFW_KEY_DOWN
#define PLAYER_2_LEFT  GLFW_KEY_LEFT
#define PLAYER_2_RIGHT GLFW_KEY_RIGHT
#define PLAYER_2_B1    GLFW_KEY_KP_0
#define PLAYER_2_B2    GLFW_KEY_KP_1
#define PLAYER_2_B3    GLFW_KEY_KP_2

void game_player::init()
{

}

void game_player::cleanup()
{

}

void game_player::draw(double time)
{

}

void game_player::update(double time)
{

}

void game_player::events(double time)
{
	glm::vec4 dir_vec = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool b1 = false;
	bool b2 = false;
	bool b3 = false;

	if (player_num == 1) {
		up =    (glfwGetKey(world->game->window, PLAYER_1_UP) == GLFW_PRESS);
		down =  (glfwGetKey(world->game->window, PLAYER_1_DOWN) == GLFW_PRESS);
	        left =  (glfwGetKey(world->game->window, PLAYER_1_LEFT) == GLFW_PRESS);
		right = (glfwGetKey(world->game->window, PLAYER_1_RIGHT) == GLFW_PRESS);
		b1 =    (glfwGetKey(world->game->window, PLAYER_1_B1) == GLFW_PRESS);
		b2 =    (glfwGetKey(world->game->window, PLAYER_1_B2) == GLFW_PRESS);
		b3 =    (glfwGetKey(world->game->window, PLAYER_1_B3) == GLFW_PRESS);
	} else if (player_num == 2) {
		up =    (glfwGetKey(world->game->window, PLAYER_2_UP) == GLFW_PRESS);
		down =  (glfwGetKey(world->game->window, PLAYER_2_DOWN) == GLFW_PRESS);
	        left =  (glfwGetKey(world->game->window, PLAYER_2_LEFT) == GLFW_PRESS);
		right = (glfwGetKey(world->game->window, PLAYER_2_RIGHT) == GLFW_PRESS);
		b1 =    (glfwGetKey(world->game->window, PLAYER_2_B1) == GLFW_PRESS);
		b2 =    (glfwGetKey(world->game->window, PLAYER_2_B2) == GLFW_PRESS);
		b3 =    (glfwGetKey(world->game->window, PLAYER_2_B3) == GLFW_PRESS);
	}

	if (up)
		dir_vec += glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	if (down)
		dir_vec += glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	if (left)
		dir_vec += glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
	if (right)
		dir_vec += glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
// 	dir_vec = glm::rotate(glm::mat4(1.0f),
// 			      glm::angle(world->game->camera.rot) *
// 			      glm::dot(glm::axis(world->game->camera.rot),
// 				       glm::vec3(0.0f, 1.0f, 0.0f)),
// 			      glm::vec3(0.0f, 1.0f, 0.0f)) * dir_vec;


	avatar->move_dir(glm::vec2(dir_vec.x, dir_vec.z), time);

	if (b1) {
		if (jump_charge > 0) {
			jump_charge += time / jump_charge_time;
			if (jump_charge >= 1) {
				avatar->jump(1);
				jump_charge = 0;
			}
		}
	}
	if (b2) {
		avatar->charge_throw();
	}

	if (glfwGetKey(world->game->window, GLFW_KEY_R) == GLFW_PRESS) {
		avatar->reset();
	}
}

void game_player::on_mouse_button(GLFWwindow *window,
		     int button, int action,
		     int mods)
{
}

void game_player::on_cursor_pos(GLFWwindow *window,
		   double xpos, double ypos)
{

}

void game_player::on_cursor_enter(GLFWwindow *window,
		     int entered)
{

}

void game_player::on_scroll(GLFWwindow *window,
	       double xoffset, double yoffset)
{

}

void game_player::on_key(GLFWwindow *window,
	    int key, int scancode,
	    int action, int mods)
{
	if (player_num == 1) {
		if (key == PLAYER_1_B1) {
			if (action == GLFW_PRESS) {
				jump_charge = 0.001; // Is greater than 0
			} else if (action == GLFW_RELEASE) {
				if (jump_charge > 0) {
					avatar->jump(jump_charge);
					jump_charge = 0;
				}
			}
		}
		if (key == PLAYER_1_B2) {
			if (action == GLFW_PRESS) {
				avatar->charge_throw(); // Is greater than 0
			} else if (action == GLFW_RELEASE) {
				avatar->throw_bomb(1);
			}
		}

	}
	if (player_num == 2) {
		if (key == PLAYER_2_B1) {
			if (action == GLFW_PRESS) {
				jump_charge = 0.001; // Is greater than 0
			} else if (action == GLFW_RELEASE) {
				if (jump_charge > 0) {
					avatar->jump(jump_charge);
					jump_charge = 0;
				}
			}
		}

		if (key == PLAYER_2_B2) {
			if (action == GLFW_PRESS) {
				avatar->charge_throw(); // Is greater than 0
			} else if (action == GLFW_RELEASE) {
				avatar->throw_bomb(1);
			}
		}
	}

}

void game_player::on_char(GLFWwindow *window,
	     unsigned int codepoint)
{

}
