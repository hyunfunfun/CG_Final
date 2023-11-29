#pragma once
#include "Header.h"

class Head {
	glm::mat4 TR{ glm::mat4(1.0f) };
public:
	void draw(GLuint, unsigned int);
	void transform();
};	

class Arm {
	glm::mat4 TR{ glm::mat4(1.0f) };
	bool is_right;
public:
	void draw(GLuint, unsigned int);
	void check_right(int i);
	//void transform();
};

class Leg {
	glm::mat4 TR{ glm::mat4(1.0f) };
	bool is_right;
public:
	void draw(GLuint, unsigned int);
	void check_right(int i);
	//void transform();
};

class Body {
	glm::mat4 TR{ glm::mat4(1.0f) };
public:
	void draw(GLuint, unsigned int);
	void transform();
};

class Human {
	Head head;
	Body body;
	Arm left_arm;
	Arm right_arm;
	Leg left_leg;
	Leg right_leg;	
public:
	Human();
	void draw(GLuint head_vao, GLuint body_vao, GLuint right_arm_vao, GLuint left_arm_vao, GLuint right_leg_vao, GLuint left_leg_vao, unsigned int modelLocation);
};