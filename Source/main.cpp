
#define GLEW_STATIC

#include <GL/glew.h>

#include <iostream>
#include <thread>
#include <chrono>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "vertices.h"
#include "shaders.h"
#include "solid.h"
#include "controller.h"

using std::string;

void debugShaders(GLint, GLint);
void setUpSDL(int, int, string, bool);
void setUpGL();
void setUpTransforms();
void updateGame();
void updateDisplay();
void destroyDisplay();

bool keyW = false, keyA = false, keyS = false, keyD = false, keySPACE = false, keyLSHIFT = false, running = true;
bool jumping = false;
bool bulletMoving, grenadeMoving;
double initCursorX, initCursorY;
GLuint vbo, vao, ebo, vertexShader, fragmentShader, shaderProgram, posAttrib, colAttrib, texAttrib;
GLuint textures[4];
SDL_Window* window;
SDL_GLContext glContext;
const int FPS = 60, FRAME_LEN = 1000 / 60;
const int WIDTH = 1080, HEIGHT = 720, BLOCK_ARRAY_SIZE = 4;
const float BLOCK_SIZE = 8.5f;

GLfloat xPos = 0.0f;
GLfloat yPos = 0.0f;
GLfloat zPos = 10.0f;
GLfloat playerHeight = 1.0f;
GLfloat hatHeight = 0.1f;
GLfloat fallSpeed = 0.05f;
GLfloat bulletXPos = 0.0f;
GLfloat bulletYPos = 0.0f;
GLfloat bulletZPos = 0.0f;
GLfloat grenadeXPos = 0.0f;
GLfloat grenadeYPos = 0.0f;
GLfloat grenadeZPos = 0.0f;
GLfloat viewAngleHoriz = 0.0f;
GLfloat viewAngleVert = 0.0f;
GLfloat moveSpeed = 0.12f;
GLfloat strafeSpeed = 0.12f;
GLfloat turnSpeed = 0.002f;
GLfloat bulletSpeed = 0.5f;
GLfloat grenadeSpeed = 0.25f;
GLfloat xPOV = xPos + cos(viewAngleHoriz);
GLfloat yPOV = yPos + sin(viewAngleHoriz);
GLfloat zPOV = zPos + sin(viewAngleVert);
GLfloat gravityAcc = -0.003f;
GLfloat grenadeVertVel = 0.0f;

glm::vec3 viewVec = glm::vec3(xPOV - xPos, yPOV - yPos, zPOV - zPos);
glm::vec3 viewInv = glm::vec3(viewVec.y, -viewVec.x, viewVec.z);
glm::vec3 bulletVec = viewVec;
glm::vec3 grenadeVec = viewVec;

glm::mat4 view, proj, gunTrans, trans;

std::chrono::system_clock::time_point frameStart;

Solid blocks[BLOCK_ARRAY_SIZE][BLOCK_ARRAY_SIZE][BLOCK_ARRAY_SIZE];

GLint uniTrans;
GLint uniGunTrans;
GLint uniBulletTrans;
GLint uniGrenadeTrans;
GLint uniView;
GLint uniProj;
GLint uniColor;
GLint uniGunColor;

Controller controller;

Solid solids[] =
{
		Solid(0,  2,  3, 20, 2, 2, 0.0f, 0.0f, 0.0f),
		Solid(5, 10,  3, 5, 1, 0.5, 0.0f, 0.0f, 0.0f),
		Solid(12, -1,  4, 1, 1, 1, 0.0f, 0.0f, 0.0f),
		Solid(4,  0, 3, 20, 1, 20, 0.0f, 0.0f, 0.0f),
		Solid(20,  0,  2, 5, 5, 5, 0.0f, 0.0f, 0.0f),
};

int main()
{
	setUpSDL(WIDTH, HEIGHT, "Hey Young World", false);
	setUpGL();
	setUpTransforms();

	while (running)
	{
		frameStart = std::chrono::system_clock::now();

		controller.takeInput();
		updateGame();
		updateDisplay();

		std::this_thread::sleep_until(frameStart + std::chrono::milliseconds(FRAME_LEN));
	}

	destroyDisplay();

	return 0;
}

void Solid::draw()
{
	trans = glm::mat4();
	glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);
	glUniform3f(uniColor, 1.0f, 0.0f, 0.0f);
	trans = glm::rotate(trans, this->pitch, glm::vec3(0.0f, 0.0f, 1.0f));
	trans = glm::rotate(trans, this->yaw, glm::vec3(0.0f, 1.0f, 0.0f));
	trans = glm::rotate(trans, this->roll, glm::vec3(1.0f, 0.0f, 0.0f));
	trans = glm::translate(trans, glm::vec3(this->x, this->y, this->z));
	trans = glm::scale(trans, glm::vec3(this->width, this->height, this->depth));
	glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(trans));
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void updateDisplay()
{
	//set camera
	view = glm::lookAt(
		glm::vec3(xPos, yPos, zPos),
		glm::vec3(xPOV, yPOV, zPOV),
		glm::vec3(0.0f, 0.0f, 1.0f)
		);
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
	glUniform3f(uniColor, 1.0f, 1.0f, 1.0f);

	for (int x = 0; x < BLOCK_ARRAY_SIZE; x++)
	{
		for (int y = 0; y < BLOCK_ARRAY_SIZE; y++)
		{
			for (int z = 0; z < BLOCK_ARRAY_SIZE; z++)
			{
				blocks[x][y][z].draw();
			}
		}
	}

	//draw gun
	glUniform3f(uniColor, 1.0f, 1.0f, 1.0f);
	glm::vec3 gunPos = glm::vec3(
		xPos + 1.4f * cos(viewAngleHoriz - 0.0f) * cos(viewAngleVert - 0.4f),
		yPos + 1.4f * sin(viewAngleHoriz - 0.0f) * cos(viewAngleVert - 0.4f),
		zPos + 1.4f * sin(viewAngleVert  - 0.4f));
	glm::mat4 gunTrans;
	glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 1);
	gunTrans = glm::translate(gunTrans, gunPos);
	gunTrans = glm::rotate(gunTrans, glm::degrees(viewAngleHoriz), glm::vec3(0.0f, 0.0f, 1.0f));
	gunTrans = glm::rotate(gunTrans, -glm::degrees(viewAngleVert), glm::vec3(0.0f, 1.0f, 0.0f));
	gunTrans = glm::scale(gunTrans, 0.167f * glm::vec3(3.0f, 1.0f, 1.0f));
	glUniformMatrix4fv(uniGunTrans, 1, GL_FALSE, glm::value_ptr(gunTrans));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//draw bullet
	glm::mat4 bulletTrans;
	glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 2);
	if (bulletMoving)
	{
		bulletXPos += bulletVec.x * bulletSpeed;
		bulletYPos += bulletVec.y * bulletSpeed;
		bulletZPos += bulletVec.z * bulletSpeed;
	}
	else
	{
		bulletXPos = gunPos.x;
		bulletYPos = gunPos.y;
		bulletZPos = gunPos.z;
	}
	bulletTrans = glm::translate(bulletTrans, glm::vec3(bulletXPos, bulletYPos, bulletZPos));
	bulletTrans = glm::scale(bulletTrans, 0.1f * glm::vec3(1.0f, 1.0f, 1.0f));
	bulletTrans = glm::rotate(bulletTrans, viewAngleHoriz, glm::vec3(0.0f, 0.0f, 1.0f));
	bulletTrans = glm::rotate(bulletTrans, -viewAngleVert, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniBulletTrans, 1, GL_FALSE, glm::value_ptr(bulletTrans));
	glDrawArrays(GL_TRIANGLES, 0, 36);


	//draw grenade
	glm::mat4 grenadeTrans;
	glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 2);
	if (grenadeMoving)
	{
		grenadeXPos += grenadeVec.x * grenadeSpeed;
		grenadeYPos += grenadeVec.y * grenadeSpeed;
		grenadeVertVel += gravityAcc;
		grenadeZPos += grenadeVertVel;
	}
	else
	{
		grenadeXPos = gunPos.x;
		grenadeYPos = gunPos.y;
		grenadeZPos = gunPos.z;
	}
	grenadeTrans = glm::translate(grenadeTrans, glm::vec3(grenadeXPos, grenadeYPos, grenadeZPos));
	grenadeTrans = glm::scale(grenadeTrans, 0.1f * glm::vec3(1.0f, 1.0f, 1.0f));
	grenadeTrans = glm::rotate(grenadeTrans, viewAngleHoriz, glm::vec3(0.0f, 0.0f, 1.0f));
	grenadeTrans = glm::rotate(grenadeTrans, -viewAngleVert, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(uniGrenadeTrans, 1, GL_FALSE, glm::value_ptr(grenadeTrans));
	glDrawArrays(GL_TRIANGLES, 0, 36);


    SDL_GL_SwapWindow(window);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void setUpGL()
{
		glewExperimental = GL_TRUE;
		glewInit();

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);

		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);

		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glBindFragDataLocation(shaderProgram, 0, "outColor");
		glLinkProgram(shaderProgram);
		glUseProgram(shaderProgram);

		//debugShaders(vertexShader, fragmentShader);

		posAttrib = glGetAttribLocation(shaderProgram, "position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);

		colAttrib = glGetAttribLocation(shaderProgram, "color");
		glEnableVertexAttribArray(colAttrib);
		glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

		texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
		glEnableVertexAttribArray(texAttrib);
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

		glGenBuffers(1, &ebo);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

		glGenTextures(4, textures);

		int width, height;
		unsigned char* image;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		image = SOIL_load_image("blackcat.png", &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[1]);
		image = SOIL_load_image("blackcat.png", &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 1);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textures[2]);
		image = SOIL_load_image("blackcat.png", &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 2);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, textures[3]);
		image = SOIL_load_image("blackcat.png", &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 3);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		uniTrans = glGetUniformLocation(shaderProgram, "model");
		uniGunTrans = glGetUniformLocation(shaderProgram, "model");
		uniBulletTrans = glGetUniformLocation(shaderProgram, "model");
		uniGrenadeTrans = glGetUniformLocation(shaderProgram, "model");
		uniView = glGetUniformLocation(shaderProgram, "view");
		uniProj = glGetUniformLocation(shaderProgram, "proj");
		uniColor = glGetUniformLocation(shaderProgram, "overrideColor");
		uniGunColor = glGetUniformLocation(shaderProgram, "overrideColor");

		glEnable(GL_DEPTH_TEST);
}


void updateGame()
{
	if (controller.pressed(SDLK_w))
	{
		float newX = xPos + moveSpeed * cos(viewAngleHoriz);
		float newY = yPos + moveSpeed * sin(viewAngleHoriz);
		bool clear = true;
		for (int t = 0; t < (sizeof(solids) / sizeof(solids[0])); t++)
		{
			if (solids[t].collides(newX, newY, zPos - playerHeight) || solids[t].collides(newX, newY, zPos + hatHeight))
			{
				clear = false;
				break;
			}
		}
		if (clear)
		{
			xPos = newX;
			yPos = newY;
		}
	}
	if (controller.pressed(SDLK_s))
	{
		float newX = xPos - moveSpeed * cos(viewAngleHoriz);
		float newY = yPos - moveSpeed * sin(viewAngleHoriz);
		bool clear = true;
		for (int t = 0; t < (sizeof(solids) / sizeof(solids[0])); t++)
		{
			if (solids[t].collides(newX, newY, zPos - playerHeight) || solids[t].collides(newX, newY, zPos + hatHeight))
			{
				clear = false;
				break;
			}
		}
		if (clear)
		{
			xPos = newX;
			yPos = newY;
		}
	}
	if (controller.pressed(SDLK_a))
	{
		float newY = yPos + strafeSpeed * sin(1.5708f + viewAngleHoriz);
		float newX = xPos + strafeSpeed * cos(1.5708f + viewAngleHoriz);
		bool clear = true;
		for (int t = 0; t < (sizeof(solids) / sizeof(solids[0])); t++)
		{
			if (solids[t].collides(newX, newY, zPos - playerHeight) || solids[t].collides(newX, newY, zPos + hatHeight))
			{
				clear = false;
				break;
			}
		}
		if (clear)
		{
			xPos = newX;
			yPos = newY;
		}
	}
	if (controller.pressed(SDLK_d))
	{
		float newY = yPos - strafeSpeed * sin(1.5708f + viewAngleHoriz);
		float newX = xPos - strafeSpeed * cos(1.5708f + viewAngleHoriz);
		bool clear = true;
		for (int t = 0; t < (sizeof(solids) / sizeof(solids[0])); t++)
		{
			if (solids[t].collides(newX, newY, zPos - playerHeight) || solids[t].collides(newX, newY, zPos + hatHeight))
			{
				clear = false;
				break;
			}
		}
		if (clear)
		{
			xPos = newX;
			yPos = newY;
		}
	}
	if (controller.pressed(SDLK_SPACE))
	{
		fallSpeed = -0.2;
	}
	if (controller.pressed(SDLK_LSHIFT))
	{
		float newZ = zPos - moveSpeed;
		bool clear = true;
		for (int t = 0; t < (sizeof(solids) / sizeof(solids[0])); t++)
		{
			if (solids[t].collides(xPos, yPos, newZ))
			{
				clear = false;
				break;
			}
		}
		if (clear)
		{
			zPos = newZ;
		}
	}
	if (controller.quit())
	{
		running = false;
	}
	if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		bulletMoving = true;
		bulletXPos = xPos + 1.4f * cos(viewAngleHoriz - 0.0f) * cos(viewAngleVert - 0.4f);
		bulletYPos = yPos + 1.4f * sin(viewAngleHoriz - 0.0f) * cos(viewAngleVert - 0.4f);
		bulletZPos = zPos + 1.4f * sin(viewAngleVert - 0.4f);
		bulletVec = viewVec;
	}
	if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT))
	{
		grenadeMoving = true;
		grenadeXPos   = xPos + 1.4f * cos(viewAngleHoriz - 0.0f) * cos(viewAngleVert - 0.4f);
		grenadeYPos   = yPos + 1.4f * sin(viewAngleHoriz - 0.0f) * cos(viewAngleVert - 0.4f);
		grenadeZPos   = zPos + 1.4f * sin(viewAngleVert - 0.4f);
		grenadeVec    = viewVec;
		grenadeVertVel = 0.05 * grenadeVec.z;
	}

	bool clear = true;
	for (int x = 0; x < BLOCK_ARRAY_SIZE; x++)
	{
		for (int y = 0; y < BLOCK_ARRAY_SIZE; y++)
		{
			for (int z = 0; z < BLOCK_ARRAY_SIZE; z++)
			{
				bool downTest = blocks[x][y][z].collides(xPos, yPos, zPos - fallSpeed - gravityAcc - playerHeight);
				bool upTest = blocks[x][y][z].collides(xPos, yPos, zPos - fallSpeed - gravityAcc + playerHeight);
				if (fallSpeed >= 0 && downTest)
				{
					clear = false;
					break;
				}
				if (fallSpeed < 0 && upTest)
				{
					fallSpeed = 0;
				}
			}
		}
	}
	if (clear)
	{
		zPos -= fallSpeed;
		fallSpeed -= gravityAcc;
	}

	int currentCursorX, currentCursorY;
	SDL_GetMouseState(&currentCursorX, &currentCursorY);

	viewAngleHoriz += (float) (initCursorX - currentCursorX) * turnSpeed;

	float newVertAngle = viewAngleVert + (float)(initCursorY - currentCursorY) *  turnSpeed;

	bool cursorInScreen = !((newVertAngle > 3.14f / 2.0f && initCursorY > currentCursorY) || (newVertAngle < -3.14f / 2.0f && initCursorY < currentCursorY));
	if (cursorInScreen)
	{
		viewAngleVert = newVertAngle;
	}

	xPOV = xPos + cos(viewAngleHoriz) * cos(viewAngleVert);
	yPOV = yPos + sin(viewAngleHoriz) * cos(viewAngleVert);
	zPOV = zPos + sin(viewAngleVert);

	viewVec = glm::vec3(xPOV - xPos, yPOV - yPos, zPOV - zPos);
	viewInv = glm::vec3(viewVec.y, -viewVec.x, viewVec.z);

	SDL_WarpMouseInWindow(window, WIDTH / 2, HEIGHT / 2);
	initCursorX = WIDTH / 2;
	initCursorY = HEIGHT / 2;

}

void setUpTransforms()
{
	proj = glm::perspective(45.0f, (float) WIDTH / HEIGHT, 0.01f, 100.0f);

	view = glm::lookAt(
		glm::vec3(xPos, yPos, zPos),
		glm::vec3(xPOV, yPOV, zPOV),
		glm::vec3(0.0f, 0.0f, 1.0f)
		);
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	bulletMoving = false;
	grenadeMoving = false;
	initCursorX = WIDTH / 2;
	initCursorY = HEIGHT / 2;
	gunTrans = glm::scale(gunTrans, 0.167f * glm::vec3(3.0f, 1.0f, 1.0f));

	for (int x = 0; x < BLOCK_ARRAY_SIZE; x++)
	{
		for (int y = 0; y < BLOCK_ARRAY_SIZE; y++)
		{
			for (int z = 0; z < BLOCK_ARRAY_SIZE; z++)
			{
				blocks[x][y][z] = Solid(x * BLOCK_SIZE, y * BLOCK_SIZE, z * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, 0.0f, 0.0f, 0.0f);
			}
		}
	}

}

void setUpSDL(int width, int height, string title, bool fullScreen)
{
	SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    GLuint flags = fullScreen ? (SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN) : SDL_WINDOW_OPENGL;
    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
    glContext = SDL_GL_CreateContext(window);

    SDL_ShowCursor(SDL_FALSE);
}

void destroyDisplay()
{
	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
}

void debugShaders(GLint vertexShaderIn, GLint fragmentShaderIn)
{
	GLint status;
	glGetShaderiv(vertexShaderIn, GL_COMPILE_STATUS, &status);

	char buffer[512];
	glGetShaderInfoLog(vertexShaderIn, 512, NULL, buffer);

	std::cout << "VERTEX" << std::endl;
	std::cout << buffer << std::endl;

	glGetShaderiv(fragmentShaderIn, GL_COMPILE_STATUS, &status);

	glGetShaderInfoLog(fragmentShaderIn, 512, NULL, buffer);

	std::cout << "FRAGMENT" << std::endl;
	std::cout << buffer << std::endl;
}
