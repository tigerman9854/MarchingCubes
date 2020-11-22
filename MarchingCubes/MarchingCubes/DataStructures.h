#pragma once
#include <vector>
#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 pos;
	float val;

	Vertex(glm::vec3 p, float v) :
		pos(p), val(v) { }
};

struct Edge {
	std::vector<Vertex*> vertices;
};

// From http://www.cs.carleton.edu/cs_comps/0405/shape/marching_cubes.html
//                 v7_______e6_____________v6
//                  /|                    /|
//                 / |                   / |
//              e7/  |                e5/  |
//               /___|______e4_________/   |
//            v4|    |                 |v5 |e10
//              |    |                 |   |
//              |    |e11              |e9 |
//            e8|    |                 |   |
//              |    |_________________|___|
//              |   / v3      e2       |   /v2
//              |  /                   |  /
//              | /e3                  | /e1
//              |/_____________________|/
//              v0         e0          v1

struct Cube {
	std::vector<Vertex*> vertices;
	std::vector<Edge*> edges;
};

struct ScalarField {
	std::vector<Vertex*> vertices;
	std::vector<Cube*> cubes;

	// Scalar field owns its cubes
	~ScalarField() {
		for (auto it : vertices) {
			delete it;
		}
		for (auto it : cubes) {
			delete it;
		}
	}
};