
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

using std::string;

void debugShaders(GLint, GLint);
void setUpSDL(int, int, string, bool);
void setUpGL();
void setUpTransforms();
void takeInput();
void updateGame();
void updateDisplay();
void destroyDisplay();

bool keyW = false, keyA = false, keyS = false, keyD = false, keySPACE = false, keyLSHIFT = false, running = true;
bool bulletMoving, grenadeMoving;
double initCursorX, initCursorY;
GLuint vbo, vao, ebo, vertexShader, fragmentShader, shaderProgram, posAttrib, colAttrib, texAttrib;
GLuint textures[4];
SDL_Window* window;
SDL_GLContext glContext;
const int FPS = 60, FRAME_LEN = 1000 / 60;

GLfloat xPos = 0.0f;
GLfloat yPos = 0.0f;
GLfloat zPos = 0.0f;
GLfloat bulletXPos = 0.0f;
GLfloat bulletYPos = 0.0f;
GLfloat bulletZPos = 0.0f;
GLfloat grenadeXPos = 0.0f;
GLfloat grenadeYPos = 0.0f;
GLfloat grenadeZPos = 0.0f;
GLfloat viewAngleHoriz = 0.0f;
GLfloat viewAngleVert = 0.0f;
GLfloat moveSpeed = 0.05f;
GLfloat strafeSpeed = 0.05f;
GLfloat turnSpeed = 0.002f;
GLfloat bulletSpeed = 0.32f;
GLfloat grenadeSpeed = 0.12f;
GLfloat xPOV = xPos + cos(viewAngleHoriz);
GLfloat yPOV = yPos + sin(viewAngleHoriz);
GLfloat zPOV = zPos + sin(viewAngleVert);
GLfloat gravityAcc = -0.0001f;
GLfloat grenadeVertVel = 0.0f;

glm::vec3 viewVec = glm::vec3(xPOV - xPos, yPOV - yPos, zPOV - zPos);
glm::vec3 viewInv = glm::vec3(viewVec.y, -viewVec.x, viewVec.z);
glm::vec3 bulletVec = viewVec;
glm::vec3 grenadeVec = viewVec;

glm::mat4 view, proj, gunTrans, trans;

std::chrono::system_clock::time_point frameStart;

GLint uniTrans;
GLint uniGunTrans;
GLint uniBulletTrans;
GLint uniGrenadeTrans;
GLint uniView;
GLint uniProj;
GLint uniColor;
GLint uniGunColor;

int main()
{

	setUpSDL(1080, 720, "Hey Young World", false);
	setUpGL();
	setUpTransforms();

	while (running)
	{
		frameStart = std::chrono::system_clock::now();

		takeInput();
		updateGame();
		updateDisplay();

		std::this_thread::sleep_until(frameStart + std::chrono::milliseconds(FRAME_LEN));
	}

	destroyDisplay();

	return 0;
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

	trans = glm::mat4();
	glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);
	glUniform3f(uniColor, 1.0f, 1.0f, 1.0f);
	glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(trans));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//draw hat
	glDrawArrays(GL_TRIANGLES, 36, 6);
	glUniform3f(uniColor, 0.0f, 0.0f, 0.0f);
	trans = glm::translate(trans, glm::vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(trans));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//draw floor
	trans = glm::mat4();
	glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 3);
	glUniform3f(uniColor, 1.0f, 1.0f, 1.0f);
	trans = glm::translate(trans, glm::vec3(0.0f, 0.0f, -3.0f));
	trans = glm::scale(trans, glm::vec3(20.0f, 20.0f, 1.0f));
	glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(trans));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//draw gun
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
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void setUpGL()
{
		glewExperimental = GL_TRUE;
		glewInit();

		//glOrtho(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 50.0f);
		//gluPerspective(90.0, 1.5, 0.5, 20.0);

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
		image = SOIL_load_image("blackcat.jpg", &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textures[1]);
		image = SOIL_load_image("blackcat.jpg", &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 1);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textures[2]);
		image = SOIL_load_image("blackcat.jpg", &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 2);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, textures[3]);
		image = SOIL_load_image("blackcat.jpg", &width, &height, 0, SOIL_LOAD_RGB);
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
	if (keyW)
	{
		yPos += moveSpeed * sin(viewAngleHoriz);
		xPos += moveSpeed * cos(viewAngleHoriz);
	}
	if (keyS)
	{
		yPos -= moveSpeed * sin(viewAngleHoriz);
		xPos -= moveSpeed * cos(viewAngleHoriz);
	}
	if (keyA)
	{
		yPos += strafeSpeed * sin(1.5708f + viewAngleHoriz);
		xPos += strafeSpeed * cos(1.5708f + viewAngleHoriz);
	}
	if (keyD)
	{
		yPos -= strafeSpeed * sin(1.5708f + viewAngleHoriz);
		xPos -= strafeSpeed * cos(1.5708f + viewAngleHoriz);
	}
	if (keySPACE)
	{
		zPos += moveSpeed;
	}
	if (keyLSHIFT)
	{
		zPos -= moveSpeed;
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

	int currentCursorX, currentCursorY;
	SDL_GetMouseState(&currentCursorX, &currentCursorY);

	viewAngleHoriz += (float) (initCursorX - currentCursorX) * turnSpeed;
	initCursorX  =  currentCursorX;

	bool cursorInScreen = !((viewAngleVert > 3.0f / 2.0f && initCursorY > currentCursorY) || (viewAngleVert < -3.0f / 2.0f && initCursorY < currentCursorY));
	if (cursorInScreen)
	{
		viewAngleVert += (float)(initCursorY - currentCursorY) *  turnSpeed;
	}
	initCursorY = currentCursorY;

	xPOV = xPos + cos(viewAngleHoriz) * cos(viewAngleVert);
	yPOV = yPos + sin(viewAngleHoriz) * cos(viewAngleVert);
	zPOV = zPos + sin(viewAngleVert);

	viewVec = glm::vec3(xPOV - xPos, yPOV - yPos, zPOV - zPos);
	viewInv = glm::vec3(viewVec.y, -viewVec.x, viewVec.z);
}

void takeInput()
{
    SDL_Event windowEvent;
      if (SDL_PollEvent(&windowEvent))
      {
          if (windowEvent.type == SDL_QUIT)
        	  running = false;
          if (windowEvent.type == SDL_KEYDOWN)
          {
              if (windowEvent.key.keysym.sym == SDLK_ESCAPE)
            	  running = false;

          	switch (windowEvent.key.keysym.sym)
          	{
          	case SDLK_w:
          		keyW = true;
          	break;
          	case SDLK_s:
          		keyS = true;
          	break;
          	case SDLK_a:
				keyA = true;
			break;
          	case SDLK_d:
				keyD = true;
			break;
          	case SDLK_SPACE:
          		keySPACE = true;
          	break;
          	case SDLK_LSHIFT:
          		keyLSHIFT = true;
          	break;
          	}
          }

          if (windowEvent.type == SDL_KEYUP)
          {
          	switch (windowEvent.key.keysym.sym)
				{
				case SDLK_w:
					keyW = false;
				break;
				case SDLK_s:
					keyS = false;
				break;
				case SDLK_a:
					keyA = false;
				break;
				case SDLK_d:
					keyD = false;
				break;
				case SDLK_SPACE:
					keySPACE = false;
				break;
				case SDLK_LSHIFT:
					keyLSHIFT = false;
				break;
				}
          }
      }
}

void setUpTransforms()
{
	proj = glm::perspective(45.0f, 1.5f, 1.0f, 10.0f);

	view = glm::lookAt(
		glm::vec3(xPos, yPos, zPos),
		glm::vec3(xPOV, yPOV, zPOV),
		glm::vec3(0.0f, 0.0f, 1.0f)
		);
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	bulletMoving = false;
	grenadeMoving = false;
	initCursorX = 0.0;
	initCursorY = 0.0;
	gunTrans = glm::scale(gunTrans, 0.167f * glm::vec3(3.0f, 1.0f, 1.0f));

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
