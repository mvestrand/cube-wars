#ifndef GLFW_GAME_PHYSICS_OBJECT_HPP_
#define GLFW_GAME_PHYSICS_OBJECT_HPP_

#include "object.hpp"

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include "glm/glm.hpp"

class physics_object: public game_object {
public:
	physics_object();
	virtual ~physics_object() {}

	virtual bool on_ground();
	virtual void on_fall() {} // Called upon leaving the ground
	virtual void on_land() {} // Called upon landing on the ground
	virtual void in_block(int type) {} // Called when inside a block
	virtual void on_bounce(glm::vec3 dir) {} // Called upon bouncing off the ground
	virtual void on_collision(game_object *obj, int msg) {}
	void friction(double t);
	void gravity(double t);
	void wall_collision(double t);
	void collision_sphere(glm::vec3 pos, float radius, int msg);

	bool midair; // Whether or not the object is in the air

	glm::vec3 vel;
	float elasticity = 0.0;
	float friction_coeff = 0.0;
	float in_block_vel = 10.0; // Speed used to force obj out of blocks
};

#endif
