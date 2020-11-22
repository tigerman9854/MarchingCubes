#include <fstream>

#include "glm/glm.hpp"


void ExportScalarFieldCSV(const glm::vec3& minPosition, const glm::vec3& maxPosition, const glm::vec3& step, const std::string& filename, float function(const glm::vec3&)) {
	// Warning: No error checking on input

	// Format:
	// minx, miny, minz
	// maxx, maxy, maxz
	// stepx, stepy, stepz
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
	const int count = counts.x * counts.y * counts.z;
	file << count << "\n";

	// Loop through all 3 dimensions with <step> distance between each point
	for (float x = minPosition.x; x <= maxPosition.x; x += step.x) {
		for (float y = minPosition.y; y <= maxPosition.x; y += step.y) {
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

	const glm::vec3 min = { -10, -10, -10 };
	const glm::vec3 max = { 10, 10, 10 };
	const glm::vec3 step = { 1, 1, 1 };

	ExportScalarFieldCSV(min, max, step, "../Output/sphere.txt", [](const glm::vec3& pos) {
		// x^2 + y^2 + z^2 = r^2
		const float radius = 8.f;
		if (glm::dot(pos, pos) <= radius * radius) {
			// Inside the sphere
			return 1.f;
		} 
		else {
			// Outside the sphere
			return 0.f;
		}
	});

	ExportScalarFieldCSV(min, max, step, "../Output/saddle.txt", [](const glm::vec3& pos) {
		// y = 1/5(x^2 - z^2)
		if (pos.x * pos.x - pos.z * pos.z - pos.y * 5.f <= 0) {
			// Below the saddle
			return 1.f;
		}
		else {
			// Above the saddle
			return 0.f;
		}
	});

	return 0;
}