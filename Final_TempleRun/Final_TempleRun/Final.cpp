#define _CRT_SECURE_NO_WARNINGS
#include "Header.h"

struct Shape {
	float colors[1000][3] = {
	};
	GLuint vbo[2], vao, ebo;
};

Shape s[6];

std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;

std::vector< glm::vec3 > vertices;
std::vector< glm::vec2 > uvs;
std::vector< glm::vec3 > normals;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 2.0f); //--- 카메라 위치
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향

GLfloat radius = 0.1f;
GLfloat rColor = 1, gColor = 1, bColor = 1;

GLint width, height;
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;
GLchar* vertexSource, * fragmentSource;

/*OPGL관렴 함수*/
GLvoid drawScene();
GLvoid Reshape(int w, int h);

/*셰이더 관련 함수*/
void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();

/*vao, vbo 관련 함수*/
void Initvbovao();
void Draw();

char* filetobuf(const char* file)
{
	ifstream in(file, ios_base::binary);

	if (!in) {
		cerr << file << "파일 못찾음";
		exit(1);
	}

	in.seekg(0, ios_base::end);
	long len = in.tellg();
	char* buf = new char[len + 1];
	in.seekg(0, ios_base::beg);

	int cnt = -1;
	while (in >> noskipws >> buf[++cnt]) {}
	buf[len] = 0;

	return buf;
}
bool  Load_Object(const char* path) {
	vertexIndices.clear();
	uvIndices.clear();
	normalIndices.clear();
	vertices.clear();
	uvs.clear();
	normals.clear();

	ifstream in(path);
	if (!in) {
		cerr << path << "파일 못찾음";
		exit(1);
	}

	//vector<char> lineHeader(istream_iterator<char>{in}, {});

	while (in) {
		string lineHeader;
		in >> lineHeader;
		if (lineHeader == "v") {
			glm::vec3 vertex;
			in >> vertex.x >> vertex.y >> vertex.z;
			vertices.push_back(vertex);
		}
		else if (lineHeader == "vt") {
			glm::vec2 uv;
			in >> uv.x >> uv.y;
			uvs.push_back(uv);
		}
		else if (lineHeader == "vn") {
			glm::vec3 normal;
			in >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		}
		else if (lineHeader == "f") {
			char a;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];

			for (int i = 0; i < 3; i++)
			{
				in >> vertexIndex[i] >> a >> uvIndex[i] >> a >> normalIndex[i];
				vertexIndices.push_back(vertexIndex[i] - 1);
				uvIndices.push_back(uvIndex[i] - 1);
				normalIndices.push_back(normalIndex[i] - 1);
			}
		}
	}

	return true;
}

int main(int argc, char** argv) {

	srand(time(NULL));

	glutInit(&argc, argv);
	width = 800, height = 600;
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);

	glutCreateWindow("Cube");
	glewExperimental = GL_TRUE;
	glewInit();

	/*초기화 함수*/
	make_shaderProgram();
	Initvbovao();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutMainLoop();

	return 0;
}

GLvoid drawScene() {
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glUseProgram(shaderProgramID);

	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform"); //--- 뷰잉 변환 설정
	unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform"); //--- 투영 변환 값 설정

	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	projection = glm::perspective(glm::radians(30.0f), 1.0f, 0.1f, 50.0f);
	projection = glm::translate(projection, glm::vec3(0.0, 0.0, -1.0));
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

	/*그리기*/
	Draw();
	glutSwapBuffers(); //--- 화면에 출력하기
}

GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

void Initvbovao()
{
	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color");

	//머리
	Load_Object("head.obj");
	{
		for (int i = 0; i < 1000; i++) {
			s[0].colors[i][0] = 0.1;
			s[0].colors[i][1] = 0.5;
			s[0].colors[i][2] = 1.0;
		}

		glGenVertexArrays(1, &s[0].vao);
		glGenBuffers(2, s[0].vbo);
		glGenBuffers(1, &s[0].ebo);

		glBindVertexArray(s[0].vao);

		glBindBuffer(GL_ARRAY_BUFFER, s[0].vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, s[0].vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(s[0].colors), s[0].colors, GL_STATIC_DRAW);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s[0].ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(unsigned int), &vertexIndices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(PosLocation);
		glEnableVertexAttribArray(ColorLocation);
	}

	Load_Object("body.obj");
	{
		for (int i = 0; i < 1000; i++) {
			s[1].colors[i][0] = 0.7;
			s[1].colors[i][1] = 0.4;
			s[1].colors[i][2] = 0.0;
		}

		glGenVertexArrays(1, &s[1].vao);
		glGenBuffers(2, s[1].vbo);
		glGenBuffers(1, &s[1].ebo);

		glBindVertexArray(s[1].vao);

		glBindBuffer(GL_ARRAY_BUFFER, s[1].vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, s[1].vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(s[1].colors), s[1].colors, GL_STATIC_DRAW);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s[1].ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(unsigned int), &vertexIndices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(PosLocation);
		glEnableVertexAttribArray(ColorLocation);
	}
	Load_Object("leftarm.obj");
	{
		for (int i = 0; i < 1000; i++) {
			s[2].colors[i][0] = 0.5;
			s[2].colors[i][1] = 0.5;
			s[2].colors[i][2] = 1.0;
		}

		glGenVertexArrays(1, &s[2].vao);
		glGenBuffers(2, s[2].vbo);
		glGenBuffers(1, &s[2].ebo);

		glBindVertexArray(s[2].vao);

		glBindBuffer(GL_ARRAY_BUFFER, s[2].vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, s[2].vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(s[2].colors), s[2].colors, GL_STATIC_DRAW);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s[2].ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(unsigned int), &vertexIndices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(PosLocation);
		glEnableVertexAttribArray(ColorLocation);
	}
	Load_Object("rightarm.obj");
	{
		for (int i = 0; i < 1000; i++) {
			s[3].colors[i][0] = 0.5;
			s[3].colors[i][1] = 0.5;
			s[3].colors[i][2] = 1.0;
		}

		glGenVertexArrays(1, &s[3].vao);
		glGenBuffers(2, s[3].vbo);
		glGenBuffers(1, &s[3].ebo);

		glBindVertexArray(s[3].vao);

		glBindBuffer(GL_ARRAY_BUFFER, s[3].vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, s[3].vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(s[3].colors), s[3].colors, GL_STATIC_DRAW);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s[3].ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(unsigned int), &vertexIndices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(PosLocation);
		glEnableVertexAttribArray(ColorLocation);
	}
	Load_Object("leftleg.obj");
	{
		for (int i = 0; i < 1000; i++) {
			s[4].colors[i][0] = 0.5;
			s[4].colors[i][1] = 0.5;
			s[4].colors[i][2] = 1.0;
		}

		glGenVertexArrays(1, &s[4].vao);
		glGenBuffers(2, s[4].vbo);
		glGenBuffers(1, &s[4].ebo);

		glBindVertexArray(s[4].vao);

		glBindBuffer(GL_ARRAY_BUFFER, s[4].vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, s[4].vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(s[4].colors), s[4].colors, GL_STATIC_DRAW);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s[4].ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(unsigned int), &vertexIndices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(PosLocation);
		glEnableVertexAttribArray(ColorLocation);
	}
	Load_Object("rightleg.obj");
	{
		for (int i = 0; i < 1000; i++) {
			s[5].colors[i][0] = 0.5;
			s[5].colors[i][1] = 0.5;
			s[5].colors[i][2] = 1.0;
		}

		glGenVertexArrays(1, &s[5].vao);
		glGenBuffers(2, s[5].vbo);
		glGenBuffers(1, &s[5].ebo);

		glBindVertexArray(s[5].vao);

		glBindBuffer(GL_ARRAY_BUFFER, s[5].vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, s[5].vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(s[5].colors), s[5].colors, GL_STATIC_DRAW);
		glVertexAttribPointer(ColorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s[5].ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(unsigned int), &vertexIndices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(PosLocation);
		glEnableVertexAttribArray(ColorLocation);
	}
}
void Draw() {
	int PosLocation = glGetAttribLocation(shaderProgramID, "in_Position");
	int ColorLocation = glGetAttribLocation(shaderProgramID, "in_Color");

	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform"); //--- 버텍스 세이더에서 모델링 변환 위치 가져오기


	//머리
	glm::mat4 TR = glm::mat4(1.0f); //--- 합성 변환 행렬

	{
		TR = glm::translate(TR, glm::vec3(0, 0, 0)); //--- x축으로 이동 행렬
		TR = glm::rotate(TR, glm::radians(0.0f), glm::vec3(1.0, 0.0, 0.0)); //--- x축에 대하여 회전 행렬
		TR = glm::rotate(TR, glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0)); //--- y축에 대하여 회전 행렬
		TR = glm::scale(TR, glm::vec3(0.01, 0.01, 0.01));

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR)); //--- modelTransform 변수에 변

		glBindVertexArray(s[0].vao);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, 1000, GL_UNSIGNED_INT, 0);
	}


	//몸
	glm::mat4 TR1 = glm::mat4(1.0f); //--- 합성 변환 행렬

	{
		TR1 = glm::translate(TR1, glm::vec3(0, -0.23, 0)); //--- x축으로 이동 행렬
		TR1 = glm::rotate(TR1, glm::radians(0.0f), glm::vec3(1.0, 0.0, 0.0)); //--- x축에 대하여 회전 행렬
		TR1 = glm::rotate(TR1, glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0)); //--- y축에 대하여 회전 행렬
		TR1 = glm::scale(TR1, glm::vec3(0.01, 0.01, 0.01));

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR1)); //--- modelTransform 변수에 변

		glBindVertexArray(s[1].vao);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, 1000, GL_UNSIGNED_INT, 0);
	}
	

	//왼팔
	glm::mat4 TR2 = glm::mat4(1.0f); //--- 합성 변환 행렬

	{
		TR2 = glm::translate(TR2, glm::vec3(0.07, -0.2, 0)); //--- x축으로 이동 행렬
		TR2 = glm::rotate(TR2, glm::radians(0.0f), glm::vec3(1.0, 0.0, 0.0)); //--- x축에 대하여 회전 행렬
		TR2 = glm::rotate(TR2, glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0)); //--- y축에 대하여 회전 행렬
		TR2 = glm::scale(TR2, glm::vec3(0.01, 0.01, 0.01));

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR2)); //--- modelTransform 변수에 변

		glBindVertexArray(s[2].vao);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, 1000, GL_UNSIGNED_INT, 0);
	}
	//오른팔
	glm::mat4 TR3 = glm::mat4(1.0f); //--- 합성 변환 행렬

	{
		TR3 = glm::translate(TR3, glm::vec3(-0.08, -0.2, 0)); //--- x축으로 이동 행렬
		TR3 = glm::rotate(TR3, glm::radians(0.0f), glm::vec3(1.0, 0.0, 0.0)); //--- x축에 대하여 회전 행렬
		TR3 = glm::rotate(TR3, glm::radians(-10.0f), glm::vec3(0.0, 1.0, 0.0)); //--- y축에 대하여 회전 행렬
		TR3 = glm::scale(TR3, glm::vec3(0.01, 0.01, 0.01));

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR3)); //--- modelTransform 변수에 변

		glBindVertexArray(s[3].vao);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, 1000, GL_UNSIGNED_INT, 0);
	}
	//왼다리
	glm::mat4 TR4 = glm::mat4(1.0f); //--- 합성 변환 행렬

	{
		TR4 = glm::translate(TR4, glm::vec3(0.04, -0.45, 0)); //--- x축으로 이동 행렬
		TR4 = glm::rotate(TR4, glm::radians(0.0f), glm::vec3(1.0, 0.0, 0.0)); //--- x축에 대하여 회전 행렬
		TR4 = glm::rotate(TR4, glm::radians(-10.0f), glm::vec3(0.0, 1.0, 0.0)); //--- y축에 대하여 회전 행렬
		TR4 = glm::scale(TR4, glm::vec3(0.01, 0.01, 0.01));

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR4)); //--- modelTransform 변수에 변

		glBindVertexArray(s[4].vao);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, 1000, GL_UNSIGNED_INT, 0);
	}
	//오른다리
	glm::mat4 TR5 = glm::mat4(1.0f); //--- 합성 변환 행렬

	{
		TR5 = glm::translate(TR5, glm::vec3(-0.04, -0.45, 0)); //--- x축으로 이동 행렬
		TR5 = glm::rotate(TR5, glm::radians(0.0f), glm::vec3(1.0, 0.0, 0.0)); //--- x축에 대하여 회전 행렬
		TR5 = glm::rotate(TR5, glm::radians(10.0f), glm::vec3(0.0, 1.0, 0.0)); //--- y축에 대하여 회전 행렬
		TR5 = glm::scale(TR5, glm::vec3(0.01, 0.01, 0.01));

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR5)); //--- modelTransform 변수에 변

		glBindVertexArray(s[5].vao);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, 1000, GL_UNSIGNED_INT, 0);
	}
}


void make_shaderProgram()
{
	make_vertexShaders(); //--- 버텍스 세이더 만들기
	make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	//-- shader Program
	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);
	//--- 세이더 삭제하기
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	//--- Shader Program 사용하기
	glUseProgram(shaderProgramID);
}

void make_vertexShaders()
{
	vertexSource = filetobuf("vertex.glsl");
	//--- 버텍스 세이더 객체 만들기
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	//--- 버텍스 세이더 컴파일하기
	glCompileShader(vertexShader);
	//--- 컴파일이 제대로 되지 않은 경우: 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShaders()
{
	fragmentSource = filetobuf("fragment.glsl");
	//--- 프래그먼트 세이더 객체 만들기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	//--- 프래그먼트 세이더 컴파일
	glCompileShader(fragmentShader);
	//--- 컴파일이 제대로 되지 않은 경우: 컴파일 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

//dfffffff