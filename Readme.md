# Brief
A very minimal game that I made when first learning OpenGL and object oriented programming in C++.

**Author:** Max Vestrand <br> **Created:** 2014

*This readme was written in March, 2026, when it was uploaded to github (it had no source control when made).*


# Description
This is a *very* simple 2 player game made in C++ using OpenGL, GLM for math, and GLFW for a windowing and input system.
It's a toy project I made when I first learning OpenGL. It was made while I was an undergraduate at NMT, but was entirely independent and unrelated to any classes or course work that I had. 
It has no audio and the configuration of the game has to be typed into the main menu (with the game window focused *not* the console).

The players try to be the last one alive. Each player can jump and drop bombs to destroy terrain. The terrain drops power-ups that make the player faster and give them more simultaneous bombs. 




# Compiling
The precompiled game executable (for 32 bit windows) is included just to guarantee that there is at least some machine that it runs on. In theory it should be able to compile and run on windows or linux using the included libraries (I'm sorry, I don't know the specific architectures they actually support), as long as they have GCC's c++ compiler and make installed. 

However, given that this is from over 10 years ago, I don't know that I would trust the ABI's to still be consistent. I have no idea what version of GCC this or the shared libraries were built with other than that it uses c++11. Personally, I would recommend using the prebuilt executable if at all possible. Or maybe it runs fine? Again, I'm sorry, I don't have a proper way to test it at the moment.


# Setup
Enter the file of the world map you want to play on, the number of players (originally planned to be more than 2), and the number of rounds to play.  The bombs damage the terrain voxels until they are destroyed, and can drop powerups. Each player tries to trap the other into falling off the map or getting hit by a bomb. 


### Controls
Player 1(Red):  
``move - wasd``<br>``jump - space``<br>``bomb - F``

Player 2(Blue):  
``move - arrowkeys``<br>``jump - Num0``<br>``bomb - Num1``


### Map Files
The game map files consist of a "*.world" file, where you specify the game map dimensions and the terrain files to fill it with.  The "*.terra" files are files that were intended to be placeable terrain patterns, the intended idea being to build up a world by placing preset patterns of terrain blocks, however the existing implementation of it makes that extremely difficult, as I apparently failed to include dimensions in the terra files.


##### World File format
``worldsizeX,worldsizeY,worldsizeZ``<br>
``xpos,ypos,zpos,terrain_filename``<br>
``xpos,ypos,zpos,terrain_filename``<br>
``...``


##### Terra File format
Each .terra file consists of rows of values 0-9 indicating each voxels hp. They consist of multiple single layers that build up a 3d space.


# Misc. Comments
The OpenAL32.dll was probably intended to be used to add audio, but at that point the project was abandoned. There are a number of extraneous build artifacts included, and the source code seems to be duplicated between the root directory and the src directory.  However I've left everything as is to avoid breaking anything.


# Credit

The GL, GLFW, and glm code and the included .a and .dll shared libraries are not my work and I take no credit for them.  









