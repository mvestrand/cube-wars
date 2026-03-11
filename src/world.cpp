#include "world.hpp"
#include "round_manager.hpp"
#include "character.hpp"
#include <algorithm>
#include "power_up.hpp"

glm::vec3 world::get_cam_track_center(float *max_dist, int *count)
{
	int obj_count = 0;
	glm::vec3 pos_sum = glm::vec3(0.0f, 0.0f, 0.0f);
	*max_dist = 0.0f;

	std::list<game_object *>::iterator i;
	if (!objects->empty()) {
		for (i = objects->begin(); i != objects->end(); i++) {
			if ((*i)->should_track) {
				pos_sum += (*i)->get_center();
				obj_count++;
			}
		}
	}
	if (!objects->empty()) {
		for (i = objects->begin(); i != objects->end(); i++) {
			if ((*i)->should_track) {
				*max_dist = std::max(glm::length((*i)->pos - pos_sum), *max_dist);
			}
		}
	}
	*count = obj_count;
	return pos_sum;
}







void world::calc_cam_settings(glm::mat4 mvp_matrix, float *max_x, float *min_x, float *max_y, float *min_y)
{
	int obj_count = 0;
// 	glm::vec3 pos_sum = glm::vec3(0.0f, 0.0f, 0.0f);
// 	*max_dist = 0.0f;
	*max_x = 0;
	*min_x = 0;
	*max_y = 0;
	*min_y = 0;

	std::list<game_object *>::iterator i;
	if (!objects->empty()) {
		for (i = objects->begin(); i != objects->end(); i++) {
			if ((*i)->should_track) {
				obj_count++;
				glm::vec4 pos = glm::vec4((*i)->get_center(), 1.0f);
				pos = mvp_matrix * pos;
				pos = glm::vec4(pos.x/pos.w, pos.y/pos.w, pos.x/pos.w, pos.w/pos.w);
				if (obj_count == 1) {
					*max_x = pos.x;
					*min_x = pos.x;
					*max_y = pos.y;
					*min_y = pos.y;
				} else {
					*max_x = std::max(pos.x, *max_x);
					*min_x = std::min(pos.x, *min_x);
					*max_y = std::max(pos.y, *max_y);
					*min_y = std::min(pos.y, *min_y);
				}

			}
		}
	}

// 	if (!objects->empty()) {
// 		for (i = objects->begin(); i != objects->end(); i++) {
// 			if ((*i)->should_track) {
// 				*max_dist = std::max(glm::length((*i)->pos - pos_sum), *max_dist);
// 			}
// 		}
// 	}
// 	*count = obj_count;
// 	return pos_sum;

}




/**
 * Add an object to the world
 * @param game_object *obj, the object to add
 */
void world::add_object(game_object *obj)
{
	obj->world = this;
	objects->push_back(obj);
	obj->init();
}

/**
 * Mark an object for deletion
 * @param game_object *obj, the object to be deleted
 */
void world::del_object(game_object *obj)
{
	obj->should_delete = true;
}

void world::del_objects()
{
	if (!objects->empty()) {
		std::list<game_object *>::iterator i;
		for (i = objects->begin(); i != objects->end();) {
			if ((*i)->should_delete) {
				delete (*i);
				i = objects->erase(i);
			} else {
				i++;
			}
		}
	}
}

/**
 * Add a new player to the world
 * @returns int, the number of the newly created player
 */
int world::add_player()
{
	game_player *player = new game_player;
	player->world = this;
	player->player_num = players->size() + 1;
	players->push_back(player);
	player->init();
	return players->size();
}

/**
 * Get the pointer to a player
 * @param int n, the number of the player to get
 * @returns game_player *, the player asked for, NULL if not found
 */
game_player *world::get_player(int n)
{
	if (!players->empty()) {
		std::list<game_player *>::iterator i;
		for (i = players->begin(); i != players->end(); i++) {
			if ((*i)->player_num == n) {
				return *i;
			}
		}
	}

	return NULL;
}

/**
 * Remove a player from the world
 * @param int n, the number of the player to remove
 */
void world::del_player(int n)
{
	if (!players->empty()) {
		// Remove the player
		std::list<game_player *>::iterator i;
		int x;
		for (i = players->begin(); i != players->end(); i++) {
			if ((*i)->player_num == n) {
				(*i)->cleanup();
				delete (*i);
				i = players->erase(i);
			}
		}
		// Renumber the players
		x = 1;
		for (i = players->begin(); i != players->end(); i++) {
			(*i)->player_num = x;
			x++;
		}
	}
}

void world::del_all_objects()
{
	while (!objects->empty()) {
		delete objects->back();
		objects->pop_back();
	}
}

void world::del_all_players()
{
	while (!players->empty()) {
		delete players->back();
		players->pop_back();
	}
}

void world::init()
{
// 	fprintf(stderr, "enter: world::init()\n");
	if (game == NULL)
		fprintf(stderr, "Error: World not given a ptr to the app\n");

	round_man = new round_manager(this);
// 	round_man->new_game(2, 3, "terrain.terra", 17, 20, 4);

}

void world::cleanup()
{
// 	fprintf(stderr, "enter: world::cleanup()\n");

	while (!objects->empty()) {
		delete objects->back();
		objects->pop_back();
	}
	while (!players->empty()) {
		delete players->back();
		players->pop_back();
	}
	delete ground;
// 	fprintf(stderr, "exit: world::cleanup()\n");
}

void world::draw(double time)
{
// 	fprintf(stderr, "enter: world::draw()\n");

	ground->draw();

	std::list<game_object *>::iterator i;
	if (!objects->empty()) {

		for (i = objects->begin(); i != objects->end(); i++) {
			game->set_draw_location((*i)->pos, (*i)->rot, (*i)->scale);
			(*i)->draw(time);
		}
	}

	std::list<game_player *>::iterator i2;
	if (!players->empty()) {

		for (i2 = players->begin(); i2 != players->end(); i2++) {
			(*i2)->draw(time);
		}
	}

// 	fprintf(stderr, "exit: world::draw()\n");
}

void world::update(double time)
{
// 	fprintf(stderr, "enter: world::update()\n");
	std::list<game_object *>::iterator i;
	if (!objects->empty()) {

		for (i = objects->begin(); i != objects->end(); i++) {
			(*i)->update(time);
		}
	}

	std::list<game_player *>::iterator i2;
	if (!players->empty()) {

		for (i2 = players->begin(); i2 != players->end(); i2++) {
			(*i2)->update(time);
		}
	}
	del_objects();
	round_man->update(time);
}

void world::events(double time)
{
// 	fprintf(stderr, "enter: world::events()\n");
	std::list<game_object *>::iterator i;
	if (!objects->empty()) {

		for (i = objects->begin(); i != objects->end(); i++) {
			(*i)->events(time);
		}
	}

	std::list<game_player *>::iterator i2;
	if (!players->empty()) {

		for (i2 = players->begin(); i2 != players->end(); i2++) {
			(*i2)->events(time);
		}
	}
// 	fprintf(stderr, "exit: world::events()\n");
}

void world::on_mouse_button(GLFWwindow *window,
		     int button, int action,
		     int mods)
{
	std::list<game_player *>::iterator i2;
	if (!players->empty()) {

		for (i2 = players->begin(); i2 != players->end(); i2++) {
			(*i2)->on_mouse_button(window, button, action, mods);
		}
	}

}
void world::on_cursor_pos(GLFWwindow *window,
		   double xpos, double ypos)
{
	std::list<game_player *>::iterator i2;
	if (!players->empty()) {

		for (i2 = players->begin(); i2 != players->end(); i2++) {
			(*i2)->on_cursor_pos(window, xpos, ypos);
		}
	}

}
void world::on_cursor_enter(GLFWwindow *window,
		     int entered)
{
	std::list<game_player *>::iterator i2;
	if (!players->empty()) {

		for (i2 = players->begin(); i2 != players->end(); i2++) {
			(*i2)->on_cursor_enter(window, entered);
		}
	}

}
void world::on_scroll(GLFWwindow *window,
	       double xoffset, double yoffset)
{
	std::list<game_player *>::iterator i2;
	if (!players->empty()) {

		for (i2 = players->begin(); i2 != players->end(); i2++) {
			(*i2)->on_scroll(window, xoffset, yoffset);
		}
	}

}
void world::on_key(GLFWwindow *window,
	    int key, int scancode,
	    int action, int mods)
{
// 	fprintf(stderr, "enter: world::on_key()\n");
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		power_up *pow = new power_up();
		pow->pos = glm::vec3(0.0f, 5.0f, 1.0f);
		add_object(pow);
	}


	std::list<game_player *>::iterator i2;
	if (!players->empty()) {

		for (i2 = players->begin(); i2 != players->end(); i2++) {
			(*i2)->on_key(window, key, scancode,
				      action, mods);
		}
	}
// 	fprintf(stderr, "exit: world::on_key()\n");

}

void world::on_char(GLFWwindow *window,
	     unsigned int codepoint)
{
	std::list<game_player *>::iterator i2;
	if (!players->empty()) {

		for (i2 = players->begin(); i2 != players->end(); i2++) {
			(*i2)->on_char(window, codepoint);
		}
	}

}
