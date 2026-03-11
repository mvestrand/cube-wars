#include "physics_object.hpp"
#include "world.hpp"
#include <list>

#define GRAVITY_ACCEL -22.0f

physics_object::physics_object()
{
	pos = glm::vec3(0.0f, 5.0f, 0.0f);
	rot = glm::quat();
	vel = glm::vec3(0.0f, 0.0f, 0.0f);
}

// This function doesn't really work
void physics_object::friction(double time)
{
	if (glm::length(vel) == 0)
		return;

	if (!midair) {
		if (glm::sqrt(vel.x * vel.x + vel.z * vel.z) < friction_coeff * time) {
			vel.x = 0.0f;
			vel.z = 0.0f;
		} else {
			vel.x -= vel.x * (float)friction_coeff * (float)time;
			vel.z -= vel.z * (float)friction_coeff * (float)time;
		}
	}
}

/**
 * Find all objects within the given radius at the given position; calls on_collision()
 * @param vec3 pos, the position to test from
 * @param float radius, the radius to test in
 * @param int msg, the message to send to on_collision() (not used for anything right now)
 */
void physics_object::collision_sphere(glm::vec3 pos, float radius,
						 int msg)
{
	std::list<game_object *> *objects = (world->objects);
	if (!objects->empty()) {
		std::list<game_object *>::iterator i;
		for (i = objects->begin(); i != objects->end(); i++) {
			if (*i == this)
				continue;
			if (glm::length((*i)->pos
					+ glm::vec3(0.0f, (*i)->height / 2.0f, 0.0f)
					- pos)
			    < radius + (*i)->radius) {
				on_collision(*i, msg);
			}
		}
	}
}

/**
 * Test if the object is on the ground
 * @return bool, true if the object is on the ground
 */
bool physics_object::on_ground()
{
	double dist = world->ground->get_ground_dist(pos);
	bool grounded = dist < 0.0001 && dist > -0.0001;
	if (grounded && midair) {
		midair = false;
		on_land();
	} else if (!grounded && !midair) {
		midair = true;
		on_fall();
	}
	return grounded;
}

/**
 * Update the position based on the velocity and position of walls
 * @param double t, the time that has passed since the last update
 */
void physics_object::wall_collision(double t)
{
	float x_dist = 0.0f, z_dist = 0.0f;
	if (vel.x > 0) {
		x_dist = world->ground->get_wall_dist(pos, glm::vec2( 1.0f, 0.0f),
						      radius, height);
		if (vel.x > x_dist) {
			vel.x = -vel.x * elasticity;
			on_bounce(glm::vec3(-1.0f, 0.0f, 0.0f));
		} else {
			x_dist = vel.x;
		}
		pos.x += x_dist;
	} else if (vel.x < 0) {
		x_dist = -world->ground->get_wall_dist(pos, glm::vec2(-1.0f, 0.0f),
						       radius, height);
		if (vel.x < x_dist) {
			vel.x = -vel.x * elasticity;
			on_bounce(glm::vec3(1.0f, 0.0f, 0.0f));
		} else {
			x_dist = vel.x;
		}
		pos.x += x_dist;
	}
	if (vel.z > 0) {
		z_dist = world->ground->get_wall_dist(pos, glm::vec2( 0.0f, 1.0f),
						      radius, height);
		if (vel.z > z_dist) {
			vel.z = -vel.z * elasticity;
			on_bounce(glm::vec3(0.0f, 0.0f, -1.0f));
		} else {
			z_dist = vel.z;
		}
		pos.z += z_dist;
	} else if (vel.z < 0) {
		z_dist = -world->ground->get_wall_dist(pos, glm::vec2( 0.0f, -1.0f),
						       radius, height);
		if (vel.z < z_dist) {
			vel.z = -vel.z * elasticity;
			on_bounce(glm::vec3(0.0f, 0.0f, 1.0f));
		} else {
			z_dist = vel.z;
		}
		pos.z += z_dist;
	}

}

/**
 * Update the vertical position based on the location of the ground and ceiling
 * @param double t, the time that has passed since the last update
 */
void physics_object::gravity(double t)
{
	// If the object is in a block, push it out
	int type = world->ground->get_value(pos);
	if (type > 0) {
		vel.y = in_block_vel;
		in_block(type);
		return;
	}

	// If on the ground, do nothing
	double grnd_dist = world->ground->get_ground_dist(pos, radius);
	if (grnd_dist < 0.0001 && grnd_dist > -0.0001 && vel.y == 0.0f) {
		vel.y = 0.0f;
		if (midair) {
			midair = false;
			on_fall();
		}
		return;
	}

	// Call on_fall
	if (!midair) {
		midair = true;
		on_fall();
	}

	double ceil_dist = world->ground->get_ceiling_dist(glm::vec3(pos.x,
								     pos.y + height,
								     pos.z),
							   radius);
	// Calculate the change in position
	float y_max = 0.5f * GRAVITY_ACCEL * (float)t * (float)t +
		vel.y * (float)t;

	// Update the position and velocity based on ground and ceiling distance
	if (y_max <= -grnd_dist) { // If near the ground, place self on the ground
		vel.y = -vel.y * elasticity;
		on_bounce(glm::vec3(0.0f, 1.0f, 0.0f));
		pos.y -= (float)grnd_dist;
		midair = false;
		on_land();
	} else if (y_max >= ceil_dist) { // If close to the ceiling, place self at the ceiling
		vel.y = -vel.y * elasticity;
		on_bounce(glm::vec3(0.0f, -1.0f, 0.0f));
		pos.y += (float)ceil_dist;
	} else { // Update the y position
		vel.y += (float)GRAVITY_ACCEL * t;
		pos.y += y_max;
	}

}
