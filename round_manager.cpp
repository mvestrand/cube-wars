#include "round_manager.hpp"
#include "glfw_game.hpp"
#include "terrain.hpp"
#include "object.hpp"
#include "physics_object.hpp"
#include "player.hpp"
#include "character.hpp"
#include "glm/glm.hpp"

#include <iostream>
#include <string>

round_manager::round_manager(world *wrld)
{
	game_world = wrld;
	show_main_menu();
}

void round_manager::show_main_menu()
{
	if (midgame)
		end_game();

	game_world->game->camera.in_main_menu = true;
	game_world->ground = new terrain(game_world, 17, 20, 3);
	game_world->ground->load("title.terra");
	num_players = 0;
	goal = 0;

}

glm::vec3 round_manager::get_player_color(int player)
{
	switch(player) {
	case 1:
		return glm::vec3(1.0f, 0.0f, 0.0f);
	case 2:
		return glm::vec3(0.0f, 0.0f, 1.0f);
	case 3:
		return glm::vec3(0.0f, 1.0f, 0.0f);
	case 4:
		return glm::vec3(1.0f, 0.0f, 1.0f);
	case 5:
		return glm::vec3(1.0f, 1.0f, 0.0f);
	case 6:
		return glm::vec3(0.0f, 1.0f, 1.0f);
	case 7:
		return glm::vec3(1.0f, 0.6f, 0.0f);
	case 8:
		return glm::vec3(0.0f, 1.0f, 0.6f);
	}
	return glm::vec3(0.8f, 0.8f, 0.8f);
}

#define MAX_LINE 256

char *next_token(char *str, char token)
{
	char *cp;
	static char *last_str = NULL;
	if (str == NULL) {
		str = last_str;
		if (str == NULL)
			return NULL;
	}
	if ((cp = strchr(str, token)) != NULL) {
		*cp = '\0';
		last_str = cp + 1;
		return str;
	} else {
		last_str = NULL;
		return str;
	}
}

void round_manager::load_world_file(const char *filename)
{
	char line[MAX_LINE];
	FILE *fptr = fopen(filename, "r");
	char *cp1 = NULL;
	char *cp2 = NULL;
	int x, y, z;
	if (fptr == NULL) {
		perror("Error opening world file");
		exit(1);
	}
	// Get world dimensions
	if (fgets(line, MAX_LINE, fptr) != NULL) {
		x = atoi(next_token(line, ','));
		y = atoi(next_token(NULL, ','));
		z = atoi(next_token(NULL, ','));
		game_world->ground = new terrain(game_world, x, y, z);
	}
	// Load terrain files listed in the world file
	while (fgets(line, MAX_LINE, fptr) != NULL) {
		x = atoi(next_token(line, ','));
		y = atoi(next_token(NULL, ','));
		z = atoi(next_token(NULL, ','));
		cp1 = next_token(NULL, ',');
		if ((cp2 = strchr(cp1, '\n')) != NULL) {
			*cp2 = '\0';
		}
		game_world->ground->load_at(cp1, x, y, z);
	}
}

void round_manager::new_game(int players, int rounds, const char *filename, int x, int y, int z)
{
	if (midgame == true) {
		end_game();
	}
	midgame = true;
	num_players = players;
	goal = rounds;

	if (game_world->ground != NULL) {
		delete game_world->ground;
	}

	load_world_file(filename);

	int i;
	for (i = 1; i <= players; i++) {
		game_world->add_player();
		character *chara = new character;
		game_world->add_object(chara);
		game_world->get_player(i)->avatar = chara;
		glm::vec3 color = get_player_color(i);
		chara->color[0] = color.x;
		chara->color[1] = color.y;
		chara->color[2] = color.z;
	}
	round_num = 0;
	game_world->game->camera.in_main_menu = false;
	new_round();

}

void round_manager::new_round()
{
	int i;
	game_player *player;
	if (!midgame)
		return;
	for (i = 1; i <= num_players; i++) {
		player = game_world->get_player(i);
		player->avatar->pos = game_world->ground->get_start_location(i);
		player->avatar->reset();
	}
	round_num++;
	printf("Round %d\n", round_num);
}

void round_manager::update_game(double time)
{
	std::list<game_player *> *players = game_world->players;
	std::list<game_player *>::iterator i;
	int living_players = num_players;
	game_player *last_living_player = NULL;
	for (i = players->begin(); i != players->end(); i++) {
		if (!(*i)->avatar->is_alive()) {
			living_players--;
		}
		else {
			last_living_player = (*i);
		}
	}
	if (living_players == 0) {
		end_round(0);
	} else if (living_players == 1) {
		if (timer >= win_time)
			end_round(last_living_player->player_num);
		else
			timer += time;
	} else {
		timer = 0.0;
	}
}



char *round_manager::get_string(const char *prompt)
{
	if (prompt != last_prompt) {
		last_prompt = prompt;
		printf(prompt);
		game_world->game->start_keyboard();
	}

	char *line = game_world->game->get_keyboard_line();
	if (line != NULL) {
		game_world->game->stop_keyboard();
	}

	return line;
}

void round_manager::update_menu(double time)
{
	const char *str = NULL;
	if (strlen(world_name) == 0) {
		if ((str = get_string("Enter world file name: ")) != NULL) {
			strncpy(world_name, str, sizeof(world_name));
		}
	} else if (num_players == 0) {
		if ((str = get_string("Enter number of players: ")) != NULL) {
			num_players = atoi(str);
			if (num_players < 1 || num_players > 20) {
				num_players = 0;
				last_prompt = NULL;
			}
		}
	} else if (goal == 0) {
		if ((str = get_string("Enter number of rounds: ")) != NULL) {
			goal = atoi(str);
			if (goal < 1 || goal > 100) {
				goal = 0;
				last_prompt = NULL;
			}
		}
	} else {
		last_prompt = NULL;
		new_game(num_players, goal, world_name, 20, 20, 20);
	}
}

void round_manager::update(double time)
{
	if (midgame)
		update_game(time);
	else
		update_menu(time);
}

void round_manager::end_round(int winner)
{
	if (winner <= 0) {
		declare_draw();
	} else {
		declare_round_winner(game_world->get_player(winner));
	}
	new_round();
}

void round_manager::declare_draw()
{
	printf("Draw!\n");
}

void round_manager::declare_round_winner(game_player *player)
{
	printf("Player %d wins!\n", player->player_num);
	player->score += 1;
	if (player->score >= goal) {
		declare_game_winner(player);
	}

}

void round_manager::declare_game_winner(game_player *player)
{
	printf("Player %d wins the game!\n", player->player_num);
	end_game();
	show_main_menu();
}

void round_manager::end_game()
{
	midgame = false;
	game_world->del_all_objects();
	game_world->del_all_players();
	if (game_world->ground != NULL) {
		delete game_world->ground;
		game_world->ground = NULL;
	}
	num_players = 0;
	goal = 0;
}
