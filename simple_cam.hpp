#ifndef SIMPLE_CAM_HPP_
#define SIMPLE_CAM_HPP_

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include <math.h>
#include <stddef.h>



#ifndef SC_MODELVIEW_NAME
#define SC_MODELVIEW_NAME  "mv_matrix"
#endif
#ifndef SC_PROJECTION_NAME
#define SC_PROJECTION_NAME "proj_matrix"
#endif
#ifndef SC_PAN_SPEED
#define SC_PAN_SPEED 10.0f
#endif
#ifndef SC_ROTATE_SPEED
#define SC_ROTATE_SPEED 1.0f
#endif
#ifndef SC_FOVY
#define SC_FOVY 50.0f
#endif
#ifndef SC_NEAR_CLIP
#define SC_NEAR_CLIP 0.1f
#endif
#ifndef SC_FAR_CLIP
#define SC_FAR_CLIP 1000.0f
#endif

#define SC_RIGHT      0
#define SC_BACKWARD  90
#define SC_LEFT     180
#define SC_FORWARD  270

// Future camera idea
//  class map_function {double map(double t);}
//  void smooth(map_function *, camera *, double t);
//  void smooth(map_function *, quat rot, vec3 pos, double t);
//  void smooth(map_function *, quat rot, double t);

class simple_cam {
public:
	void init(GLuint program, int w, int h);
	void init(GLuint program, int w, int h, glm::vec3 init_pos);
	void resize_screen(int w, int h);
	virtual void pan(double dir, double time_chng);
	virtual void pan_rad(double dir, double time_chng);
	virtual void turn(double x, double y);
	void reset_rotation();
	void set_zoom(double z);
	void set_position(glm::vec3 position);
	glm::mat4 get_projection();
	glm::mat4 get_modelview();
	void rotate(float angle, float x, float y, float z);
	void rotate(float angle, glm::vec3 axis);
	void rotate_rad(float angle, float x, float y, float z);
	void rotate_rad(float angle, glm::vec3 axis);
	void update_uniforms();

	int width;
	int height;
	float zoom;
	GLuint mv_location;
	GLuint proj_location;
	glm::vec3 pos;
	glm::quat rot;
};
#endif
