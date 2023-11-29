#pragma once
#include "Header.h"

bool Load_Object(const char* path, std::vector<glm::vec3>& temp_vertices, std::vector<glm::vec2>& temp_uvs, std::vector<glm::vec3>& temp_normals, std::vector<glm::vec3>& vertices,
	std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals, std::vector<unsigned int>& vertexIndices, std::vector<unsigned int>& uvIndices, std::vector<unsigned int>& normalIndices);
char* filetobuf(const char* file);
