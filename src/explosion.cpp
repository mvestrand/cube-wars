#include "explosion.hpp"
#include "world.hpp"
#include "character.hpp"
#include "bomb.hpp"
#include "vertex_arrays.hpp"
#include "terrain.hpp"
#include "GL/glew.h"

void explosion::init()
{
	static const GLfloat vertices[] = VA_UNIT_DIAMOND_3F_VERTEX_DATA;
	static GLfloat normals[] = VA_DIAMOND_3F_NORMAL_DATA;

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

void explosion::update(double time)
{
	if (timer < time) {
		time = timer;
		timer = 0;
	} else
		timer -= time;

	radius += time * expansion_rate;
	scale = radius;

	collision_sphere(pos, radius, 1);
	world->ground->handle_explosion(pos, radius, this);

	if (timer == 0) {
		world->del_object(this);
		return;
	}
}

void explosion::draw(double time)
{
	GLfloat color[] = {1.0f, 0.6f, 0.0f};

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

	// Draw cube
	glDrawArrays(GL_TRIANGLES, 0, VA_DIAMOND_3F_VERTEX_COUNT);

}

void explosion::on_collision(game_object *obj, int msg)
{
	// Only affect objects one time
	std::list<game_object *>::iterator i;
	if ((i = find(affected.begin(), affected.end(), obj)) == affected.end()) {
		affected.push_back(obj);
	} else
		return;

	// If the object is a character, damage it
	character *chara = dynamic_cast<character *>(obj);
	if (chara != NULL) {
		chara->damage(damage);
		return;
	}

	// If the character is a bomb, make it explode
	bomb *bmb = dynamic_cast<bomb *>(obj);
	if (bmb != NULL) {
		bmb->explode();
		return;
	}
}

/**
 * Return whether or not the explosion has damaged a block yet
 * @param int x, the x location in the terrain grid
 * @param int y, the y location in the terrain grid
 * @param int z, the z location in the terrain grid
 * @bool, true if the explosion has damaged the given block already
 */
bool explosion::has_damaged_block(int x, int y, int z)
{
	if (damaged_blocks.empty())
		return false;

	std::list<glm::vec3>::iterator it;
	for (it = damaged_blocks.begin(); it != damaged_blocks.end(); it++) {
		if ((*it).x == x && (*it).y == y && (*it).z == z)
			return true;
	}
	return false;
}

/**
 * Notify the explosion that it has damaged a block
 * @param int x, the x location of the block
 * @param int y, the y location of the block
 * @param int z, the z location of the block
 */
void explosion::damaged_block(int x, int y, int z)
{
	damaged_blocks.push_back(glm::vec3(x, y, z));
}
