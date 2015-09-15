/*
 * controller.cpp
 *
 *  Created on: 15 Sep 2015
 *      Author: adam
 */

#include "controller.h"

Controller::Controller()
{
	keyMap[SDLK_w] = false;
	keyMap[SDLK_a] = false;
	keyMap[SDLK_s] = false;
	keyMap[SDLK_d] = false;
	keyMap[SDLK_SPACE] = false;
	keyMap[SDLK_LSHIFT] = false;
	keyMap[SDLK_ESCAPE] = false;
	quitPressed = false;
}

Controller::~Controller()
{
}

bool Controller::pressed(SDL_Keycode button)
{
	return keyMap[button];
}

bool Controller::quit()
{
	return (quitPressed || keyMap[SDLK_ESCAPE]);
}

void Controller::takeInput()
{
    SDL_Event windowEvent;
      while (SDL_PollEvent(&windowEvent))
      {
          if (windowEvent.type == SDL_QUIT)
        	  quitPressed = true;

          if (windowEvent.type == SDL_KEYDOWN)
          {
              keyMap[windowEvent.key.keysym.sym] = true;
          }

          if (windowEvent.type == SDL_KEYUP)
          {
              keyMap[windowEvent.key.keysym.sym] = false;
          }
      }
}
