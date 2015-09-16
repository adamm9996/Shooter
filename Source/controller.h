/*
 * controller.h
 *
 *  Created on: 15 Sep 2015
 *      Author: adam
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <map>
#include <SDL2/SDL.h>

class Controller
{
public:
	Controller();
	virtual ~Controller();
	bool pressed(SDL_Keycode);
	bool mousePressed(int);
	void takeInput();
	bool quit();
private:
	std::map<SDL_Keycode, bool> keyMap;
	bool quitPressed;
};

#endif /* CONTROLLER_H_ */
