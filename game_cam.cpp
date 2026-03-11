#include "game_cam.hpp"
#include <algorithm>

#define EFFECTIVE_MAX_DIST 2
#define MAX_MOVE_SPEED 9.0f
#define MAX_TILT_SPEED 3.0f
#define MAX_ZOOM_SPEED .4f
#define TILT_DAMPENING .3f

/**
 * Move toward a value by the amount given
 * @param float start, the initial value
 * @param float goal, the desired final value
 * @param float amount, the increment to change the value by
 * @return float, the new value, shifted toward the goal
 */
float towardf(float start, float goal, float amount)
{
	if (start == goal)
		return goal;

	if (start > goal) {
		return std::max(goal, start-amount);
	} else {
		return std::min(goal, start+amount);
	}
}

// Parameters determining how the camera behaves
#define VERT_STEP_SPEED 80.0f
#define HOR_STEP_SPEED  80.0f
#define VERT_ZOOM_SPEED 80.0f
#define HOR_ZOOM_SPEED  80.0f
#define VERT_CEILING 0.65f
#define HOR_CEILING  0.65f
#define VERT_ZOOM_CEILING 1.3f
#define HOR_ZOOM_CEILING  1.3f
#define VERT_ZOOM_FLOOR   1.25f
#define HOR_ZOOM_FLOOR    1.25f
#define MIN_ZOOM        5.0f


/**
 * Set the camera based on the parameters given
 * @param double t, the time that has passed since the last update
 * @param float max_x, the highest horizontal value to track, in screen coordinates
 * @param float min_x, the lowest horizontal value to track, in screen coordinates
 * @param float max_y, the highest vertical value to track, in screen coordinates
 * @param float min_y, the lowest vertical value to track, in screen coordinates
 */
void game_cam::set_camera(glm::vec3 center, float max_dist, int objs, double t,
			  float max_x, float min_x, float max_y, float min_y)
{
	if (!in_main_menu) {// Set the ingame camera
		// Set the camera angle
		pitch = -glm::radians(75.0f);
		yaw = 0;

		// Calculate values used for camera control
		float dif_x = max_x - min_x;
		float dif_y = max_y - min_y;

		// Move the camera
		if (max_x > HOR_CEILING && dif_x < HOR_ZOOM_CEILING) {
			pos.x += HOR_STEP_SPEED * t * glm::abs(max_x - HOR_CEILING);
		}
		if (dif_x < HOR_ZOOM_CEILING && min_x < -HOR_CEILING){
			pos.x -= HOR_STEP_SPEED * t * glm::abs(-min_x - HOR_CEILING);
		}
		if (max_y > VERT_CEILING && dif_y < VERT_ZOOM_CEILING) {
			pos.z -= VERT_STEP_SPEED * t * glm::abs(max_y - VERT_CEILING);
		}
		if (dif_y < VERT_ZOOM_CEILING && min_y < -VERT_CEILING){
			pos.z += VERT_STEP_SPEED * t * glm::abs(-min_y - VERT_CEILING);
		}

		// Zoom control
		if (dif_x > HOR_ZOOM_CEILING) {
			zoom -= HOR_ZOOM_SPEED * t * (dif_x - HOR_ZOOM_CEILING);
		}
		if (dif_y > VERT_ZOOM_CEILING) {
			zoom -= VERT_ZOOM_SPEED * t * (dif_y - VERT_ZOOM_CEILING);
		}
		if (dif_x < HOR_ZOOM_FLOOR
		    && dif_y < VERT_ZOOM_FLOOR
		    && zoom < -MIN_ZOOM) {
			if (dif_x > dif_y) {
				zoom += HOR_ZOOM_SPEED * t * (HOR_ZOOM_FLOOR - dif_x);
			} else {
				zoom += VERT_ZOOM_SPEED * t * (VERT_ZOOM_FLOOR - dif_y);
			}
		}

		rot = glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f))
			* glm::angleAxis(pitch, glm::vec3(1.0f, 0.0f, 0.0f));
	} else {
		// Set the camera to view the title screen
		pos.y = 4.0f;
		pos.x = 0.5f;
		pos.z = 0.5f;
		pitch = -glm::radians(25.0f);
		yaw += glm::radians(25.0f) * t; // Make the camera spin

		// Set the zoom and rotation
		zoom = towardf(zoom, -22.25, MAX_ZOOM_SPEED);
		rot = glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f))
			* glm::angleAxis(pitch, glm::vec3(1.0f, 0.0f, 0.0f));
	}
}
