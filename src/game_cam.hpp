#ifndef GLFW_GAME_CAM_HPP_
#define GLFW_GAME_CAM_HPP_

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
#include "simple_cam.hpp"


class game_cam: public simple_cam {
public:
	void set_camera(glm::vec3 center, float max_dist, int objs, double t,
			float max_x, float min_x, float max_y, float min_y);
	void start_spinning(float speed) {spin_speed = speed;}
	void stop_spinning() {spin_speed = 0;}
	float pitch = -.5;
	float yaw = 0.0;
	float spin_speed = 0;
	bool in_main_menu = true;
};
#endif
