/*
 * shaders.h
 *
 *  Created on: 4 Sep 2015
 *      Author: adam
 */

#ifndef SHADERS_H_
#define SHADERS_H_

#define GLSL(src) "#version 130\n" #src

const GLchar* vertexShaderSource = GLSL
(
	in vec3 position;
	in vec3 color;
	in vec2 texcoord;

	out vec3 Color;
	out vec2 Texcoord;

	uniform mat4 model;
	uniform mat4 proj;
	uniform mat4 view;
	uniform vec3 overrideColor;

	void main()
	{
		Texcoord = texcoord;
		Color = overrideColor * color;
		gl_Position = proj * view * model * vec4(position, 1.0);
	}
);

const GLchar* fragmentShaderSource = GLSL
(
	in vec3 Color;
	in vec2 Texcoord;

	out vec4 outColor;

	uniform sampler2D tex;

	void main()
	{
		vec4 texColor = texture(tex, Texcoord);
		outColor = vec4(Color, 1.0) * texColor;
	}
);

#endif /* SHADERS_H_ */
