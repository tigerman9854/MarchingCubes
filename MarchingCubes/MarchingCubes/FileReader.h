#pragma once
#include <fstream>

#include "DataStructures.h"

ScalarField* ReadFile(const std::string& filepath) {
	std::ifstream file(filepath);
	if (!file.is_open()) {
		return nullptr;
	}

	// Read metadata
	float x, y, z, scalar, vertexCount;
	file >> x >> y >> z;
	glm::vec3 min(x, y, z);
	file >> x >> y >> z;
	glm::vec3 max(x, y, z);
	file >> x >> y >> z;
	glm::vec3 step(x, y, z);
	file >> vertexCount;

	// Allocate a scalar field
	ScalarField* ret = new ScalarField();

	// Read all vertices
	for(int i = 0; i < vertexCount; ++i) {
		file >> x >> y >> z >> scalar;
		ret->vertices.push_back(new Vertex({ x,y,z }, scalar));
	}

	// Helper to find a vertex
	/*auto findVertex = [&](glm::vec3 pos) {

	};*/

	return ret;
}