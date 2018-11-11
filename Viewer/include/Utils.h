#pragma once
#include <glm/glm.hpp>
#include <string>
#include "MeshModel.h"

/*
 * Utils class.
 * This class is consisted of static helper methods that can have many clients across the code.
 */
class Utils
{
public:
	static glm::vec3 Vec3fFromStream(std::istream& issLine);
	static glm::vec2 Vec2fFromStream(std::istream& issLine);
	static MeshModel LoadMeshModel(const std::string& filePath);
	static glm::vec4 Utils::Vec3to4(const glm::vec3 vertex);
	static std::vector<glm::vec4> Vec3to4Xmat(const std::vector<glm::vec3> vertices, const glm::mat4 matrix = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1,
		});
	static glm::vec3 Utils::Vertex4to3(const glm::vec4 vertex);
	static std::vector<glm::vec3> Vec4to3(const std::vector<glm::vec4> vertices);
	static glm::mat4 Utils::GetMatrix(std::string transformation, float a, float b, float c) ;

	// Add here more static utility functions...
	// For example:
	//	1. function that gets an angle, and returns a rotation matrix around a certian axis
	//	2. function that gets a vector, and returns a translation matrix
	//	3. anything else that may be useful in global scope

private:
	static std::string GetFileName(const std::string& filePath);
};
