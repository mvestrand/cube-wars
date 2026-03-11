
CC = g++

LIBS = libglew32.a libglfw3.a

L_FLAGS = -lgdi32 -lopengl32 -lglu32

C_FLAGS = -std=c++11 -m64 -DGLEW_STATIC -g -Wall

COMMON_HEADERS = glfw_app.hpp vertex_arrays.hpp

DEPS = glfw_app other simple_cam world terrain physics_object character player
OBJ_FILES = glfw_app.o simple_cam.o world.o terrain.o physics_object.o character.o player.o bomb.o explosion.o game_cam.o power_up.o round_manager.o

glfw_game: glfw_game.cpp $(DEPS) $(COMMON_HEADERS)
	$(CC) $(C_FLAGS) -c $@.cpp
	$(CC) -m64 $@.o $(OBJ_FILES) -o $@ $(LIBS) $(L_FLAGS)

glfw_app: glfw_app.cpp glfw_app.hpp
	$(CC) $(C_FLAGS) -c $@.cpp

simple_cam: simple_cam.cpp simple_cam.hpp
	$(CC) $(C_FLAGS) -c $@.cpp

world: world.cpp world.hpp
	$(CC) $(C_FLAGS) -c $@.cpp

terrain: terrain.cpp terrain.hpp
	$(CC) $(C_FLAGS) -c $@.cpp

exposion: exposion.cpp exposion.hpp physics_object
	$(CC) -std=c++11 -m64 -DGLEW_STATIC -c $@.cpp

game_cam: game_cam.cpp game_cam.hpp
	$(CC) -std=c++11 -m64 -DGLEW_STATIC -c $@.cpp

physics_object: physics_object.cpp physics_object.hpp object.hpp
	$(CC) $(C_FLAGS) -c $@.cpp

character: character.cpp character.hpp physics_object
	$(CC) $(C_FLAGS) -c $@.cpp

player: player.cpp player.hpp
	$(CC) $(C_FLAGS) -c $@.cpp

other:
	$(CC) $(C_FLAGS) -c bomb.cpp
	$(CC) $(C_FLAGS) -c explosion.cpp
	$(CC) $(C_FLAGS) -c game_cam.cpp
	$(CC) $(C_FLAGS) -c power_up.cpp
	$(CC) $(C_FLAGS) -c round_manager.cpp


clean:
	rm *.o *.exe *~

