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
	Solid(int, int, int, int, int, int);
	virtual ~Solid();
	int getDepth() const	{return depth;}
	int getHeight() const	{return height;}
	int getWidth() const	{return width;}
	int getX() const		{return x;}
	int getY() const		{return y;}
	int getZ() const		{return z;}

private:
	int x, y, z, width, height, depth;
};

#endif /* SOLID_H_ */
