/*
 * display.h
 *
 *  Created on: 16 Sep 2015
 *      Author: adam
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

class Display
{
public:
	Display(int, int, std::string, bool);
	virtual ~Display();
private:
	SDL_Window* window;
	SDL_GLContext glContext;
};

#endif /* DISPLAY_H_ */
