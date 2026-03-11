#ifndef GLFW_GAME_EXPLOSION_HPP_
#define GLFW_GAME_ EXPLOSION_HPP_

#include "physics_object.hpp"
#include "GL/glew.h"
#include <list>

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include "glm/glm.hpp"

class explosion: public physics_object {
public:
	explosion(glm::vec3 position, float dmg, float exp_time, float init_rad, float final_rad)
		{
			pos = position;
			damage = dmg;
			timer = exp_time;
			expansion_rate = (final_rad - init_rad) / exp_time;
			radius = init_rad;
			scale = init_rad;
		}
	~explosion() {
		glDeleteBuffers(2, buffer);
	}
	virtual void init();
	virtual void update(double time);
	virtual void draw(double time);
	virtual void on_collision(game_object *obj, int msg);
	bool has_damaged_block(int x, int y, int z);
	void damaged_block(int x, int y, int z);

	float damage;
	float timer;
	float expansion_rate;

	std::list<game_object *> affected; //Objects affected by the explosion
	std::list<glm::vec3> damaged_blocks; // Blocks affected by the explosion

	GLuint buffer[2];
};

#endif
