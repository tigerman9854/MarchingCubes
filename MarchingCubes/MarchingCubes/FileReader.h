#pragma once
#include <fstream>

#include "DataStructures.h"

ScalarField* ReadFile(const std::string& filepath) {
	std::ifstream file(filepath);
	if (!file.is_open()) {
		return nullptr;
	}

	// Read metadata
	int vertexCount;
	float x, y, z, scalar;
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

	// Helper lambda to find a vertex
	auto findVertex = [=](const glm::vec3& pos) -> Vertex* {
		for (auto it : ret->vertices) {
			if (equal(it->pos, pos)) {
				return it;
			}
		}
		return nullptr;
	};

	const glm::vec3 offset1 = glm::vec3(1, 0, 0);
	const glm::vec3 offset2 = glm::vec3(1, 0, 1);
	const glm::vec3 offset3 = glm::vec3(0, 0, 1);
	const glm::vec3 offset4 = glm::vec3(0, 1, 0);
	const glm::vec3 offset5 = glm::vec3(1, 1, 0);
	const glm::vec3 offset6 = glm::vec3(1, 1, 1);
	const glm::vec3 offset7 = glm::vec3(0, 1, 1);


	// Generate the cube data-structure
	for (auto it : ret->vertices) {
		// Search for 8 vertices to build this cube
		Vertex* const v0 = it;
		Vertex* const v1 = findVertex(v0->pos + offset1);
		Vertex* const v2 = findVertex(v0->pos + offset2);
		Vertex* const v3 = findVertex(v0->pos + offset3);
		Vertex* const v4 = findVertex(v0->pos + offset4);
		Vertex* const v5 = findVertex(v0->pos + offset5);
		Vertex* const v6 = findVertex(v0->pos + offset6);
		Vertex* const v7 = findVertex(v0->pos + offset7);

		// If all vertices were valid, add a cube to the data structure
		if (v0 && v1 && v2 && v3 && v4 && v5 && v6 && v7) {
			ret->cubes.push_back(new Cube({ v0, v1, v2, v3, v4, v5, v6, v7 }));
		}
	}

	return ret;
}