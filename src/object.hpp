#ifndef GLFW_GAME_OBJECT_HPP_
#define GLFW_GAME_OBJECT_HPP_

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

class world;

class game_object {
public:
	virtual ~game_object() {}
	virtual void init() {}
	virtual void draw(double time) {}
	virtual void update(double time) {}
	virtual void events(double time) {}
	virtual glm::vec3 get_center()
		{return glm::vec3(pos.x, pos.y + height/2.0f, pos.z);}
	world *world;

	float radius = 0.0;
	float height = 0.0;
	float scale = 1.0;

	bool should_delete = false;
	bool should_track = false;

	glm::vec3 pos;
	glm::quat rot;
};

#endif
