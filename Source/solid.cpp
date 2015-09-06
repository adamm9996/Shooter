/*
 * solid.cpp
 *
 *  Created on: 6 Sep 2015
 *      Author: adam
 */

#include "solid.h"

Solid::Solid(int x, int y, int z, int width, int height, int depth)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->width = width;
	this->height = height;
	this->depth = depth;
}

Solid::~Solid()
{
}

