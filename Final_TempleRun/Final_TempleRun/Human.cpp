#define _CRT_SECURE_NO_WARNINGS
#include "Human.h"

void Head::transform() {
	TR = glm::mat4(1.0f);
	TR = glm::translate(TR, glm::vec3(0, 0, 0)); //--- x축으로 이동 행렬
	TR = glm::rotate(TR, glm::radians(0.0f), glm::vec3(1.0, 0.0, 0.0)); //--- x축에 대하여 회전 행렬
	TR = glm::rotate(TR, glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0)); //--- y축에 대하여 회전 행렬
	TR = glm::scale(TR, glm::vec3(0.01, 0.01, 0.01));
}

void Head::draw(GLuint vao, unsigned int modelLocation) {
	Head::transform();
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 1000);
}

void Body::transform() {
	TR = glm::mat4(1.0f);
	TR = glm::translate(TR, glm::vec3(0, -0.23, 0)); //--- x축으로 이동 행렬
	TR = glm::rotate(TR, glm::radians(0.0f), glm::vec3(1.0, 0.0, 0.0)); //--- x축에 대하여 회전 행렬
	TR = glm::rotate(TR, glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0)); //--- y축에 대하여 회전 행렬
	TR = glm::scale(TR, glm::vec3(0.01, 0.01, 0.01));
}

void Body::draw(GLuint vao, unsigned int modelLocation) {
	Body::transform();
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 1000);
}

void Arm::check_right(int i) {
	if (int i = 0)
		is_right = true;
	else
		is_right = false;
}

void Arm::draw(GLuint vao, unsigned int modelLocation) {
	//Arm::transform();
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 1000);
}

void Leg::check_right(int i) {
	if (int i = 0)
		is_right = true;
	else
		is_right = false;
}

void Leg::draw(GLuint vao, unsigned int modelLocation) {
	//Leg::transform();
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR)); //--- modelTransform 변수에 변환 값 적용하기
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 1000);
}

Human::Human() {
	right_arm.check_right(0);
	left_arm.check_right(1);
	right_leg.check_right(0);
	left_leg.check_right(1);
}

void Human::draw(GLuint head_vao, GLuint body_vao, GLuint right_arm_vao, GLuint left_arm_vao, GLuint right_leg_vao, GLuint left_leg_vao, unsigned int modelLocation) {
	head.draw(head_vao, modelLocation);
	body.draw(body_vao, modelLocation);
	/*right_arm.draw(right_arm_vao, modelLocation);
	left_arm.draw(left_arm_vao, modelLocation);
	right_leg.draw(right_leg_vao, modelLocation);
	left_leg.draw(left_leg_vao, modelLocation);*/
}
