#include "simple_cam.hpp"

void simple_cam::set_position(glm::vec3 position)
{
	pos = position;
}

void simple_cam::set_zoom(double z)
{
	zoom = (float)z;
}

void simple_cam::turn(double x, double y)
{
	rotate(x * SC_ROTATE_SPEED, glm::mat3_cast(rot) * glm::vec3(0.0f, 1.0f, 0.0f));
	rotate(y * SC_ROTATE_SPEED, glm::mat3_cast(rot) * glm::vec3(1.0f, 0.0f, 0.0f));
}

void simple_cam::pan(double dir, double time_chng)
{
	pan_rad(glm::radians(dir), time_chng);
}

void simple_cam::pan_rad(double dir, double time_chng)
{
	pos = pos + glm::mat3_cast(rot) * glm::vec3((float)glm::cos(dir) * SC_PAN_SPEED * time_chng,
						    0.0f,
						    (float)glm::sin(dir) * SC_PAN_SPEED * time_chng);
}

void simple_cam::reset_rotation()
{
	rot = glm::quat();
}

void simple_cam::rotate(float angle, float x, float y, float z)
{
	rotate_rad(glm::radians(angle), x, y, z);
}

void simple_cam::rotate(float angle, glm::vec3 axis)
{
	rotate_rad(glm::radians(angle), axis);
}

void simple_cam::rotate_rad(float angle, float x, float y, float z)
{
	rot = glm::angleAxis(angle, glm::vec3(x, y, z)) * rot;
}

void simple_cam::rotate_rad(float angle, glm::vec3 axis)
{
	rot = glm::angleAxis(angle, axis) * rot;
}

void simple_cam::init(GLuint program, int w, int h)
{
	init(program, w, h, glm::vec3(0, 0, 2));
}


void simple_cam::init(GLuint program, int w, int h, glm::vec3 init_pos)
{
	mv_location = glGetUniformLocation(program, SC_MODELVIEW_NAME);
	proj_location = glGetUniformLocation(program, SC_PROJECTION_NAME);
	width = w;
	height = h;
	if (height == 0)	// Prevent divide by zero
		height = 1;
	pos = init_pos;
	rot = glm::quat();
}

void simple_cam::resize_screen(int w, int h)
{
	width = w;
	height = h;
	if (height == 0)	// Prevent divide by zero
		height = 1;

}

void simple_cam::update_uniforms()
{
	// Send new matrices to opengl
	glUniformMatrix4fv(mv_location, 1, GL_FALSE, glm::value_ptr(get_modelview()));
	glUniformMatrix4fv(proj_location, 1, GL_FALSE, glm::value_ptr(get_projection()));
}

glm::mat4 simple_cam::get_projection()
{
	return glm::perspective(glm::radians(SC_FOVY),
				((float)width / (float)height),
				SC_NEAR_CLIP,
				SC_FAR_CLIP);
}

glm::mat4 simple_cam::get_modelview()
{
	return glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, zoom))
		* glm::mat4_cast(glm::inverse(rot))
		* glm::translate(glm::mat4(1.0f), -pos);
}
