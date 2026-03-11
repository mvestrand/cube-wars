#ifndef GLFW_GAME_TERRAIN_HPP_
#define GLFW_GAME_TERRAIN_HPP_


#define TERRA_TYPE_AIR          0
#define TERRA_TYPE_LOOSE_DIRT   1
#define TERRA_TYPE_DIRT         2
#define TERRA_TYPE_GRASS        3
#define TERRA_TYPE_GRAVEL       4
#define TERRA_TYPE_CRACKED_ROCK 5
#define TERRA_TYPE_ROCK         6
#define TERRA_TYPE_UNBREAKABLE  7
#define TERRA_TYPE_WOOD         8
#define TERRA_TYPE_START_POINT  9

#include <stdio.h>
#include <stdlib.h>
#include "glm/glm.hpp"
#include "GL/glew.h"

// #ifndef DEBUG
// #define DEBUG(msg) fprintf(stderr, (msg))
// #endif

#define INIT_MAX_VERTEX

#define MAX_VERTICES 1000000

enum cube_face {
	X_PLUS, X_MINUS, Y_PLUS, Y_MINUS, Z_PLUS, Z_MINUS
};

class world;
class explosion;

class terrain {
public:
	terrain(world *wrld, int x, int y, int z);
	terrain(world *wrld, int x, int y, int z, float x_off, float y_off, float z_off);
	~terrain();
	void load(const char *filename);
	void load_at(const char *filename, float x, float y, float z);
	void clear(int val);

	int get_x_max();
	int get_y_max();
	int get_z_max();

// 	double collision_test(glm::vec3 init_pos, glm::vec3 final_pos,
// 				       glm::vec3 *col_pos, glm::vec3 *col_normal);
	double get_wall_dist(glm::vec3 pos, glm::vec2 dir, float radius, float height);
	double get_wall_dist(glm::vec3 pos, glm::vec2 dir);
	double get_ceiling_dist(glm::vec3 pos);
	double get_ceiling_dist(glm::vec3 pos, float radius);
	double get_ground_dist(glm::vec3 pos);
	double get_ground_dist(glm::vec3 pos, float radius);

	void handle_explosion(glm::vec3 pos, float radius, explosion *exp);
	void damage(int x, int y, int z);

	glm::vec3 get_nth_location(int value, int n);
	glm::vec3 get_start_location(int player);

	int get_value(glm::vec3 pos);
	int get_value(int x, int y, int z);
	int set_value(int value, int x, int y, int z);
	void create_pow_at(int val, int x, int y, int z);

	void draw();
	void update_model();
	void write_face(cube_face dir, int value, float x, float y, float z);
	int test_vertex(glm::vec3 pos, float radius, int x, int y, int z);

private:
	//Logic
	world *game_world;
	int x_max, y_max, z_max;
	int *data;
	float x_offset, y_offset, z_offset;

	//Graphics
	bool need_update;
	int vertex_count;
	//int max_vert_count;

	GLuint buffer[3];
	GLfloat vertex_buffer[MAX_VERTICES * 3];
	GLfloat color_buffer[MAX_VERTICES * 3];
	GLfloat normal_buffer[MAX_VERTICES * 3];
};


#endif
