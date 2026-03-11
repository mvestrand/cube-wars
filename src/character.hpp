#ifndef GLFW_GAME_CHARACTER_HPP_
#define GLFW_GAME_CHARACTER_HPP_


#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#include "physics_object.hpp"
#include "GL/glew.h"
#include "glm/glm.hpp"


#define MIN_THROW_CHARGE 0.1
#define THROW_CHARGE_TIME 0.7


class character: public physics_object {
public:
	~character() {
		glDeleteBuffers(2, buffer);
	}
	virtual void init();
	virtual void update(double time);
	virtual void draw(double time);

	void on_fall();

	bool is_alive() {return alive;}

	void turn_towards(glm::vec2 dir, double t);
	void move_dir(glm::vec2 dir, double t);
	void jump(double charge);
	void charge_throw() {
		if (bombs > 0 && throw_charge == 0)
			throw_charge = MIN_THROW_CHARGE;
	}
	void throw_bomb(int type);
	virtual void damage(float dmg) {
		// If there is a shield, damage the shield first
		if (shield > 0) {
			if (shield < dmg) {
				dmg -= shield;
				shield = 0;
			} else {
				dmg = 0;
				shield -= dmg;
			}
		}
		health -= dmg;
		if (health <= 0)
			die();
	}
	void die() {
		health = 0;
		alive = false;
		should_track = false;
	}
	void revive() {
		health = 1.0;
		alive = true;
		should_track = true;
	}
	void reset() {
		revive();
		max_speed = 0.075;
		accel = 0.95;
		bombs = 1;
		shield = 0;
		power = 1;
	}


	bool alive = true;
	double throw_charge = 0; // How charged-up the next throw is
	double health = 1.0;
	double facing_dir = 0.0;
	double turn_speed = 1200;
	double static_turn_speed = 360;
	double midair_control = 0.65; // Ability to control character midair
	double max_speed = 0.075;
	double accel = 0.95;
	int midair_jumps = 1;
	int max_midair_jumps = 1;
	float jump_vel = 8.0f;
	double turn_ctrl = .6;
	float throw_speed = .18f;
	glm::vec2 fall_vel; // The planar velocity when leaving the ground
	glm::vec2 control_vel; // The planar velocity from using the controls

	double power = 1;
	int bombs = 1;
	double shield = 0;

	GLfloat color[3];
	GLuint buffer[2];
};

#endif
