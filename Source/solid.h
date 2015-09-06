/*
 * solid.h
 *
 *  Created on: 6 Sep 2015
 *      Author: adam
 */

#ifndef SOLID_H_
#define SOLID_H_

class Solid
{
public:
	Solid();
	Solid(float, float, float, float, float, float, float, float, float);
	virtual ~Solid();
	void draw();
	bool collides(float, float, float);
	float getDepth() const	{return depth;}
	float getHeight() const	{return height;}
	float getWidth() const	{return width;}
	float getX() const		{return x;}
	float getY() const		{return y;}
	float getZ() const		{return z;}

private:
	float x, y, z, width, height, depth, pitch, yaw, roll;
};

#endif /* SOLID_H_ */
