#include "character.hpp"
#include "bomb.hpp"
#include "vertex_arrays.hpp"
#include "world.hpp"

void character::init()
{
	static const GLfloat vertices[] = VA_WEDGE_3F_VERTEX_DATA;
	static GLfloat normals[] = VA_CUBE_3F_NORMAL_DATA;

	radius = 0.25;
	height = radius * 2;
	should_track = true;

	glGenBuffers(2, buffer);

	// Designate vertex attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	// Designate normal attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);
}

void character::update(double time)
{

	if (throw_charge > 0) {
		throw_charge += (1-MIN_THROW_CHARGE) * time / THROW_CHARGE_TIME;
		if (throw_charge > 1)
			throw_charge = 1;
	}

	// Calculate collisions
	wall_collision(time);
	gravity(time);
	collision_sphere(pos + glm::vec3(0.0f, height/2.0f, 0.0f), radius, 1);

	rot = glm::angleAxis((float)glm::radians(facing_dir), glm::vec3(0.0f, 1.0f, 0.0f));

	if (world->get_death_height() > pos.y)
		die();
}

void character::draw(double time)
{
	GLfloat draw_color[3];
	// Make the color darker if the character is dead
	if (!is_alive()) {
		draw_color[0] = .3 * color[0];
		draw_color[1] = .3 * color[1];
		draw_color[2] = .3 * color[2];
	} else {
		draw_color[0] = color[0];
		draw_color[1] = color[1];
		draw_color[2] = color[2];
	}

	// Designate vertex attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	// Set color attribute
	glDisableVertexAttribArray(1);
	glVertexAttrib3fv(1, draw_color);

	// Designate normal attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

	// Draw character
	glDrawArrays(GL_TRIANGLES, 0, VA_CUBE_3F_VERTEX_COUNT);

}

/**
 * Convert a two dimensional vector to a counter-clockwise angle
 * @param vec2 dir, the directional vector; y should point along the -z axis
 * @return double, the angle of the direction vector
 */
double vec2_to_degrees(glm::vec2 dir)
{
	dir.y = -dir.y; // Turn counter clockwise

	// Handle edge cases
	if (dir.x == 0.0f) {
		if (dir.y > 0)
			return 90.0;
		else
			return 270.0;
	} if (dir.y == 0.0f) {
		if (dir.x > 0)
			return 0.0;
		else
			return 180.0;
	}

	if (dir.x >= 0.0f)
		return glm::mod((double)glm::degrees(glm::atan(dir.y / dir.x)), 360.0);
	else
		return glm::degrees(glm::atan(dir.y / dir.x)) + 180.0;
}

/**
 * Convert a counter-clockwise angle to a two dimensional vector
 * @param double dir, the angle to be converted
 * @return vec2, the angle as a 2d vector; y points along the -z axis
 */
glm::vec2 degrees_to_vec2(double dir)
{
	dir = glm::radians(dir);
	return glm::vec2((float)glm::cos(dir), -(float)glm::sin(dir));
}

/**
 * Make the character turn towards a direction
 * @param vec2 dir, the direction to turn towards
 * @param double t, the change in time
 */
void character::turn_towards(glm::vec2 dir, double t)
{
	if (glm::length(dir) > 0) {
		double angle_chng;
		if (glm::length(control_vel) > 0)
			angle_chng = (1-turn_ctrl) * turn_speed * t
				+ (turn_ctrl/2.0) * turn_speed * t * glm::dot(glm::normalize(dir),
									      glm::normalize(control_vel));
		else
			angle_chng = (1-turn_ctrl) * static_turn_speed * t
				+ (turn_ctrl/2.0) * static_turn_speed * t;
		double target_angle = vec2_to_degrees(dir);

		if (facing_dir > target_angle) {
			if (facing_dir - target_angle > 180) {
				if (360 - (facing_dir - target_angle) < angle_chng)
					facing_dir = target_angle;
				else
					facing_dir += angle_chng;
			} else {
				if (facing_dir - target_angle < angle_chng)
					facing_dir = target_angle;
				else
					facing_dir -= angle_chng;
			}
		} else {
			if (target_angle - facing_dir > 180) {
				if (360 - (target_angle - facing_dir) < angle_chng)
					facing_dir = target_angle;
				else
					facing_dir -= angle_chng;
			} else {
				if (target_angle - facing_dir < angle_chng)
					facing_dir = target_angle;
				else
					facing_dir += angle_chng;
			}
		}
	}

	facing_dir = glm::mod(facing_dir, 360.0);
}

/**
 * Set the character velocity based on the direction vector given
 * @param double vec2 dir, the direction to try to move towards
 * @param double t, the change in time
 */
void character::move_dir(glm::vec2 dir, double t)
{
	if (!is_alive()) {
		dir = glm::vec2(0.0f, 0.0f);
	} else if (throw_charge > 0) {
		turn_towards(dir, t);
		dir = glm::vec2(0.0f, 0.0f);
	} else {
		turn_towards(dir, t);
	}

	// Calculate how much the controls should affect velocity
	if (glm::length(dir) > 0) {
		// Accelerate in the given direction
		dir = glm::normalize(dir);
		control_vel += (float)accel * (float)t * dir;
		if (glm::length(control_vel) > max_speed) {
			control_vel = (float)max_speed * glm::normalize(control_vel);
		}
	} else {
		// No control input; decelerate
		if (glm::length(control_vel) <= accel * t) {
			control_vel = glm::vec2(0.0f, 0.0f);
		} else {
			control_vel += (float)accel * (float)t * glm::normalize(-control_vel);
		}
	}


	// Calculate the new velocity in the x-z plane
	if (!midair) {
		vel.x = (float)control_vel.x;
		vel.z = (float)control_vel.y;
	} else {
		vel.x = (float)((1 - midair_control) * fall_vel.x +
			 midair_control *control_vel.x);
		vel.z = (float)((1 - midair_control) * fall_vel.y +
			 midair_control * control_vel.y);

	}
}

void character::on_fall()
{
	// Set the fall velocity and number of midair jumps
	fall_vel = glm::vec2(vel.x, vel.z);
	midair_jumps = max_midair_jumps;
}

/**
 * Make the character jump if they can
 * @param double charge, how charged up the jump was; should be between 0 and 1
 */
void character::jump(double charge)
{
	if (!is_alive())
		return;

	if (on_ground()) {
		vel.y += charge * jump_vel;
	} else if (midair_jumps > 0) {
		midair_jumps--;
		fall_vel = glm::vec2(vel.x, vel.z);
		if (vel.y < jump_vel) {
			vel.y = glm::min(charge * jump_vel + vel.y, charge * jump_vel);

		}
	}
}

/**
 * Make the character throw a bomb
 * @param int type, the type of bomb to throw (not used for anything right now)
 */
void character::throw_bomb(int type)
{
	// Check that a bomb can be thrown
	if (bombs <= 0 || throw_charge == 0)
		return;

	bombs--;

	// Create a bomb at the character; set velocity based on
	// facing and how charged up the throw was
	glm::vec2 dir = degrees_to_vec2(facing_dir);
	bomb *b = new bomb;
	b->owner = this;
	b->blast_radius *= power;
	b->pos.x = pos.x;
	b->pos.y = pos.y + 2.0f * height / 3.0f;
	b->pos.z = pos.z;
	b->vel.x = dir.x * throw_charge * throw_speed;
	b->vel.z = dir.y * throw_charge * throw_speed;
	world->add_object(b);
	throw_charge = 0;
}
