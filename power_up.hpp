#ifndef GLFW_GAME_POWER_UP_
#define GLFW_GAME_POWER_UP_

#include "physics_object.hpp"
#include "glm/gtc/random.hpp"
#include "GL/glew.h"

#define PWR_UP_SPIN_SPEED   135.0f
#define PWR_UP_PULSE_FRAC 0.15f
#define PWR_UP_PULSE_SLOW 5.00f
#define PWR_UP_PULSE_FAST 20.0f
#define PWR_UP_LIFE_SPAN    15.0

#define PWR_UP_TYPE_COUNT  4

#define SPEED_PWR_UP       1
#define MAX_BOMB_PWR_UP    2
#define BLAST_RAD_PWR_UP   3
#define SHIELD_PWR_UP      4

#include <stdlib.h>
#include <time.h>

class character;

class power_up: public physics_object {
public:
	power_up() {
		srand(time(NULL));
		type = rand() % PWR_UP_TYPE_COUNT + 1;
		timer = PWR_UP_LIFE_SPAN;
		lifespan = PWR_UP_LIFE_SPAN;
	}
	power_up(int typ) {
		type = typ;
		timer = PWR_UP_LIFE_SPAN;
		lifespan = PWR_UP_LIFE_SPAN;
	}
	power_up(int typ, double duration) {
		type = typ;
		timer = duration;
		lifespan = duration;
	}
	~power_up() {
		glDeleteBuffers(2, buffer);
	}
	void init();
	void update(double time);
	void draw(double time);

	void destroy();
	void upgrade(character *chara);
	void on_collision(game_object *obj, int msg);

	int type;
	double timer;
	double lifespan;
	bool used = false;
	GLuint buffer[2];
};
#endif
