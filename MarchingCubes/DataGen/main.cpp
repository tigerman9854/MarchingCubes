#include <fstream>

#include "glm/glm.hpp"


void ExportScalarFieldCSV(const glm::vec3& minPosition, const glm::vec3& maxPosition, const glm::vec3& step, const std::string& filename, float function(const glm::vec3&)) {
	// Warning: No error checking on input

	// Format:
	// minx, miny, minz
	// maxx, maxy, maxz
	// stepx, stepy, stepz
	// vertexCount
	// x1, y1, z1, scalar1
	// x2, y2, z2, scalar2
	//     ...
	// xn, yn, zn, scalarn

	// Open a txt file to print the results
	std::ofstream file;
	file.open(filename);

	if (!file.is_open()) {
		return;
	}

	// Print the parameters
	file << minPosition.x << " " << minPosition.y << " " << minPosition.z << "\n";
	file << maxPosition.x << " " << maxPosition.y << " " << maxPosition.z << "\n";
	file << step.x << " " << step.y << " " << step.z << "\n";

	// Count the number of points we will be generating
	const glm::vec3 counts = (maxPosition - minPosition) / step + glm::vec3(1, 1, 1);
	const int count = (int)counts.x * (int)counts.y * (int)counts.z;
	file << count << "\n";

	// Loop through all 3 dimensions with <step> distance between each point
	for (float x = minPosition.x; x <= maxPosition.x; x += step.x) {
		for (float y = minPosition.y; y <= maxPosition.y; y += step.y) {
			for (float z = minPosition.z; z <= maxPosition.z; z += step.z) {

				// Evaluate the function at this point
				const float value = function({ x, y, z });

				// Add the evaulated point as a new line in the output file
				file << x << " " << y << " " << z << " " << value << "\n";
			}
		}
	}

	// Cleanup and close the file
	file.close();
}



int main() {
	// Define the boundaries of the scalar field
	const glm::vec3 min = { -10, -10, -10 };
	const glm::vec3 max = { 10, 10, 10 };
	const glm::vec3 step = { 1, 1, 1 };


	auto sphereFunc = [](const glm::vec3& pos) {
		// x^2 + y^2 + z^2 - r^2 = 0
		const float radius = 8.1f;
		return glm::dot(pos, pos) - radius * radius;
	};

	auto saddleFunc = [](const glm::vec3& pos) {
		// x^2 - z^2 - 5.1y = 0
		return pos.x * pos.x - pos.z * pos.z - pos.y * 5.1f;
	};
	
	auto coneFunc = [](const glm::vec3& pos) {
		// x^2 + y^2 - z = 0
		const float offset = 5.0;
		return (pos.x * pos.x) + (pos.y * pos.y) - pos.z - offset;
	};

	auto parabolaFunc = [](const glm::vec3& pos) {
		// x^2/a^2 + y^2/b^2 - z = 0
		const float a = 2.0;
		const float b = 2.0;
		const float offset = 5.0;
		return (pos.x * pos.x) / (a * a) + (pos.y * pos.y) / (b * b) - pos.z - offset;
	};

	auto chairFunc = [](const glm::vec3& pos) {
		// z * (x + y) = 0
		return pos.z * (pos.x + pos.y);
	};

	auto torusFunc = [](const glm::vec3& pos) {
		// sqrt(x^2 + y^2) + z^3 - z^2 = 0
		const float min_r = 3.0;
		const float max_r = 4.5;
		return powf((sqrt((pos.x * pos.x) + (pos.y * pos.y)) - max_r), 2) + pos.z * pos.z - min_r * min_r;
		//return (4.0 * max_r * max_r) * ((pos.x * pos.x) + (pos.x * pos.x));
	};

	auto cylinderFunc = [](const glm::vec3& pos) {
		// sqrt(x^2 + y^2) + z^3 - z^2 = 0
		const float r2 = 4.5;
		return (pos.x * pos.x + pos.y * pos.y - r2 * r2);
	};

	auto curveFunc = [](const glm::vec3& pos) {
		// x^3 - y^2 - z = 0
		return pos.x * pos.x * pos.x - pos.y * pos.y - pos.z;
	};

	auto curve2Func = [](const glm::vec3& pos) {
		// x^3 + y^2z + xz^2 + x^2 = 0
		return pos.x * pos.x * pos.x + pos.y * pos.y * pos.z + pos.x * pos.z * pos.z + pos.x * pos.x;
	};


	ExportScalarFieldCSV(min, max, step, "../Output/sphere.txt", sphereFunc);
	ExportScalarFieldCSV(min, max, step, "../Output/saddle.txt", saddleFunc);
	ExportScalarFieldCSV(min, max, step, "../Output/cone.txt", coneFunc);
	ExportScalarFieldCSV(min, max, step, "../Output/parabola.txt", parabolaFunc);
	ExportScalarFieldCSV(min, max, step, "../Output/chair.txt", chairFunc);
	ExportScalarFieldCSV(min, max, step, "../Output/torus.txt", torusFunc);
	ExportScalarFieldCSV(min, max, step, "../Output/cylinder.txt", cylinderFunc);
	ExportScalarFieldCSV(min, max, step, "../Output/curve.txt", curveFunc);
	ExportScalarFieldCSV(min, max, step, "../Output/curve2.txt", curve2Func);

	return 0;
}
