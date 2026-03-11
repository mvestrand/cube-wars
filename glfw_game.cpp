#include "glfw_game.hpp"

#define VERTEX_BUFFER 0
#define COLOR_BUFFER  1
#define NORMAL_BUFFER 2

#include <iostream>

using namespace glm;

GLuint compile_shaders()
{
	GLuint vs;
	GLuint fs;
	GLuint program;

	static const GLchar *vs_src[] = GLSL
		(
		 430,

		 layout(location = 0) in vec4 position;
		 layout(location = 1) in vec3 color;
		 layout(location = 2) in vec3 normal;

		 out vec4 vs_col;

		 uniform mat4 rot_matrix;
		 uniform mat4 mv_matrix;
		 uniform mat4 proj_matrix;
		 uniform vec4 light_vec;

		 void main(void)
		 {
			 gl_Position = proj_matrix * mv_matrix * rot_matrix * position;
			 vs_col = 0.6 * vec4(color, 1.0)
				 + 0.4 * vec4(color, 1.0) * dot(light_vec, rot_matrix * vec4(normal, 0));
		 }
		 );
	static const GLchar *fs_src[] = GLSL
		(
		 430,

		 uniform sampler2D tex_object;

		 in vec4 vs_col;
		 out vec4 color;

		 void main(void)
		 {
			 color = vs_col;
		 }
		);

	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, vs_src, NULL);
	glCompileShader(vs);
	check_glsl_status(vs, "Vertex shader compiled\n",
			  "Vertex shader failed to compile\n");

	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, fs_src, NULL);
	glCompileShader(fs);
	check_glsl_status(fs, "Fragment shader compiled\n",
			  "Fragment shader failed to compile\n");

	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	check_glsl_status(program, "Program linked\n",
			  "Program failed to link correctly\n");

	GLint status = GL_FALSE;
	glValidateProgram(program);
	glGetProgramiv( program, GL_VALIDATE_STATUS, &status);
	if (status == GL_FALSE) {
		ERROR("Program failed validation\n");
	} else {
		DEBUG("Program validated\n");
	}

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

/**
 * Set the app to draw at the given location
 * @param vec3 pos, the position to draw at
 * @param quat rot, the rotation of the object to be drawn
 * @param float scale, the size of the object to be drawn
 */
void glfw_game::set_draw_location(vec3 pos, quat rot, float scale)
{
	glUniformMatrix4fv(mv_location, 1, GL_FALSE,
			   value_ptr(camera.get_modelview()
				     * glm::translate(mat4(1.0f), pos)
				     * glm::scale(mat4(1.0f), glm::vec3(scale))));
	glUniformMatrix4fv(rot_location, 1, GL_FALSE, value_ptr(mat4_cast(rot)));
}

void glfw_game::init()
{
	rend_prog = compile_shaders();

	//Get uniform locations
	mv_location = glGetUniformLocation(rend_prog, "mv_matrix");
	proj_location = glGetUniformLocation(rend_prog, "proj_matrix");
	rot_location = glGetUniformLocation(rend_prog, "rot_matrix");
	light_location = glGetUniformLocation(rend_prog, "light_vec");

	// Initialize the camera
	camera.init(rend_prog, width, height);
	camera.set_zoom(-20);
	rot_matrix = mat4(1.0f);

	// Create vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Enable depth testing and face culling
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);

	// Create the game world
	game_world = new world;
	game_world->game = this;
	game_world->init();
}

void glfw_game::update()
{
	static double last_time;
	double time_passed = current_time - last_time;
	last_time = current_time;

	game_world->update(time_passed);
}

void glfw_game::draw()
{
	static double last_time;
	double time_passed = current_time - last_time;
	last_time = current_time;

	glViewport(0, 0, width, height);

	glClearColor(0.2f, 0.7f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Create a vector representing the light direction
	vec4 light_vec = rotate(mat4(1.0f), radians(15.0f), vec3(1.0f, 0.0f, 0.0f)) *
		rotate(mat4(1.0f), radians(35.0f), vec3(0.0f, 1.0f, 0.0f)) *
		vec4(0.0f, 1.0f, 0.0f, 0.0f);

	glUseProgram(rend_prog);

	// Update the camera
	camera.resize_screen(width, height);
	float max_dist;
	float max_x, min_x, max_y, min_y;
	int obj_count;
	glm::vec3 cam_center = game_world->get_cam_track_center(&max_dist, &obj_count);
	game_world->calc_cam_settings(camera.get_projection() * camera.get_modelview(),
				      &max_x, &min_x, &max_y, &min_y);
	camera.set_camera(cam_center, max_dist, obj_count, time_passed,
			  max_x, min_x, max_y, min_y);

	// Update uniforms
	glUniformMatrix4fv(mv_location, 1, GL_FALSE, value_ptr(camera.get_modelview()));
	glUniformMatrix4fv(proj_location, 1, GL_FALSE, value_ptr(camera.get_projection()));
	glUniformMatrix4fv(rot_location, 1, GL_FALSE, value_ptr(rot_matrix));
	glUniform4fv(light_location, 1, value_ptr(light_vec));

	game_world->draw(time_passed);
}

void glfw_game::cleanup()
{
	game_world->cleanup();
	delete game_world;
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(rend_prog);
}

void glfw_game::events()
{
	static double last_time;
	double time_passed = current_time - last_time;
	last_time = current_time;

	game_world->events(time_passed);
}


void glfw_game::on_mouse_button(GLFWwindow *window,
		     int button, int action,
		     int mods)
{
	game_world->on_mouse_button(window, button, action, mods);
}



void glfw_game::on_cursor_pos(GLFWwindow *window,
				double xpos, double ypos)
{
	game_world->on_cursor_pos(window, xpos, ypos);
}

void glfw_game::on_cursor_enter(GLFWwindow *window, int entered)
{
	game_world->on_cursor_enter(window, entered);
}


void glfw_game::on_scroll(GLFWwindow *window,
			  double xoffset, double yoffset)
{
	game_world->on_scroll(window, xoffset, yoffset);
}


void glfw_game::on_key(GLFWwindow *window,
			 int key, int scancode,
			 int action, int mods)
{
	// Handle backspaces for keyboard input
	if (key == GLFW_KEY_BACKSPACE && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		if (get_text)
			delete_keyboard_char();
	} else if (key == GLFW_KEY_ENTER && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		if (get_text)
			put_keyboard_char('\n');
	}

	// Close if the escape key is pressed
	if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
		glfwSetWindowShouldClose(window, true);
	}

	game_world->on_key(window, key, scancode, action, mods);
}

void glfw_game::on_char(GLFWwindow *window, unsigned int codepoint)
{
	// Handle keyboard input
	if (get_text)
		put_keyboard_char((char)codepoint);

	game_world->on_char(window, codepoint);
}

/**
 * Start collecting keyboard input in the app's buffer
 */
void glfw_game::start_keyboard()
{
	get_text = true;
	next_char = 0;
	char_buffer[0] = '\0';
}

/**
 * Get the buffer with keyboard input stored in it
 * @return char *, the buffer with keyboard input
 */
char *glfw_game::get_current_keyboard()
{
	return char_buffer;
}

/**
 * Get a line from the keyboard buffer if there is one
 * @return char *, the buffer with keyboard input
 */
char *glfw_game::get_keyboard_line()
{
	char *cp;
	if ((cp = strchr(char_buffer, '\n')) != NULL) {
		*cp = 0;
		return char_buffer;
	}
	return NULL;
}

/**
 * Delete a single character from the buffer
 */
void glfw_game::delete_keyboard_char()
{
	if (next_char > 0) {
		// Delete a character and clear it from the terminal
		putchar('\b');
		putchar(' ');
		putchar('\b');
		std::cout.flush();
		char_buffer[next_char] = '\0';
		next_char--;
	}
}

/**
 * Add a single character to the buffer and print it to the terminal
 * @param char c, the character to store
 */
void glfw_game::put_keyboard_char(char c)
{
	if (next_char < 1023) {
		putchar(c);
		std::cout.flush();
		char_buffer[next_char] = c;
		char_buffer[next_char+1] = '\0';
		next_char++;
	}
}

/**
 * Stop collecting keyboard input
 */
void glfw_game::stop_keyboard()
{
	get_text = false;
}

/*
 * Creates a main function that runs the application
 */
GLFW_APP_DECLARE_MAIN(glfw_game);
