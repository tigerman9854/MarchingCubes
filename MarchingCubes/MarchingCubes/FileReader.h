#pragma once
#include <fstream>
#include <omp.h>
#include <vector>

#include "DataStructures.h"

// Define the number of threads OpenMP will use
const int threadCount = 24;

// Forward declare the marching cubes function (this is where the main work is done)
void MarchingCubes(ScalarField*);

ScalarField* ReadFile(const std::string& filepath) {
	std::ifstream file(filepath);
	if (!file.is_open()) {
		return nullptr;
	}

	// Read metadata (min, max, step, vertex count)
	int vertexCount;
	float x, y, z, scalar;
	file >> x >> y >> z;
	glm::vec3 min(x, y, z);
	file >> x >> y >> z;
	glm::vec3 max(x, y, z);
	file >> x >> y >> z;
	glm::vec3 step(x, y, z);
	file >> vertexCount;

	// Allocate a new scalar field
	ScalarField* ret = new ScalarField();

	// Read all vertices
	for (int i = 0; i < vertexCount; ++i) {
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

	const glm::vec3 offset1 = glm::vec3(1, 0, 0) * step;
	const glm::vec3 offset2 = glm::vec3(1, 0, 1) * step;
	const glm::vec3 offset3 = glm::vec3(0, 0, 1) * step;
	const glm::vec3 offset4 = glm::vec3(0, 1, 0) * step;
	const glm::vec3 offset5 = glm::vec3(1, 1, 0) * step;
	const glm::vec3 offset6 = glm::vec3(1, 1, 1) * step;
	const glm::vec3 offset7 = glm::vec3(0, 1, 1) * step;

	// Generate the cube data-structure
	// This is a slow n^2 operation so use OpenMP for parallel computing
	omp_set_num_threads(threadCount);
	std::vector<Cube*> tempCubeVectors[threadCount];

#pragma omp parallel for default(shared), schedule(dynamic, 10)
	for (int i = 0; i < (int)ret->vertices.size(); ++i) {
		const int threadNumber = omp_get_thread_num();

		// Search for the 8 vertices to build this cube
		Vertex* const v0 = ret->vertices[i];
		Vertex* const v1 = findVertex(v0->pos + offset1);
		Vertex* const v2 = findVertex(v0->pos + offset2);
		Vertex* const v3 = findVertex(v0->pos + offset3);
		Vertex* const v4 = findVertex(v0->pos + offset4);
		Vertex* const v5 = findVertex(v0->pos + offset5);
		Vertex* const v6 = findVertex(v0->pos + offset6);
		Vertex* const v7 = findVertex(v0->pos + offset7);

		// If all vertices were valid, add a cube to the data structure
		if (v0 && v1 && v2 && v3 && v4 && v5 && v6 && v7) {
			Cube* const c = new Cube({ v0, v1, v2, v3, v4, v5, v6, v7 });
			tempCubeVectors[threadNumber].push_back(c);
		}
	}

	// Use one thread to add all temp vectors to the main vector
	for (auto it : tempCubeVectors) {
		ret->cubes.insert(ret->cubes.end(), it.begin(), it.end());
	}



	// Apply the marching cubes algorithm to the scalar field
	// This is based on the original implementation from the paper
	MarchingCubes(ret);

	return ret;
}

void MarchingCubes(ScalarField* scalarField) 
{
	for (auto it : scalarField->cubes) {
		// Determine the index into the edge table which
		// tells us which vertices are inside of the surface
		int cubeType = 0;
		if (it->vertices[0]->val > 0) cubeType |= 1;
		if (it->vertices[1]->val > 0) cubeType |= 2;
		if (it->vertices[2]->val > 0) cubeType |= 4;
		if (it->vertices[3]->val > 0) cubeType |= 8;
		if (it->vertices[4]->val > 0) cubeType |= 16;
		if (it->vertices[5]->val > 0) cubeType |= 32;
		if (it->vertices[6]->val > 0) cubeType |= 64;
		if (it->vertices[7]->val > 0) cubeType |= 128;

		// Convert the cube type into a set of vertices
		std::vector<Vertex*> tempVertexStorage;
		for (int i = 0; triTable[cubeType][i] != -1; ++i) {
			const int edgeIndex = triTable[cubeType][i];
			Vertex* edge = it->edges[edgeIndex];
			tempVertexStorage.push_back(edge);

			// Generate a triangle if we have three vertices
			if (tempVertexStorage.size() == 3) {
				Triangle* t = new Triangle(tempVertexStorage);
				it->marchingCubesResults.push_back(t);
				tempVertexStorage.clear();
			}
		}
	}
}
