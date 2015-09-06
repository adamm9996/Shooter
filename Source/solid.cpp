/*
 * solid.cpp
 *
 *  Created on: 6 Sep 2015
 *      Author: adam
 */

#include "solid.h"

Solid::Solid()
{
	this->x = 0.0f;
	this->y = 0.0f;
	this->z = 0.0f;
	this->width = 1.0f;
	this->height = 1.0f;
	this->depth = 1.0f;
	this->pitch = 0.0f;
	this->yaw = 0.0f;
	this->roll = 0.0f;
}

Solid::Solid(float x, float y, float z, float width, float depth, float height, float pitch, float yaw, float roll)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->width = width;
	this->height = height;
	this->depth = depth;
	this->pitch = pitch;
	this->yaw = yaw;
	this->roll = roll;
}

bool Solid::collides(float x, float y, float z)
{
	if (this->pitch == 0.0f && this->yaw == 0.0f && this->roll == 0.0f)
	{
		//if Solid is not rotated, use AABB testing
		if(x > this->x - width / 2  && x < this->x + width / 2 &&
		y > this->y - height / 2 && y < this->y + height / 2 &&
	    z > this->z - depth / 2 && z < this->z + depth / 2)
	    {
	        return true;
	    }
		return false;
	}
	return false;
}

Solid::~Solid()
{
}

