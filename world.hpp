#ifndef GLFW_GAME_WORLD_HPP_
#define GLFW_GAME_WORLD_HPP_

#include <list>
#include "glfw_game.hpp"
#include "terrain.hpp"
#include "object.hpp"
#include "physics_object.hpp"
#include "player.hpp"

class glfw_game;
class game_player;
class round_manager;

class world {
public:
	world() {
		objects = new std::list<game_object *>;
		players = new std::list<game_player *>;
	}
	~world() {
		delete objects;
		delete players;
	}
	void init();
	void add_object(game_object *obj);
	void del_object(game_object *obj);
	void del_objects();
	void del_all_objects();

	int add_player();
	game_player *get_player(int n);
	void del_player(int n);
	void del_all_players();

	void cleanup();
	void draw(double time);
	void update(double time);
	void events(double time);

	void new_game(int players);
	void start_round(int players);
	void end_round(int players);

	glm::vec3 get_cam_track_center(float *max_dist, int *count);
	void calc_cam_settings(glm::mat4 mvp_matrix,
			       float *max_x, float *min_x,
			       float *max_y, float *min_y);
	double get_death_height() {return -6;}

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

	terrain *ground = NULL;
	round_manager *round_man = NULL;
	std::list<game_object *> *objects = NULL;
	std::list<game_player *> *players = NULL;

	glfw_game *game = NULL;
};

#endif
