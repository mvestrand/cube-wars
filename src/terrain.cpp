#include "terrain.hpp"
#include <algorithm>
#include "explosion.hpp"
#include "world.hpp"
#include "power_up.hpp"
#include <stdlib.h>
#include <time.h>


#define MIN_VERTICES_FOR_DAMAGE   3
#define CHANCE_OF_POWER_UP      0.5

int terrain::test_vertex(glm::vec3 pos, float radius, int x, int y, int z)
{
	glm::vec3 block_pos = glm::vec3((float)(x + x_offset), (float)(y + y_offset), (float)(z + z_offset));
	if (glm::length(pos - block_pos) < radius)
		return 1;
	return 0;
}

void terrain::handle_explosion(glm::vec3 pos, float radius, explosion *exp)
{
	int x_low  = pos.x - x_offset - radius;
	int x_high = pos.x - x_offset + radius;
	int y_low  = pos.y - y_offset - radius;
	int y_high = pos.y - y_offset + radius;
	int z_low  = pos.z - z_offset - radius;
	int z_high = pos.z - z_offset + radius;

	int x, y, z;
	for (z = z_low; z <= z_high; z++) {
		for (y = y_low; y <= y_high; y++) {
			for (x = x_low; x <= x_high; x++) {
				if (exp->has_damaged_block(x, y, z))
					continue;
				int count = 0;
				count += test_vertex(pos, radius,   x,   y,   z);
				count += test_vertex(pos, radius, x+1,   y,   z);
				count += test_vertex(pos, radius,   x, y+1,   z);
				count += test_vertex(pos, radius,   x,   y, z+1);
				count += test_vertex(pos, radius, x+1, y+1,   z);
				count += test_vertex(pos, radius,   x, y+1, z+1);
				count += test_vertex(pos, radius, x+1,   y, z+1);
				count += test_vertex(pos, radius, x+1, y+1, z+1);

				if (count >= MIN_VERTICES_FOR_DAMAGE) {
					damage(x, y, z);
					exp->damaged_block(x, y, z);
				}
			}
		}
	}

}



void terrain::create_pow_at(int val, int x, int y, int z)
{
	if (((double)rand() / (double)RAND_MAX) > CHANCE_OF_POWER_UP) {
		return;
	}

	power_up *pow_up = new power_up();
	pow_up->pos = glm::vec3((float)(x + x_offset + 0.5),
			     (float)(y + y_offset + 0.5),
			     (float)(z + z_offset + 0.5));
	game_world->add_object(pow_up);
}

void terrain::damage(int x, int y, int z)
{
	int value = get_value(x, y, z);
	switch(value) {
	case TERRA_TYPE_AIR:
	case TERRA_TYPE_UNBREAKABLE:
		return;
	case TERRA_TYPE_LOOSE_DIRT:
		set_value(TERRA_TYPE_AIR, x, y, z);
		create_pow_at(TERRA_TYPE_LOOSE_DIRT, x, y, z);
		break;
	case TERRA_TYPE_DIRT:
	case TERRA_TYPE_GRASS:
		set_value(TERRA_TYPE_LOOSE_DIRT, x, y, z);
		break;
	case TERRA_TYPE_GRAVEL:
		set_value(TERRA_TYPE_AIR, x, y, z);
		create_pow_at(TERRA_TYPE_GRAVEL, x, y, z);
		break;
	case TERRA_TYPE_CRACKED_ROCK:
		set_value(TERRA_TYPE_GRAVEL, x, y, z);
		break;
	case TERRA_TYPE_ROCK:
		set_value(TERRA_TYPE_CRACKED_ROCK, x, y, z);
		break;
	case TERRA_TYPE_WOOD:
		set_value(TERRA_TYPE_AIR, x, y, z);
		create_pow_at(TERRA_TYPE_WOOD, x, y, z);
		break;
	}
}

terrain::terrain(world *wrld, int x, int y, int z)
{
	srand(time(NULL));
	game_world = wrld;
	x_offset = -x/2;
	y_offset = 0;
	z_offset = -z/2;
	x_max = x;
	y_max = y;
	z_max = z;
	data = (int *)calloc(x_max * y_max * z_max, sizeof(int));
	if (!data) {
		perror("Failed to allocated memory for terrain\n");
		exit(1);
		if (vertex_count > MAX_VERTICES) {
			fprintf(stderr, "Too many vertices for terrain object\n");
			exit(1);
		}
	}
	need_update = true;
	vertex_count = 0;
	glGenBuffers(3, buffer);
	fprintf(stderr, "%d x %d x %d terrain created\n", x, y, z);
}

terrain::terrain(world *wrld, int x, int y, int z, float x_off, float y_off, float z_off)
{
	srand(time(NULL));
	game_world = wrld;
	x_offset = x_off;
	y_offset = y_off;
	z_offset = z_off;
	x_max = x;
	y_max = y;
	z_max = z;
	data = (int *)calloc(x_max * y_max * z_max, sizeof(int));
	if (!data) {
		perror("Failed to allocated memory for terrain\n");
		exit(1);
		if (vertex_count > MAX_VERTICES) {
			fprintf(stderr, "Too many vertices for terrain object\n");
			exit(1);
		}

	}
	need_update = true;
	vertex_count = 0;
	glGenBuffers(3, buffer);
	fprintf(stderr, "%d x %d x %d terrain created\n", x, y, z);
}

terrain::~terrain()
{
	free(data);
	glDeleteBuffers(3, buffer);
}

int terrain::get_x_max() {return x_max;}
int terrain::get_y_max() {return y_max;}
int terrain::get_z_max() {return z_max;}

int terrain::get_value(glm::vec3 pos)
{
	return get_value((int)(pos.x - x_offset), (int)(pos.y - y_offset), (int)(pos.z - z_offset));
}

glm::vec3 terrain::get_nth_location(int value, int n)
{
	int x, y, z;
	// Test each block
	for (z = 0; z < z_max; z++) {
		for (y = 0; y < y_max; y++) {
			for (x = 0; x < x_max; x++) {
				if (value == get_value(x, y, z)) {
					if (n-- == 1)
						return glm::vec3((float)(x+x_offset), (float)(y+y_offset), (float)(z+z_offset));
				}
			}
		}
	}

	return glm::vec3(0.0f, 0.0f, 0.0f);
}

glm::vec3 terrain::get_start_location(int player)
{
	glm::vec3 loc;
	if ((loc = get_nth_location(TERRA_TYPE_START_POINT, player)) != glm::vec3(0.0f, 0.0f, 0.0f)) {
		return loc + glm::vec3(0.5f, 2.0f, 0.5f);
	}


	return glm::vec3(0.0f, 5.0f, 0.0f);
}

double terrain::get_ceiling_dist(glm::vec3 pos, float radius)
{
	double tl = get_ceiling_dist(pos + glm::vec3(-radius, 0.0f,  radius));
	double tr = get_ceiling_dist(pos + glm::vec3( radius, 0.0f,  radius));
	double bl = get_ceiling_dist(pos + glm::vec3(-radius, 0.0f, -radius));
	double br = get_ceiling_dist(pos + glm::vec3( radius, 0.0f, -radius));

	return std::min(std::min(tl, tr), std::min(bl, br));
}

/**
 * Get the distance to the ground in a square around the given position
 * @param glm::vec3 pos, the position vector
 * @param float radius, half the side length of the square; must be less than .5
 */
double terrain::get_ground_dist(glm::vec3 pos, float radius)
{
	double tl = get_ground_dist(pos + glm::vec3(-radius, 0.0f,  radius));
	double tr = get_ground_dist(pos + glm::vec3( radius, 0.0f,  radius));
	double bl = get_ground_dist(pos + glm::vec3(-radius, 0.0f, -radius));
	double br = get_ground_dist(pos + glm::vec3( radius, 0.0f, -radius));

	return std::min(std::min(tl, tr), std::min(bl, br));
}

double terrain::get_wall_dist(glm::vec3 pos, glm::vec2 dir, float radius, float height)
{
	double c1 = get_wall_dist(pos + glm::vec3(-radius,   0.0f,  radius), dir);
	double c2 = get_wall_dist(pos + glm::vec3( radius,   0.0f,  radius), dir);
	double c3 = get_wall_dist(pos + glm::vec3( radius,   0.0f, -radius), dir);
	double c4 = get_wall_dist(pos + glm::vec3(-radius,   0.0f, -radius), dir);
	double c5 = get_wall_dist(pos + glm::vec3(-radius, height * 1.0f,  radius), dir);
	double c6 = get_wall_dist(pos + glm::vec3( radius, height * 1.0f,  radius), dir);
	double c7 = get_wall_dist(pos + glm::vec3( radius, height * 1.0f, -radius), dir);
	double c8 = get_wall_dist(pos + glm::vec3(-radius, height * 1.0f, -radius), dir);

	c1 = std::min(std::min(c1, c2), std::min(c3, c4));
	c2 = std::min(std::min(c5, c6), std::min(c7, c8));

	return std::min(c1, c2);
}

float min_in_vec(glm::vec3 v)
{
	return (float)std::min(v.z, std::min(v.x, v.y));
}

float max_in_vec(glm::vec3 v)
{
	return (float)std::max(v.z, std::max(v.x, v.y));
}

bool between_vecs(glm::vec3 v, glm::vec3 a, glm::vec3 b)
{
	return v.x >= std::min(a.x, b.x) && v.x <= std::max(a.x, b.x) &&
		v.y >= std::min(a.y, b.y) && v.y <= std::max(a.y, b.y) &&
		v.z >= std::min(a.z, b.z) && v.z <= std::max(a.z, b.z);
}

double terrain::get_ceiling_dist(glm::vec3 pos)
{
	int x = (int)(pos.x - x_offset);
	int y = (int)(pos.y - y_offset);
	int z = (int)(pos.z - z_offset);

	if (get_value(x, y++, z) > 0)  // Handle case of being in a block
		return 0;

	while (y < y_max) {
		if (get_value(x, y, z) > 0) {
			return (double)y + y_offset - pos.y - 0.001;
		}

		y++;
	}

	return 10000000000; // A large number
}

double terrain::get_ground_dist(glm::vec3 pos)
{
	int x = (int)(pos.x - x_offset);
	int y = (int)(pos.y - y_offset) - 1;
	int z = (int)(pos.z - z_offset);

	while (y >= 0) {
		if (get_value(x, y, z) > 0) {
			return (pos.y - y_offset - (double)y - 1);
		}

		y--;
	}

	return 10000000000; // A large number
}

/**
 * Get the distance to a wall from a pos
 * glm::vec3 pos, the position to start from
 * glm::vec2 dir, the direction to look, must point north, east, south, or west
 */
double terrain::get_wall_dist(glm::vec3 pos, glm::vec2 dir)
{
	int x = (int)(pos.x - x_offset);
	int y = (int)(pos.y - y_offset);
	int z = (int)(pos.z - z_offset);

	if (dir.x > 0) {
		while (x < x_max) {
			if (get_value(x, y, z) > 0) {
				return -(pos.x - x_offset - (double)x) - 0.001f;
			}
			x++;
		}
	} else if (dir.x < 0) {
		while (x >= 0) {
			if (get_value(x, y, z) > 0) {
				return (pos.x - x_offset - (double)x - 1) - 0.001f;
			}

			x--;
		}
	} else if (dir.y > 0) {
		while (z < z_max) {
			if (get_value(x, y, z) > 0) {
				return -(pos.z - z_offset - (double)z) - 0.001f;
			}
			z++;
		}
	} else if (dir.y < 0) {
		while (z >= 0) {
			if (get_value(x, y, z) > 0) {
				return (pos.z - z_offset - (double)z - 1) - 0.001f;
			}
			z--;
		}
	}

	return 10000000000; // A large number
}

// double terrain::collision_test(glm::vec3 init_pos, glm::vec3 final_pos, glm::vec3 *col_pos, glm::vec3 *col_normal)
// {
// 	glm::vec3 pos = init_pos;
// 	*col_pos = glm::vec3(0.0f, 0.0f, 0.0f);
// 	*col_normal = glm::vec3(0.0f, 0.0f, 0.0f);
// 	if (init_pos == final_pos)
// 		return 2.0; // Larger than 1

// 	int x = (int)(init_pos.x - x_offset);
// 	int y = (int)(init_pos.y - y_offset);
// 	int z = (int)(init_pos.z - z_offset);

// 	glm::vec3 r = (final_pos - init_pos) / glm::abs(max_in_vec(final_pos - init_pos));
// 	float dx, dy, dz;

// 	while (between_vecs(pos, init_pos, final_pos)) {
// 		if (r.x > 0) {
// 			dx = (1 - glm::mod(pos.x, 1)) / r.x;
// 		} else if (r.x < 0) {
// 			dx = (- glm::mod(pos.x, 1)) / r.x;
// 		} else {
// 			dx = 2.0; //Larger than 1
// 		}
// 		if (r.y > 0) {
// 			dy = (1 - glm::mod(pos.y, 1)) / r.y;
// 		} else if (r.y < 0) {
// 			dy = (- glm::mod(pos.y, 1)) / r.y;
// 		} else {
// 			dy = 2.0; //Larger than 1
// 		}
// 		if (r.z > 0) {
// 			dz = (1 - glm::mod(pos.z, 1)) / r.z;
// 		} else if (r.z < 0) {
// 			dz = (- glm::mod(pos.z, 1)) / r.z;
// 		} else {
// 			dz = 2.0; //Larger than 1
// 		}

// 		if (dx <= dy && dx <= dz) {

// 		}
// 		if (dy <= dx && dy <= dz) {

// 		}
// 		if (dy <= dx && dy <= )
// 	}



// 	return 2.0;
// }

void terrain::draw()
{
	if (need_update) {
		update_model();
		need_update = false;
	}

	// Bind vertex attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	// Bind color attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	// Bind normal attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

	if (vertex_count > 0)
		glDrawArrays(GL_TRIANGLES, 0, vertex_count);

}

// Update the model used by the terrain object
void terrain::update_model()
{
// 	fprintf(stderr, "Updating terrain model\n");

	int x, y, z;
	int val;
	vertex_count = 0;
	// Test each block that is not empty
	for (z = 0; z < z_max; z++) {
		for (y = 0; y < y_max; y++) {
			for (x = 0; x < x_max; x++) {
				if ((val = get_value(x, y, z))) {

					// Test if each of the adjacent blocks is empty
					if (!get_value(x+1, y, z))
						write_face(X_PLUS, val, x + x_offset,
							   y + y_offset, z + z_offset);
					if (!get_value(x-1, y, z))
						write_face(X_MINUS, val, x + x_offset,
							   y + y_offset, z + z_offset);
					if (!get_value(x, y+1, z))
						write_face(Y_PLUS, val, x + x_offset,
							   y + y_offset, z + z_offset);
					if (!get_value(x, y-1, z))
						write_face(Y_MINUS, val, x + x_offset,
							   y + y_offset, z + z_offset);
					if (!get_value(x, y, z+1))
						write_face(Z_PLUS, val, x + x_offset,
							   y + y_offset, z + z_offset);
					if (!get_value(x, y, z-1))
						write_face(Z_MINUS, val, x + x_offset,
							   y + y_offset, z + z_offset);
				}
			}
		}
	}

	// Bind vertex attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*vertex_count, vertex_buffer, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	// Bind color attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*vertex_count, color_buffer, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	// Bind normal attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*vertex_count, normal_buffer, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

// 	fprintf(stderr, "Terrain model updated\n");
}

// Load a terrain file at the origin
void terrain::load(const char *filename)
{
	load_at(filename, 0.0f, 0.0f, 0.0f);
}


// Load a terrain file at the given point
void terrain::load_at(const char *filename, float x_init, float y_init, float z_init)
{
	fprintf(stderr, "Opening file \"%s\"\n", filename);
	char c;
	FILE *file_ptr = fopen(filename, "r");
	if (!file_ptr) {
		perror("Error opening terrain file\n");
		exit(1);
	}
	int x = x_init, y = y_init, z = z_init;


	do {
		c = getc(file_ptr);
		switch(c) {
		case '0':
			if (x >= x_max || y >= y_max || z >= z_max) {
				break;
			}
			data[x + y * x_max + z * x_max * y_max] = 0;
			x++;
			break;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if (x >= x_max || y >= y_max || z >= z_max) {
				break;
			}
			data[x + y * x_max + z * x_max * y_max] = c - '0';
			x++;
			break;
		case '\n':
			x = x_init;
			z++;
			break;
		case 'n':
			c = getc(file_ptr);
			x = x_init;
			z = z_init;
			y++;
			break;
		}
	}
	while (c != EOF);

	fclose(file_ptr);

	fprintf(stderr, "File loaded\n");
}

void terrain::clear(int val)
{
	int i;
	for (i = 0; i < x_max * y_max * z_max; i++) {
		data[i] = val;
	}
}

int terrain::get_value(int x, int y, int z)
{
	// Handle out of bounds error
	if (x >= x_max || y >= y_max || z >= z_max)
		return 0;
	if (x < 0 || y < 0 || z < 0)
		return 0;

	return data[x + y * x_max + z * x_max * y_max];
}

int terrain::set_value(int value, int x, int y, int z)
{
	// Handle out of bounds error
	if (x >= x_max || y >= y_max || z >= z_max)
		return 0;
	if (x < 0 || y < 0 || z < 0)
		return 0;

	// Get the old value
	int old_val = data[x + y * x_max + z * x_max * y_max];
	data[x + y * x_max + z * x_max * y_max] = value;

	// Check if update is necessary
	if (old_val != value)
		need_update = true;
	return old_val;
}


void terrain::write_face(cube_face dir, int value, float x, float y, float z)
{
	GLfloat color[] = {0.0f, 0.0f, 0.0f};

	switch (value) {
	case TERRA_TYPE_LOOSE_DIRT:
		color[0] = 0.2f;
		color[1] = 0.15f;
		color[2] = 0.1f;
		break;
	case TERRA_TYPE_DIRT:
		color[0] = 0.4f;
		color[1] = 0.3f;
		color[2] = 0.2f;
		break;
	case TERRA_TYPE_GRASS:
		color[0] = 0.2f;
		color[1] = 0.7f;
		color[2] = 0.2f;
		break;
	case TERRA_TYPE_GRAVEL:
		color[0] = 0.3f;
		color[1] = 0.3f;
		color[2] = 0.3f;
		break;
	case TERRA_TYPE_CRACKED_ROCK:
		color[0] = 0.5f;
		color[1] = 0.5f;
		color[2] = 0.5f;
		break;
	case TERRA_TYPE_ROCK:
		color[0] = 0.7f;
		color[1] = 0.7f;
		color[2] = 0.7f;
		break;
	case TERRA_TYPE_UNBREAKABLE:
		color[0] = 0.9f;
		color[1] = 0.7f;
		color[2] = 0.5f;
		break;
	case TERRA_TYPE_WOOD:
		color[0] = 0.6f;
		color[1] = 0.5f;
		color[2] = 0.4f;
		break;
	case TERRA_TYPE_START_POINT:
		color[0] = 0.4f;
		color[1] = 0.7f;
		color[2] = 0.8f;
		break;
	}

	if (vertex_count + 6 > MAX_VERTICES) {
		fprintf(stderr, "Too many vertices for terrain object\n");
		return;
	}

	switch (dir) {
	case X_PLUS:
		// Vertices
		vertex_buffer[vertex_count * 3] = x + 1.0f;
		vertex_buffer[vertex_count * 3+1] = y;
		vertex_buffer[vertex_count * 3+2] = z;

		vertex_buffer[vertex_count * 3+3] = x + 1.0f;
		vertex_buffer[vertex_count * 3+4] = y + 1.0f;
		vertex_buffer[vertex_count * 3+5] = z;

		vertex_buffer[vertex_count * 3+6] = x + 1.0f;
		vertex_buffer[vertex_count * 3+7] = y;
		vertex_buffer[vertex_count * 3+8] = z + 1.0f;

		vertex_buffer[vertex_count * 3+9] = x + 1.0f;
		vertex_buffer[vertex_count * 3+10] = y + 1.0f;
		vertex_buffer[vertex_count * 3+11] = z;

		vertex_buffer[vertex_count * 3+12] = x + 1.0f;
		vertex_buffer[vertex_count * 3+13] = y + 1.0f;
		vertex_buffer[vertex_count * 3+14] = z + 1.0f;

		vertex_buffer[vertex_count * 3+15] = x + 1.0f;
		vertex_buffer[vertex_count * 3+16] = y;
		vertex_buffer[vertex_count * 3+17] = z + 1.0f;

		// Colors
		color_buffer[vertex_count * 3]      = color[0];
		color_buffer[vertex_count * 3 +  1] = color[1];
		color_buffer[vertex_count * 3 +  2] = color[2];

		color_buffer[vertex_count * 3 +  3] = color[0];
		color_buffer[vertex_count * 3 +  4] = color[1];
		color_buffer[vertex_count * 3 +  5] = color[2];

		color_buffer[vertex_count * 3 +  6] = color[0];
		color_buffer[vertex_count * 3 +  7] = color[1];
		color_buffer[vertex_count * 3 +  8] = color[2];

		color_buffer[vertex_count * 3 +  9] = color[0];
		color_buffer[vertex_count * 3 + 10] = color[1];
		color_buffer[vertex_count * 3 + 11] = color[2];

		color_buffer[vertex_count * 3 + 12] = color[0];
		color_buffer[vertex_count * 3 + 13] = color[1];
		color_buffer[vertex_count * 3 + 14] = color[2];

		color_buffer[vertex_count * 3 + 15] = color[0];
		color_buffer[vertex_count * 3 + 16] = color[1];
		color_buffer[vertex_count * 3 + 17] = color[2];

		// Normals
		normal_buffer[vertex_count * 3]      = 1.0f;
		normal_buffer[vertex_count * 3 +  1] = 0.0f;
		normal_buffer[vertex_count * 3 +  2] = 0.0f;

		normal_buffer[vertex_count * 3 +  3] = 1.0f;
		normal_buffer[vertex_count * 3 +  4] = 0.0f;
		normal_buffer[vertex_count * 3 +  5] = 0.0f;

		normal_buffer[vertex_count * 3 +  6] = 1.0f;
		normal_buffer[vertex_count * 3 +  7] = 0.0f;
		normal_buffer[vertex_count * 3 +  8] = 0.0f;

		normal_buffer[vertex_count * 3 +  9] = 1.0f;
		normal_buffer[vertex_count * 3 + 10] = 0.0f;
		normal_buffer[vertex_count * 3 + 11] = 0.0f;

		normal_buffer[vertex_count * 3 + 12] = 1.0f;
		normal_buffer[vertex_count * 3 + 13] = 0.0f;
		normal_buffer[vertex_count * 3 + 14] = 0.0f;

		normal_buffer[vertex_count * 3 + 15] = 1.0f;
		normal_buffer[vertex_count * 3 + 16] = 0.0f;
		normal_buffer[vertex_count * 3 + 17] = 0.0f;

		vertex_count += 6;
		break;
	case X_MINUS:
		// Vertices
		vertex_buffer[vertex_count * 3] = x;
		vertex_buffer[vertex_count * 3+1] = y;
		vertex_buffer[vertex_count * 3+2] = z;

		vertex_buffer[vertex_count * 3+3] = x;
		vertex_buffer[vertex_count * 3+4] = y;
		vertex_buffer[vertex_count * 3+5] = z + 1.0f;

		vertex_buffer[vertex_count * 3+6] = x;
		vertex_buffer[vertex_count * 3+7] = y + 1.0f;
		vertex_buffer[vertex_count * 3+8] = z + 1.0f;

		vertex_buffer[vertex_count * 3+9] = x;
		vertex_buffer[vertex_count * 3+10] = y;
		vertex_buffer[vertex_count * 3+11] = z;

		vertex_buffer[vertex_count * 3+12] = x;
		vertex_buffer[vertex_count * 3+13] = y + 1.0f;
		vertex_buffer[vertex_count * 3+14] = z + 1.0f;

		vertex_buffer[vertex_count * 3+15] = x;
		vertex_buffer[vertex_count * 3+16] = y + 1.0f;
		vertex_buffer[vertex_count * 3+17] = z;

		// Colors
		color_buffer[vertex_count * 3]      = color[0];
		color_buffer[vertex_count * 3 +  1] = color[1];
		color_buffer[vertex_count * 3 +  2] = color[2];

		color_buffer[vertex_count * 3 +  3] = color[0];
		color_buffer[vertex_count * 3 +  4] = color[1];
		color_buffer[vertex_count * 3 +  5] = color[2];

		color_buffer[vertex_count * 3 +  6] = color[0];
		color_buffer[vertex_count * 3 +  7] = color[1];
		color_buffer[vertex_count * 3 +  8] = color[2];

		color_buffer[vertex_count * 3 +  9] = color[0];
		color_buffer[vertex_count * 3 + 10] = color[1];
		color_buffer[vertex_count * 3 + 11] = color[2];

		color_buffer[vertex_count * 3 + 12] = color[0];
		color_buffer[vertex_count * 3 + 13] = color[1];
		color_buffer[vertex_count * 3 + 14] = color[2];

		color_buffer[vertex_count * 3 + 15] = color[0];
		color_buffer[vertex_count * 3 + 16] = color[1];
		color_buffer[vertex_count * 3 + 17] = color[2];

		// Normals
		normal_buffer[vertex_count * 3]      = -1.0f;
		normal_buffer[vertex_count * 3 +  1] = 0.0f;
		normal_buffer[vertex_count * 3 +  2] = 0.0f;

		normal_buffer[vertex_count * 3 +  3] = -1.0f;
		normal_buffer[vertex_count * 3 +  4] = 0.0f;
		normal_buffer[vertex_count * 3 +  5] = 0.0f;

		normal_buffer[vertex_count * 3 +  6] = -1.0f;
		normal_buffer[vertex_count * 3 +  7] = 0.0f;
		normal_buffer[vertex_count * 3 +  8] = 0.0f;

		normal_buffer[vertex_count * 3 +  9] = -1.0f;
		normal_buffer[vertex_count * 3 + 10] = 0.0f;
		normal_buffer[vertex_count * 3 + 11] = 0.0f;

		normal_buffer[vertex_count * 3 + 12] = -1.0f;
		normal_buffer[vertex_count * 3 + 13] = 0.0f;
		normal_buffer[vertex_count * 3 + 14] = 0.0f;

		normal_buffer[vertex_count * 3 + 15] = -1.0f;
		normal_buffer[vertex_count * 3 + 16] = 0.0f;
		normal_buffer[vertex_count * 3 + 17] = 0.0f;

		vertex_count += 6;
		break;
	case Z_PLUS:
		// Vertices
		vertex_buffer[vertex_count * 3] = x;
		vertex_buffer[vertex_count * 3+1] = y;
		vertex_buffer[vertex_count * 3+2] = z + 1.0f;

		vertex_buffer[vertex_count * 3+3] = x + 1.0f;
		vertex_buffer[vertex_count * 3+4] = y;
		vertex_buffer[vertex_count * 3+5] = z + 1.0f;

		vertex_buffer[vertex_count * 3+6] = x + 1.0f;
		vertex_buffer[vertex_count * 3+7] = y + 1.0f;
		vertex_buffer[vertex_count * 3+8] = z + 1.0f;

		vertex_buffer[vertex_count * 3+9] = x;
		vertex_buffer[vertex_count * 3+10] = y;
		vertex_buffer[vertex_count * 3+11] = z + 1.0f;

		vertex_buffer[vertex_count * 3+12] = x + 1.0f;
		vertex_buffer[vertex_count * 3+13] = y + 1.0f;
		vertex_buffer[vertex_count * 3+14] = z + 1.0f;

		vertex_buffer[vertex_count * 3+15] = x;
		vertex_buffer[vertex_count * 3+16] = y + 1.0f;
		vertex_buffer[vertex_count * 3+17] = z + 1.0f;

		// Colors
		color_buffer[vertex_count * 3]      = color[0];
		color_buffer[vertex_count * 3 +  1] = color[1];
		color_buffer[vertex_count * 3 +  2] = color[2];

		color_buffer[vertex_count * 3 +  3] = color[0];
		color_buffer[vertex_count * 3 +  4] = color[1];
		color_buffer[vertex_count * 3 +  5] = color[2];

		color_buffer[vertex_count * 3 +  6] = color[0];
		color_buffer[vertex_count * 3 +  7] = color[1];
		color_buffer[vertex_count * 3 +  8] = color[2];

		color_buffer[vertex_count * 3 +  9] = color[0];
		color_buffer[vertex_count * 3 + 10] = color[1];
		color_buffer[vertex_count * 3 + 11] = color[2];

		color_buffer[vertex_count * 3 + 12] = color[0];
		color_buffer[vertex_count * 3 + 13] = color[1];
		color_buffer[vertex_count * 3 + 14] = color[2];

		color_buffer[vertex_count * 3 + 15] = color[0];
		color_buffer[vertex_count * 3 + 16] = color[1];
		color_buffer[vertex_count * 3 + 17] = color[2];

		// Normals
		normal_buffer[vertex_count * 3]      = 0.0f;
		normal_buffer[vertex_count * 3 +  1] = 0.0f;
		normal_buffer[vertex_count * 3 +  2] = 1.0f;

		normal_buffer[vertex_count * 3 +  3] = 0.0f;
		normal_buffer[vertex_count * 3 +  4] = 0.0f;
		normal_buffer[vertex_count * 3 +  5] = 1.0f;

		normal_buffer[vertex_count * 3 +  6] = 0.0f;
		normal_buffer[vertex_count * 3 +  7] = 0.0f;
		normal_buffer[vertex_count * 3 +  8] = 1.0f;

		normal_buffer[vertex_count * 3 +  9] = 0.0f;
		normal_buffer[vertex_count * 3 + 10] = 0.0f;
		normal_buffer[vertex_count * 3 + 11] = 1.0f;

		normal_buffer[vertex_count * 3 + 12] = 0.0f;
		normal_buffer[vertex_count * 3 + 13] = 0.0f;
		normal_buffer[vertex_count * 3 + 14] = 1.0f;

		normal_buffer[vertex_count * 3 + 15] = 0.0f;
		normal_buffer[vertex_count * 3 + 16] = 0.0f;
		normal_buffer[vertex_count * 3 + 17] = 1.0f;

		vertex_count += 6;
		break;
	case Z_MINUS:
		// Vertices
		vertex_buffer[vertex_count * 3] = x + 1.0f;
		vertex_buffer[vertex_count * 3+1] = y;
		vertex_buffer[vertex_count * 3+2] = z;

		vertex_buffer[vertex_count * 3+3] = x;
		vertex_buffer[vertex_count * 3+4] = y;
		vertex_buffer[vertex_count * 3+5] = z;

		vertex_buffer[vertex_count * 3+6] = x;
		vertex_buffer[vertex_count * 3+7] = y + 1.0f;
		vertex_buffer[vertex_count * 3+8] = z;

		vertex_buffer[vertex_count * 3+9] = x + 1.0f;
		vertex_buffer[vertex_count * 3+10] = y;
		vertex_buffer[vertex_count * 3+11] = z;

		vertex_buffer[vertex_count * 3+12] = x;
		vertex_buffer[vertex_count * 3+13] = y + 1.0f;
		vertex_buffer[vertex_count * 3+14] = z;

		vertex_buffer[vertex_count * 3+15] = x + 1.0f;
		vertex_buffer[vertex_count * 3+16] = y + 1.0f;
		vertex_buffer[vertex_count * 3+17] = z;

		// Colors
		color_buffer[vertex_count * 3]      = color[0];
		color_buffer[vertex_count * 3 +  1] = color[1];
		color_buffer[vertex_count * 3 +  2] = color[2];

		color_buffer[vertex_count * 3 +  3] = color[0];
		color_buffer[vertex_count * 3 +  4] = color[1];
		color_buffer[vertex_count * 3 +  5] = color[2];

		color_buffer[vertex_count * 3 +  6] = color[0];
		color_buffer[vertex_count * 3 +  7] = color[1];
		color_buffer[vertex_count * 3 +  8] = color[2];

		color_buffer[vertex_count * 3 +  9] = color[0];
		color_buffer[vertex_count * 3 + 10] = color[1];
		color_buffer[vertex_count * 3 + 11] = color[2];

		color_buffer[vertex_count * 3 + 12] = color[0];
		color_buffer[vertex_count * 3 + 13] = color[1];
		color_buffer[vertex_count * 3 + 14] = color[2];

		color_buffer[vertex_count * 3 + 15] = color[0];
		color_buffer[vertex_count * 3 + 16] = color[1];
		color_buffer[vertex_count * 3 + 17] = color[2];

		// Normals
		normal_buffer[vertex_count * 3]      = 0.0f;
		normal_buffer[vertex_count * 3 +  1] = 0.0f;
		normal_buffer[vertex_count * 3 +  2] = -1.0f;

		normal_buffer[vertex_count * 3 +  3] = 0.0f;
		normal_buffer[vertex_count * 3 +  4] = 0.0f;
		normal_buffer[vertex_count * 3 +  5] = -1.0f;

		normal_buffer[vertex_count * 3 +  6] = 0.0f;
		normal_buffer[vertex_count * 3 +  7] = 0.0f;
		normal_buffer[vertex_count * 3 +  8] = -1.0f;

		normal_buffer[vertex_count * 3 +  9] = 0.0f;
		normal_buffer[vertex_count * 3 + 10] = 0.0f;
		normal_buffer[vertex_count * 3 + 11] = -1.0f;

		normal_buffer[vertex_count * 3 + 12] = 0.0f;
		normal_buffer[vertex_count * 3 + 13] = 0.0f;
		normal_buffer[vertex_count * 3 + 14] = -1.0f;

		normal_buffer[vertex_count * 3 + 15] = 0.0f;
		normal_buffer[vertex_count * 3 + 16] = 0.0f;
		normal_buffer[vertex_count * 3 + 17] = -1.0f;

		vertex_count += 6;
		break;
	case Y_MINUS:
		// Vertices
		vertex_buffer[vertex_count * 3] = x;
		vertex_buffer[vertex_count * 3+1] = y;
		vertex_buffer[vertex_count * 3+2] = z + 1.0f;

		vertex_buffer[vertex_count * 3+3] = x + 1.0f;
		vertex_buffer[vertex_count * 3+4] = y;
		vertex_buffer[vertex_count * 3+5] = z;

		vertex_buffer[vertex_count * 3+6] = x + 1.0f;
		vertex_buffer[vertex_count * 3+7] = y;
		vertex_buffer[vertex_count * 3+8] = z + 1.0f;

		vertex_buffer[vertex_count * 3+9] = x;
		vertex_buffer[vertex_count * 3+10] = y;
		vertex_buffer[vertex_count * 3+11] = z;

		vertex_buffer[vertex_count * 3+12] = x + 1.0f;
		vertex_buffer[vertex_count * 3+13] = y;
		vertex_buffer[vertex_count * 3+14] = z;

		vertex_buffer[vertex_count * 3+15] = x;
		vertex_buffer[vertex_count * 3+16] = y;
		vertex_buffer[vertex_count * 3+17] = z + 1.0f;

		// Colors
		color_buffer[vertex_count * 3]      = color[0];
		color_buffer[vertex_count * 3 +  1] = color[1];
		color_buffer[vertex_count * 3 +  2] = color[2];

		color_buffer[vertex_count * 3 +  3] = color[0];
		color_buffer[vertex_count * 3 +  4] = color[1];
		color_buffer[vertex_count * 3 +  5] = color[2];

		color_buffer[vertex_count * 3 +  6] = color[0];
		color_buffer[vertex_count * 3 +  7] = color[1];
		color_buffer[vertex_count * 3 +  8] = color[2];

		color_buffer[vertex_count * 3 +  9] = color[0];
		color_buffer[vertex_count * 3 + 10] = color[1];
		color_buffer[vertex_count * 3 + 11] = color[2];

		color_buffer[vertex_count * 3 + 12] = color[0];
		color_buffer[vertex_count * 3 + 13] = color[1];
		color_buffer[vertex_count * 3 + 14] = color[2];

		color_buffer[vertex_count * 3 + 15] = color[0];
		color_buffer[vertex_count * 3 + 16] = color[1];
		color_buffer[vertex_count * 3 + 17] = color[2];

		// Normals
		normal_buffer[vertex_count * 3]      = 0.0f;
		normal_buffer[vertex_count * 3 +  1] = -1.0f;
		normal_buffer[vertex_count * 3 +  2] = 0.0f;

		normal_buffer[vertex_count * 3 +  3] = 0.0f;
		normal_buffer[vertex_count * 3 +  4] = -1.0f;
		normal_buffer[vertex_count * 3 +  5] = 0.0f;

		normal_buffer[vertex_count * 3 +  6] = 0.0f;
		normal_buffer[vertex_count * 3 +  7] = -1.0f;
		normal_buffer[vertex_count * 3 +  8] = 0.0f;

		normal_buffer[vertex_count * 3 +  9] = 0.0f;
		normal_buffer[vertex_count * 3 + 10] = -1.0f;
		normal_buffer[vertex_count * 3 + 11] = 0.0f;

		normal_buffer[vertex_count * 3 + 12] = 0.0f;
		normal_buffer[vertex_count * 3 + 13] = -1.0f;
		normal_buffer[vertex_count * 3 + 14] = 0.0f;

		normal_buffer[vertex_count * 3 + 15] = 0.0f;
		normal_buffer[vertex_count * 3 + 16] = -1.0f;
		normal_buffer[vertex_count * 3 + 17] = 0.0f;

		vertex_count += 6;
		break;

	case Y_PLUS:
		if (value == TERRA_TYPE_DIRT) {
			color[0] = 0.2f;
			color[1] = 0.7f;
			color[2] = 0.2f;
		}


		// Vertices
		vertex_buffer[vertex_count * 3] = x + 1.0f;
		vertex_buffer[vertex_count * 3+1] = y + 1.0f;
		vertex_buffer[vertex_count * 3+2] = z;

		vertex_buffer[vertex_count * 3+3] = x;
		vertex_buffer[vertex_count * 3+4] = y + 1.0f;
		vertex_buffer[vertex_count * 3+5] = z + 1.0f;

		vertex_buffer[vertex_count * 3+6] = x + 1.0f;
		vertex_buffer[vertex_count * 3+7] = y + 1.0f;
		vertex_buffer[vertex_count * 3+8] = z + 1.0f;

		vertex_buffer[vertex_count * 3+9] = x + 1.0f;
		vertex_buffer[vertex_count * 3+10] = y + 1.0f;
		vertex_buffer[vertex_count * 3+11] = z;

		vertex_buffer[vertex_count * 3+12] = x;
		vertex_buffer[vertex_count * 3+13] = y + 1.0f;
		vertex_buffer[vertex_count * 3+14] = z;

		vertex_buffer[vertex_count * 3+15] = x;
		vertex_buffer[vertex_count * 3+16] = y + 1.0f;
		vertex_buffer[vertex_count * 3+17] = z + 1.0f;

		// Colors
		color_buffer[vertex_count * 3]      = color[0];
		color_buffer[vertex_count * 3 +  1] = color[1];
		color_buffer[vertex_count * 3 +  2] = color[2];

		color_buffer[vertex_count * 3 +  3] = color[0];
		color_buffer[vertex_count * 3 +  4] = color[1];
		color_buffer[vertex_count * 3 +  5] = color[2];

		color_buffer[vertex_count * 3 +  6] = color[0];
		color_buffer[vertex_count * 3 +  7] = color[1];
		color_buffer[vertex_count * 3 +  8] = color[2];

		color_buffer[vertex_count * 3 +  9] = color[0];
		color_buffer[vertex_count * 3 + 10] = color[1];
		color_buffer[vertex_count * 3 + 11] = color[2];

		color_buffer[vertex_count * 3 + 12] = color[0];
		color_buffer[vertex_count * 3 + 13] = color[1];
		color_buffer[vertex_count * 3 + 14] = color[2];

		color_buffer[vertex_count * 3 + 15] = color[0];
		color_buffer[vertex_count * 3 + 16] = color[1];
		color_buffer[vertex_count * 3 + 17] = color[2];

		// Normals
		normal_buffer[vertex_count * 3]      = 0.0f;
		normal_buffer[vertex_count * 3 +  1] = 1.0f;
		normal_buffer[vertex_count * 3 +  2] = 0.0f;

		normal_buffer[vertex_count * 3 +  3] = 0.0f;
		normal_buffer[vertex_count * 3 +  4] = 1.0f;
		normal_buffer[vertex_count * 3 +  5] = 0.0f;

		normal_buffer[vertex_count * 3 +  6] = 0.0f;
		normal_buffer[vertex_count * 3 +  7] = 1.0f;
		normal_buffer[vertex_count * 3 +  8] = 0.0f;

		normal_buffer[vertex_count * 3 +  9] = 0.0f;
		normal_buffer[vertex_count * 3 + 10] = 1.0f;
		normal_buffer[vertex_count * 3 + 11] = 0.0f;

		normal_buffer[vertex_count * 3 + 12] = 0.0f;
		normal_buffer[vertex_count * 3 + 13] = 1.0f;
		normal_buffer[vertex_count * 3 + 14] = 0.0f;

		normal_buffer[vertex_count * 3 + 15] = 0.0f;
		normal_buffer[vertex_count * 3 + 16] = 1.0f;
		normal_buffer[vertex_count * 3 + 17] = 0.0f;

		vertex_count += 6;
		break;
	}

}
