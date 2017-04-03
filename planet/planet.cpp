// planet.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include"GL\glew.h"
#include"GLFW\glfw3.h"

#include"shader.h"
#include"src\SOIL.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include<math.h>
bool keys[2015];//store keyborad state
#define pi (3.1415926)
GLfloat WIDTH = 1920, HEIGHT = 1200;//setting the size of window
const int frag = 30;//Set the angle of each stroke of painting round
const int layer = 60;//set how many layers round of a ball
bool firstMouse = true;
glm::mat4 view;//when keyboard or mouse action, we change the view of inspector
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

GLfloat yaw = -90.0f;	// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
GLfloat pitch = 0.0f;
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
GLuint VBO[2], VAO[2], EBO[2];
//create the index of the points of a ball
void get_index(GLuint index[]) {
	for (int j = 0; j < layer; j++)
	{
		int i = 0;
		for ( i = 1; i < frag; i++)
		{
			index[3 * i + j*(frag)*3] = i + 1 + (j-1)*(frag + 1);
			//index[3 * i +j*(frag)*3	] = 0+j*(frag+1);
			index[3 * i + j*(frag)*3+ 1 ] = i + 1 + j*(frag+1);
			index[3 * i + j*(frag)*3+ 2 ] = i + 2 + j*(frag+1);
		}
		index[3 * (i-1) +j*(frag)*3+2] = (i-1)+1 + j*(frag+1);
	}
	for (int j = 0; j < layer; j++)
	{
		int i = 0;
		for (i = 0; i < frag-1; i++)
		{
			index[3 * i + j*(frag) * 3+ (frag) * 3 * layer] = i + 2 + (j + 1)*(frag + 1);
			//index[3 * i +j*(frag)*3	] = 0+j*(frag+1);
			index[3 * i + j*(frag) * 3 + 1 + (frag) * 3 * layer] = i + 1 + j*(frag + 1);
			index[3 * i + j*(frag) * 3 + 2 + (frag) * 3 * layer] = i + 2 + j*(frag + 1);
		}
		index[3 * (i - 1) + j*(frag) * 3 + 2 + (frag) * 3 * layer] = (i - 1) + 1 + j*(frag + 1);
	}
}
//create the points position and texture points of the ball
void get_cir(GLfloat a[],GLfloat r) {
	GLfloat lay = -90;//每层的角度
	for(int j=0;j<layer;j++){
		lay += 360.0 / layer;
		GLfloat l = lay*pi / 180;
		int ratio = -180;//初始化最开始的角度
		for (int i = 0; i < 5; i++)a[j*(frag+1) * 5 + i] = 0;//初始化圆心
		a[j*(frag + 1) * 5 + 2] = r*sin(l);
		for (int i = 1; i < frag + 1; i++) {
			ratio += 360 / frag;
			GLfloat rat = (ratio)*pi / 180;
			
			a[j*(frag+1 ) * 5 + i * 5] = r*cos(rat)*cos(l);
			a[j*(frag+1 ) * 5 + i * 5 + 1] = r*sin(rat)*cos(l);
			a[j*(frag+1 ) * 5 + i * 5 + 2] = r*sin(l);
			a[j*(frag+1 ) * 5 + i * 5 + 3] = r*cos(rat)*cos(l);
			a[j*(frag+1 ) * 5 + i * 5 + 4] = r*sin(rat)*cos(l);
		}	
	}
}
// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

//mouse move
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	GLfloat sensitivity = 0.005;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;
	//set the max angle
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = -sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}
void do_movement()
{
	// Camera controls
	GLfloat cameraSpeed = 0.001f;
	if (keys[GLFW_KEY_W])
		cameraPos -= cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_S])
		cameraPos += cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_A])
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keys[GLFW_KEY_D])
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}
int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "planet", nullptr, nullptr);

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glewExperimental = GL_TRUE;
	glewInit();
	glViewport(0, 0, WIDTH, HEIGHT);
	Shader ourshader("shader.vs", "shader.frag");
	GLfloat circle[(frag+1)*5*layer];
	GLuint index[(frag)*3*layer*2];
	get_index(index);
	get_cir(circle,0.5);
	//set the sky
	GLfloat sky[] = {
		-1,-1,-10,-1,-1,
		1,-1,-10,1,-1,
		1,1,-10,1,1,
		-1,1,-10,-1,1
	};
	GLuint index_sky[] = {
		0,1,2,
		2,3,0
	};
	//VAO[0] for ball and VAO[1] for sky
	glGenVertexArrays(2, VAO);
	glGenBuffers(2, VBO);
	glGenBuffers(2, EBO);
	
	glBindVertexArray(VAO[0]);//告诉gpu我们要操作vao
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);//将vbo绑定到这个vao上
	glBufferData(GL_ARRAY_BUFFER, sizeof(circle), circle, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//texture
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//sky vertex array object
	glBindVertexArray(VAO[1]);//告诉gpu我们要操作vao
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);//将vbo绑定到这个vao上
	glBufferData(GL_ARRAY_BUFFER, sizeof(sky), sky, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_sky), index_sky, GL_STATIC_DRAW);
	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//texture
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	//sun texture
	GLuint texture[5];
	glGenTextures(5, texture);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char *image = SOIL_load_image("sun.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
	//planet1 texture
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image("planet.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	//planet2 texture
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image("planet.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);


	//satell texture
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image("image.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	//sky texture
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image("sky.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		do_movement();
		view = glm::lookAt(cameraPos, cameraFront, cameraUp);
		glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ourshader.Use();
		GLuint temp;
		//draw sun
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glUniform1i(glGetUniformLocation(ourshader.Program, "ourTexture"), 0);
		glm::mat4 model;
		model = glm::rotate(model, 90.f, glm::vec3(1.0f, 1.0f, 0.0f));
		glm::mat4 view0;
		view0 = view*view0;//connect the action and object
		view0 = glm::translate(view0, glm::vec3(0.0f, 0.0f, -3.0f));
		glm::mat4 projection;
		projection = glm::perspective(45.f, WIDTH / HEIGHT, 0.1f, 100.0f);
		temp = glGetUniformLocation(ourshader.Program, "view");
		glUniformMatrix4fv(temp, 1, GL_FALSE, glm::value_ptr(view0));
		temp = glGetUniformLocation(ourshader.Program, "projection");
		glUniformMatrix4fv(temp, 1, GL_FALSE, glm::value_ptr(projection));
		temp = glGetUniformLocation(ourshader.Program, "model");
		glUniformMatrix4fv(temp, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(VAO[0]);
		glDrawElements(GL_TRIANGLES, (frag ) * 3 * layer*2, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		/********************************************/
		//planet 1
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture[1]);
		glUniform1i(glGetUniformLocation(ourshader.Program, "ourTexture"), 1);
		glm::mat4 model_p1;
		model_p1 = glm::rotate(model_p1, 60.f, glm::vec3(1.0f, .0f, 0.0f));
		model_p1 = glm::translate(model_p1, glm::vec3(2*cos(glfwGetTime()), 0, 2* sin(glfwGetTime())));
		model_p1 = glm::scale(model_p1, glm::vec3(0.7, 0.7, 0.7));
		model_p1 = glm::rotate(model_p1, (GLfloat)glfwGetTime() * 50, glm::vec3(1.0f, 1.0f, 0.0f));
		glm::mat4 view_p1;
		view_p1 = view*view_p1;
		view_p1 = glm::translate(view_p1, glm::vec3(0.0f, 0.0f, -6.0f));
		glm::mat4 projection_p1;
		projection_p1 = glm::perspective(45.f, WIDTH / HEIGHT, 0.1f, 100.0f);
		temp = glGetUniformLocation(ourshader.Program, "view");
		glUniformMatrix4fv(temp, 1, GL_FALSE, glm::value_ptr(view_p1));
		temp = glGetUniformLocation(ourshader.Program, "projection");
		glUniformMatrix4fv(temp, 1, GL_FALSE, glm::value_ptr(projection_p1));
		temp = glGetUniformLocation(ourshader.Program, "model");
		glUniformMatrix4fv(temp, 1, GL_FALSE, glm::value_ptr(model_p1));
		glBindVertexArray(VAO[0]);
		glDrawElements(GL_TRIANGLES, (frag) * 3 * layer * 2, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		//planet 2
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texture[2]);
		glUniform1i(glGetUniformLocation(ourshader.Program, "ourTexture"), 2);
		glm::mat4 model_p2;

		model_p2 = glm::translate(model_p2, glm::vec3(3, 3, 0));
		model_p2 = glm::scale(model_p2, glm::vec3(0.5, 0.5, 0.5));
		model_p2 = glm::rotate(model_p2, (GLfloat)glfwGetTime() * 50, glm::vec3(1.0f, 1.0f, 0.0f));
		glm::mat4 view_p2;
		view_p2 = view*view_p2;
		view_p2 = glm::translate(view_p2, glm::vec3(0.0f, 0.0f, -6.0f));
		view_p2 = glm::rotate(view_p2, (GLfloat)glfwGetTime() * 80, glm::vec3(-1.0, 1.0f, 0.0f));
		glm::mat4 projection_p2;
		projection_p2 = glm::perspective(45.f, WIDTH / HEIGHT, 0.1f, 100.0f);
		temp = glGetUniformLocation(ourshader.Program, "view");
		glUniformMatrix4fv(temp, 1, GL_FALSE, glm::value_ptr(view_p2));
		temp = glGetUniformLocation(ourshader.Program, "projection");
		glUniformMatrix4fv(temp, 1, GL_FALSE, glm::value_ptr(projection_p2));
		temp = glGetUniformLocation(ourshader.Program, "model");
		glUniformMatrix4fv(temp, 1, GL_FALSE, glm::value_ptr(model_p2));
		glBindVertexArray(VAO[0]);
		glDrawElements(GL_TRIANGLES, (frag) * 3 * layer * 2, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		//planet 3
		
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, texture[3]);
		glUniform1i(glGetUniformLocation(ourshader.Program, "ourTexture"), 3);
		glm::mat4 model_p3;

		model_p3 = glm::rotate(model_p3, 60.f, glm::vec3(1.0f, .0f, 0.0f));
		model_p3 = glm::translate(model_p3, glm::vec3(2 * cos(glfwGetTime()), 0, 2 * sin(glfwGetTime())));
		model_p3 = glm::rotate(model_p3, (GLfloat)glfwGetTime() * 200, glm::vec3(1.0f, -1.0f, .0f));
		model_p3 = glm::translate(model_p3, glm::vec3(0.6, 0.6, 0));
		model_p3 = glm::rotate(model_p3, (GLfloat)glfwGetTime() * 80, glm::vec3(1.0f, 1.0f, 0.0f));
		model_p3 = glm::scale(model_p3, glm::vec3(0.3, 0.3, 0.3));
		model_p3 = glm::rotate(model_p3, (GLfloat)glfwGetTime() * 50, glm::vec3(-1.f, 1.0f, 0.0f));		
		glm::mat4 view_p3;
		view_p3 = view*view_p3;
		view_p3 = glm::translate(view_p3, glm::vec3(0.0f, 0.0f, -6.0f));
		glm::mat4 projection_p3;
		projection_p3 = glm::perspective(45.f, WIDTH / HEIGHT, 0.1f, 100.0f);
		temp = glGetUniformLocation(ourshader.Program, "view");
		glUniformMatrix4fv(temp, 1, GL_FALSE, glm::value_ptr(view_p3));
		temp = glGetUniformLocation(ourshader.Program, "projection");
		glUniformMatrix4fv(temp, 1, GL_FALSE, glm::value_ptr(projection_p3));
		temp = glGetUniformLocation(ourshader.Program, "model");
		glUniformMatrix4fv(temp, 1, GL_FALSE, glm::value_ptr(model_p3));
		glBindVertexArray(VAO[0]);
		glDrawElements(GL_TRIANGLES, (frag) * 3 * layer * 2, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		//sky
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, texture[4]);
		glUniform1i(glGetUniformLocation(ourshader.Program, "ourTexture"), 4);
		glm::mat4 model_sky;
		model_sky = glm::scale(model_sky, glm::vec3(10, 10, 0));
		glm::mat4 view_sky;
		view_sky = glm::translate(view_sky, glm::vec3(0, 0, -10));
		view_sky = view*view_sky;
		glm::mat4 projection_sky;
		projection_sky = glm::perspective(45.f, WIDTH / HEIGHT, 0.1f, 100.0f);
		temp = glGetUniformLocation(ourshader.Program, "view");
		glUniformMatrix4fv(temp, 1, GL_FALSE, glm::value_ptr(view_sky));
		temp = glGetUniformLocation(ourshader.Program, "projection");
		glUniformMatrix4fv(temp, 1, GL_FALSE, glm::value_ptr(projection_sky));
		temp = glGetUniformLocation(ourshader.Program, "model");
		glUniformMatrix4fv(temp, 1, GL_FALSE, glm::value_ptr(model_sky));
		glBindVertexArray(VAO[1]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glfwSwapBuffers(window);
	}
	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);
	glDeleteBuffers(2, EBO);
	glfwTerminate();
	return 0;
}

