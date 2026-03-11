#include "bomb.hpp"
#include "world.hpp"
#include "explosion.hpp"
#include "character.hpp"
#include "vertex_arrays.hpp"
#include "GL/glew.h"

bomb::~bomb()
{
	glDeleteBuffers(2, buffer);
}

void bomb::init()
{
	static const GLfloat vertices[] = VA_SMALL_CUBE_3F_VERTEX_DATA;
	static GLfloat normals[] = VA_CUBE_3F_NORMAL_DATA;

	// Set properties of the bomb
	radius = 0.25;
	height = radius * 2;
	elasticity = .4;
	friction_coeff = .7; // I don't know if this actually does anything

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

void bomb::update(double time)
{
	// Calculate the new position
	wall_collision(time);
	gravity(time);

	// Update the timer
	timer -= time;
	if (timer <= 0) {
		explode();
		return;
	}
}

void bomb::draw(double time)
{
	GLfloat color[] = {0.0f, 0.0f, 1.0f}; //Blue

	// Designate vertex attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	// Set the color
	glDisableVertexAttribArray(1);
	glVertexAttrib3fv(1, color);

	// Designate normal attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

	// Draw the bomb's cube
	glDrawArrays(GL_TRIANGLES, 0, VA_CUBE_3F_VERTEX_COUNT);

}


void bomb::on_bounce(glm::vec3 dir)
{
	// Decrease the velocity which each bounce to simulate friction
	dir = glm::normalize(dir);
	glm::vec3 para = glm::dot(vel, dir) * dir;	// Get velocity perpendicular to the bounce
	glm::vec3 perp = vel - para;
	vel = para + perp * bounce_fric;
}

/**
 * Make the bomb go off, creating an explosion at its center
 */
void bomb::explode()
{
	if(exploded)
		return;

	// Give owner back a bomb, create an explosion, and delete the bomb
	exploded = true;
	owner->bombs += 1;
	world->add_object(
		new explosion(pos +glm::vec3(0.0f, height / 2.0f, 0.0f),
			      (float)power, .25f, .1f, (float)blast_radius));
	world->del_object(this);
}
