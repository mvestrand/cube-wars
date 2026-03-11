#ifndef GLFW_GAME_ROUND_MANAGER_HPP_
#define GLFW_GAME_ROUND_MANAGER_HPP_

#include "glm/glm.hpp"

class world;
class game_player;

#define MAX_WORLD_NAME 256

class round_manager {
public:
	round_manager(world *wrld);
	void show_main_menu();
	void new_game(int players, int rounds, const char *filename,
		      int x, int y, int z);
	glm::vec3 get_player_color(int player);
	void new_round();
	void update(double time);
	void end_round(int winner);
	void end_game();
	char *get_string(const char *prompt);
	void declare_draw();
	void declare_round_winner(game_player *player);
	void declare_game_winner(game_player *player);
	void update_menu(double time);
	void update_game(double time);
	void load_world_file(const char *filename);

	world *game_world;
	bool midgame = false;
	int num_players = 0;
	double timer = 0.0;
	double win_time = 0.3;
	int round_num = 0;
	int goal = 0;
	const char *last_prompt = NULL;
	char world_name[MAX_WORLD_NAME] = {0};
};

#endif
