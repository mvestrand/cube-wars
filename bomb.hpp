#ifndef GLFW_GAME_BOMB_HPP_
#define GLFW_GAME_BOMB_HPP_


#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#include "physics_object.hpp"
#include "GL/glew.h"
#include "glm/glm.hpp"


class character;

/* The bomb thrown by characters; explodes after the timer runs out */
class bomb: public physics_object {
public:
	~bomb();
	virtual void init();
	virtual void update(double time);
	virtual void draw(double time);

	void on_bounce(glm::vec3 dir);
	void explode(); // Make the bomb explode now

	bool exploded = false; // Keeps the bomb from exploding more than once
	double timer = 2.0; // Time before exploding
	double power = 2.0; // How much damage the bomb does
	double blast_radius = 1.25;

	float bounce_fric = 0.9; // Fraction of velocity kept upon bouncing
	character *owner = NULL; // Character that threw the bomb; used to give player back a bomb
	                         // after exploding

	GLuint buffer[2]; // Stores vertex buffer names
};

#endif
