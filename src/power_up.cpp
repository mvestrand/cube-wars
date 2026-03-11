#include "power_up.hpp"
#include "world.hpp"
#include "character.hpp"

#define SPEED_BOOST .01
#define POWER_BOOST 0.66
#define MAX_BOMB_BOOST 1

void power_up::init()
{
	static const GLfloat vertices[] = VA_UNIT_DIAMOND_3F_VERTEX_DATA;
	static GLfloat normals[] = VA_DIAMOND_3F_NORMAL_DATA;

	// Set power-up attributes
	radius = .3;

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

void power_up::draw(double time)
{
	// Set the color based on the type of power-up
	static GLfloat color[] = {1.0f, 1.0f, 1.0f};
	switch(type) {
	case SPEED_PWR_UP:
		color[0] = 0.0f;
		color[1] = 1.0f;
		color[2] = 0.0f;
		break;
	case MAX_BOMB_PWR_UP:
		color[0] = 0.0f;
		color[1] = 0.0f;
		color[2] = 1.0f;
		break;
	case BLAST_RAD_PWR_UP:
		color[0] = 1.0f;
		color[1] = 0.0f;
		color[2] = 0.0f;
		break;
	case SHIELD_PWR_UP:
		color[0] = 0.0f;
		color[1] = 1.0f;
		color[2] = 1.0f;
		break;
	}
	// Designate vertex attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	// Set the color attribute
	glDisableVertexAttribArray(1);
	glVertexAttrib3fv(1, color);

	// Designate normal attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

	// Draw diamond
	glDrawArrays(GL_TRIANGLES, 0, VA_DIAMOND_3F_VERTEX_COUNT);
}

void power_up::update(double time)
{
	// Update timer
	if (timer <= 0) {
		destroy();
		return;
	}
	timer -= time;

	// Calculate collisions
	wall_collision(time);
	gravity(time);
	collision_sphere(pos + glm::vec3(0.0f, height/2.0f, 0.0f), radius, 1);

	// Make the power-up spin
	rot = glm::angleAxis((float)glm::radians(PWR_UP_SPIN_SPEED * time),
			     glm::vec3(0.0f, 1.0f, 0.0f)) * rot;

	// Make the power-up pulsate; faster if it is about to disappear
	float t = timer/lifespan;
	if (t > .3)
		t = 1;
	scale = 0.3 * (1-PWR_UP_PULSE_FRAC/2) +
		0.3 * PWR_UP_PULSE_FRAC
		* glm::sin((float)((PWR_UP_PULSE_SLOW * t +
				    PWR_UP_PULSE_FAST * (1 - t))
				   * (lifespan - timer)));


	// Destroy self if falling off the edge
	if (world->get_death_height() > pos.y)
		destroy();
}

void power_up::on_collision(game_object *obj, int msg)
{
	// If colliding with a character, power-up the character
	character *chara = dynamic_cast<character *>(obj);
	if (chara != NULL) {
		upgrade(chara);
	}
}

void power_up::upgrade(character *chara)
{
	// Handle exceptional cases
	if (used)
		return;
	if (!chara->is_alive())
		return;

	// Give the character a power-up based on type and destroy self
	switch(type) {
	case SPEED_PWR_UP:
		printf("Speed +1\n");
		chara->accel = ((chara->max_speed + SPEED_BOOST)/chara->max_speed) * chara->accel;
		chara->max_speed += SPEED_BOOST;
		break;
	case MAX_BOMB_PWR_UP:
		printf("Max bombs +1\n");
		chara->bombs += MAX_BOMB_BOOST;
		break;
	case BLAST_RAD_PWR_UP:
		printf("Power +1\n");
		chara->power += POWER_BOOST;
		break;
	case SHIELD_PWR_UP:
		printf("Shield\n");
		chara->shield = 2;
		break;
	}
	destroy();
}

/**
 * Remove this power-up from the world
 */
void power_up::destroy()
{
	world->del_object(this);
	used = true;
}
