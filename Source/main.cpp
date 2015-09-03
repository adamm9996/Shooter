#define GLEW_STATIC

#include <GL/glew.h>

#include <iostream>
#include <thread>
#include <chrono>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SOIL/SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void debugShaders(GLint, GLint);

bool keyW = false, keyA = false, keyS = false, keyD = false, keySPACE = false, keyLSHIFT = false;
const GLchar* vertexShaderSource =
"#version 130\n"
"in vec3 position;"
"in vec3 color;"
"in vec2 texcoord;"

"out vec3 Color;"
"out vec2 Texcoord;"

"uniform mat4 model;"
"uniform mat4 proj;"
"uniform mat4 view;"
"uniform vec3 overrideColor;"
//"uniform mat4 scale;"
"void main()"
"{"
"Texcoord = texcoord;"
"Color = overrideColor * color;"
"gl_Position = proj * view * model * vec4(position, 1.0);"
"}";

const GLchar* fragmentShaderSource =
"#version 130\n"
"in vec3 Color;"
"in vec2 Texcoord;"

"out vec4 outColor;"

"uniform float water;"
"uniform sampler2D tex;"
"void main() {"
"vec4 texColor = texture(tex, Texcoord);"
"outColor = vec4(Color, 1.0) * texColor;"
"}";



int main()
{

	//glfwInit();
    //GLFWwindow* window = glfwCreateWindow(1080, 720, "OpenGL", nullptr, nullptr);
	//glfwMakeContextCurrent(window);

	SDL_Init(SDL_INIT_EVERYTHING);

	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_Window* window = SDL_CreateWindow("Hey young world", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    		800, 600, SDL_WINDOW_OPENGL);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    glewExperimental = GL_TRUE;
	glewInit();

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);

	glOrtho(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 50.0f);

	GLfloat xPos = -3.5f;
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
	GLfloat strafeSpeed = 0.01f;
	GLfloat turnSpeed = 0.001f;
	GLfloat bulletSpeed = 0.08f;
	GLfloat grenadeSpeed = 0.04f;
	GLfloat xPOV = xPos + cos(viewAngleHoriz);
	GLfloat yPOV = yPos + sin(viewAngleHoriz);
	GLfloat zPOV = zPos + sin(viewAngleVert);
	GLfloat gravityAcc = -0.0001f;
	GLfloat grenadeVertVel = 0.0f;
	glm::vec3 viewVec = glm::vec3(xPOV - xPos, yPOV - yPos, zPOV - zPos);
	glm::vec3 viewInv = glm::vec3(viewVec.y, -viewVec.x, viewVec.z);
	glm::vec3 bulletVec = viewVec;
	glm::vec3 grenadeVec = viewVec;


	GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

		-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

		-1.0f, -1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

		-0.5f, 1.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		0.5f, 1.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f, 2.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 2.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-0.5f, 2.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, 1.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

		-0.5f, 1.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		0.5f, 1.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f, 2.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 2.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-0.5f, 2.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, 1.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

		-0.5f, 2.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		-0.5f, 2.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		-0.5f, 1.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, 1.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, 1.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, 2.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

		0.5f, 2.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f, 2.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 1.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5f, 1.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5f, 1.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		0.5f, 2.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

		-0.5f, 1.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5f, 1.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 1.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f, 1.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		-0.5f, 1.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, 1.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

		-0.5f, 2.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		0.5f, 2.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 2.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		0.5f, 2.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
		-0.5f, 2.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, 2.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	//debugShaders(vertexShader, fragmentShader);

	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);

	GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

	GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	GLuint ebo;
	glGenBuffers(1, &ebo);

	GLuint elements[] = { 0, 1, 2, 0, 2, 3 };

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	GLuint textures[4];
	glGenTextures(4, textures);

	int width, height;
	unsigned char* image;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	image = SOIL_load_image("arransmile.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	image = SOIL_load_image("arranbadass.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	image = SOIL_load_image("arransad.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	image = SOIL_load_image("arransad.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 3);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLint uniTrans = glGetUniformLocation(shaderProgram, "model");
	GLint uniGunTrans = glGetUniformLocation(shaderProgram, "model");
	GLint uniBulletTrans = glGetUniformLocation(shaderProgram, "model");
	GLint uniGrenadeTrans = glGetUniformLocation(shaderProgram, "model");
	GLint uniView = glGetUniformLocation(shaderProgram, "view");
	GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
	GLint uniWater = glGetUniformLocation(shaderProgram, "water");
	GLint uniColor = glGetUniformLocation(shaderProgram, "overrideColor");
	GLint uniGunColor = glGetUniformLocation(shaderProgram, "overrideColor");
	//GLint uniScale = glGetUniformLocation(shaderProgram, "scale");

	glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 1.0f, 10.0f);

	glm::mat4 view = glm::lookAt(
		glm::vec3(0.0f, 2.5f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f)
	);

	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	bool bulletMoving = false;
	bool grenadeMoving = false;
	double initCursorX = 0.0, initCursorY = 0.0;
	//glfwGetCursorPos(window, &initCursorX, &initCursorY);

	glm::mat4 gunTrans = glm::scale(gunTrans, 0.167f * glm::vec3(3.0f, 1.0f, 1.0f));


	while (true)
	{

        SDL_Event windowEvent;
        if (SDL_PollEvent(&windowEvent))
        {
            if (windowEvent.type == SDL_QUIT) break;
            if (windowEvent.type == SDL_KEYDOWN)
            {
                if (windowEvent.key.keysym.sym == SDLK_ESCAPE) break;

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
			std::cout << "PEW" << std::endl;
		}
        if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT))
		{
			grenadeMoving = true;
			grenadeXPos   = xPos + 1.4f * cos(viewAngleHoriz - 0.0f) * cos(viewAngleVert - 0.4f);
			grenadeYPos   = yPos + 1.4f * sin(viewAngleHoriz - 0.0f) * cos(viewAngleVert - 0.4f);
			grenadeZPos   = zPos + 1.4f * sin(viewAngleVert - 0.4f);
			grenadeVec    = viewVec;
			grenadeVertVel = 0.05 * grenadeVec.z;
			std::cout << "BOOM" << std::endl;
		}
        /*
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			break;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			yPos += moveSpeed * sin(viewAngleHoriz);
			xPos += moveSpeed * cos(viewAngleHoriz);

		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			yPos -= moveSpeed * sin(viewAngleHoriz);
			xPos -= moveSpeed * cos(viewAngleHoriz);
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			yPos += strafeSpeed * sin(1.5708f + viewAngleHoriz);
			xPos += strafeSpeed * cos(1.5708f + viewAngleHoriz);
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			yPos -= strafeSpeed * sin(1.5708f + viewAngleHoriz);
			xPos -= strafeSpeed * cos(1.5708f + viewAngleHoriz);
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			zPos += moveSpeed;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			zPos -= moveSpeed;
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			bulletMoving = true;
			bulletXPos = xPos + 1.4f * cos(viewAngleHoriz - 0.0f) * cos(viewAngleVert - 0.4f);
			bulletYPos = yPos + 1.4f * sin(viewAngleHoriz - 0.0f) * cos(viewAngleVert - 0.4f);
			bulletZPos = zPos + 1.4f * sin(viewAngleVert - 0.4f);
			bulletVec = viewVec;
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			grenadeMoving = true;
			grenadeXPos   = xPos + 1.4f * cos(viewAngleHoriz - 0.0f) * cos(viewAngleVert - 0.4f);
			grenadeYPos   = yPos + 1.4f * sin(viewAngleHoriz - 0.0f) * cos(viewAngleVert - 0.4f);
			grenadeZPos   = zPos + 1.4f * sin(viewAngleVert - 0.4f);
			grenadeVec    = viewVec;
			grenadeVertVel = 0.05 * grenadeVec.z;
		}

		double currentCursorX, currentCursorY;
		glfwGetCursorPos(window, &currentCursorX, &currentCursorY);

		viewAngleHoriz += (float) (initCursorX - currentCursorX) * turnSpeed;
		initCursorX  =  currentCursorX;

		if (!((viewAngleVert > 3.0f / 2.0f && initCursorY > currentCursorY) || (viewAngleVert < -3.0f / 2.0f && initCursorY < currentCursorY)))
		{
			viewAngleVert += (float)(initCursorY - currentCursorY) *  turnSpeed;
		}
			initCursorY = currentCursorY;
        */
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


        SDL_GL_SwapWindow(window);
		//glfwPollEvents();

		glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);

		//debug===================================================================================================
		std::cout << "X " << xPos << std::endl << "Y " << yPos << std::endl << "Z " << zPos << std::endl;
		//setup===================================================================================================
		//========================================================================================================

		//set camera
		glm::mat4 view = glm::lookAt(
			glm::vec3(xPos, yPos, zPos),
			glm::vec3(xPOV, yPOV, zPOV),
			glm::vec3(0.0f, 0.0f, 1.0f)
			);
		glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
		glUniform3f(uniColor, 1.0f, 1.0f, 1.0f);


		/*
		glm::mat4 trans;
		glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);
		glUniform3f(uniColor, 1.0f, 1.0f, 1.0f);
		glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(trans));
		glDrawArrays(GL_TRIANGLES, 0, 36);*/

		/*draw hat
		glDrawArrays(GL_TRIANGLES, 36, 6);
		glUniform3f(uniColor, 0.0f, 0.0f, 0.0f);
		trans = glm::translate(trans, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(trans));
		glDrawArrays(GL_TRIANGLES, 0, 36);*/

		/*draw name label
		trans = glm::translate(trans, glm::vec3(-0.5f, 0.0f, 1.2f));
		trans = glm::scale(trans, glm::vec3(1.0f, 1.0f, 0.2f));
		trans = glm::rotate(trans, 3.14f / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(trans));
		glDrawArrays(GL_TRIANGLES, 36, 6);*/

		/*draw floor
		trans = glm::mat4();
		glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 3);
		glUniform3f(uniColor, 1.0f, 1.0f, 1.0f);
		trans = glm::translate(trans, glm::vec3(0.0f, 0.0f, -3.0f));
		trans = glm::scale(trans, glm::vec3(20.0f, 20.0f, 1.0f));
		glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(trans));
		glDrawArrays(GL_TRIANGLES, 0, 36);*/

		/*draw gun
		glm::vec3 gunPos = glm::vec3(
			xPos + 1.4f * cos(viewAngleHoriz - 0.0f) * cos(viewAngleVert - 0.4f),
			yPos + 1.4f * sin(viewAngleHoriz - 0.0f) * cos(viewAngleVert - 0.4f),
			zPos + 1.4f * sin(viewAngleVert  - 0.4f));
	//	glm::vec3 gunPos = glm::vec3(xPOV, yPOV, zPOV);
		glm::mat4 gunTrans;
		glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 1);
		gunTrans = glm::translate(gunTrans, gunPos);
		gunTrans = glm::rotate(gunTrans, viewAngleHoriz, glm::vec3(0.0f, 0.0f, 1.0f));
		gunTrans = glm::rotate(gunTrans, -viewAngleVert, glm::vec3(0.0f, 1.0f, 0.0f));
		gunTrans = glm::scale(gunTrans, 0.167f * glm::vec3(3.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniGunTrans, 1, GL_FALSE, glm::value_ptr(gunTrans));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		*/
		/*draw bullet
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

*/
		/*draw grenade
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
		*/
	}

	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_QUIT;
	//glfwTerminate();
	return 0;
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
