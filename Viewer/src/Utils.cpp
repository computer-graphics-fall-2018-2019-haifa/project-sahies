#include "Utils.h"
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

glm::vec3 Utils::Vec3fFromStream(std::istream& issLine)
{
	float x, y, z;
	issLine >> x >> std::ws >> y >> std::ws >> z;
	return glm::vec3(x, y, z);
}

glm::vec2 Utils::Vec2fFromStream(std::istream& issLine)
{
	float x, y;
	issLine >> x >> std::ws >> y;
	return glm::vec2(x, y);
}

glm::vec4 Utils::Vec3to4(const glm::vec3 vertex)
{
	return glm::vec4(vertex[0], vertex[1], vertex[2], 1.0f);
}

std::vector<glm::vec4> Utils::Vec3to4Xmat(std::vector<glm::vec3> vertices, glm::mat4 matrix)
{
	std::vector<glm::vec4> vertices_tmp;

	for (glm::vec3 vertex : vertices) 
		vertices_tmp.push_back(matrix * Utils::Vec3to4(vertex));

	return vertices_tmp;

}

glm::vec3 Utils::Vertex4to3(const glm::vec4 vertex)
{
	return glm::vec3(vertex[0] / vertex[3], vertex[1] / vertex[3], vertex[2] / vertex[3]);
}

std::vector<glm::vec3> Utils::Vec4to3(const std::vector<glm::vec4> vertices)
{
	std::vector<glm::vec3> vertices_tmp;

	for (glm::vec4 vertex : vertices) 
		vertices_tmp.push_back(Utils::Vertex4to3(vertex));
	
	return vertices_tmp;
}

glm::mat4 Utils::GetMatrix(std::string transformation, glm::vec3 cordinates)
{
	glm::mat4 mat, matX, matY, matZ;
	const float pi = 3.14159265;
	float thetaX = (pi / 180) * cordinates.x;
	float thetaY = (pi / 180) * cordinates.y;
	float thetaZ = (pi / 180) * cordinates.z;

	if (transformation == "scale")
		mat = {
			cordinates.x, 0, 0, 0,
			0, cordinates.y, 0, 0,
			0, 0, cordinates.z, 0,
			0, 0, 0 ,1.0f
	};

	else if (transformation == "translate") {
		mat = {
			1.0f, 0, 0, cordinates.x,
			0, 1.0f, 0, cordinates.y,
			0, 0, 1.0f, cordinates.z,
			0, 0, 0, 1.0f
		};
		mat = glm::transpose(mat);
	}

	else if (transformation == "rotate") {
		matX = {
			1 ,0 ,0 ,0,
			0, glm::cos(thetaX), -1* glm::sin(thetaX), 0,
			0, glm::sin(thetaX), glm::cos(thetaX), 0,
			0, 0,0, 1
		};
		matY = glm::mat4(
			glm::cos(thetaY), 0, glm::sin(thetaY), 0,
			0, 1, 0, 0,
			-1* glm::sin(thetaY), 0, glm::cos(thetaY), 0,
			0, 0, 0, 1
		);
		matZ = glm::mat4(
			glm::cos(thetaZ), -1* glm::sin(thetaZ), 0, 0,
			glm::sin(thetaZ), glm::cos(thetaZ), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);

		mat = matX * matY * matX;

	}
	else
		mat = {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0 ,1
	};



	return mat;
	//return transpose(mat);
}


 glm::mat4 Utils::GetMatrix(std::string transformation,float a, float b, float c) 
{
	 glm::mat4 mat, matX, matY, matZ;
	const float pi = 3.14159265;
	float thetaX = pi / 180 * a;
	float thetaY = pi / 180 * b;
	float thetaZ = pi / 180 * c;

	if (transformation == "scale") 
		mat = {
			a, 0, 0, 0,
			0, b, 0, 0,
			0, 0, c, 0,
			0, 0, 0 ,1.0f
		};

	else if (transformation == "translate") {
		mat = {
			1.0f, 0, 0, a,
			0, 1.0f, 0, b,
			0, 0, 1.0f, c,
			0, 0, 0, 1.0f
		};
		//mat = glm::transpose(mat);
	}

	else if (transformation == "rotate") {
		matX = {
			1 ,0 ,0 ,0,
			0, cos(thetaX), -sin(thetaX), 0,
			0, sin(thetaX), cos(thetaX), 0,
			0, 0,0, 1
		};
		matY = glm::mat4(
			cos(thetaY), 0, sin(thetaY), 0,
			0, 1, 0, 0,
			-sin(thetaY), 0, cos(thetaY), 0,
			0, 0, 0, 1
		);
		matZ = glm::mat4(
			cos(thetaZ), -sin(thetaZ), 0, 0,
			sin(thetaZ), cos(thetaZ), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);

		mat = matX * matY * matX;

	}
	else 
		mat = {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0 ,1
		};
	


	return glm::transpose(mat);
	//return mat;
}


MeshModel Utils::LoadMeshModel(const std::string& filePath)
{
	std::vector<Face> faces;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::ifstream ifile(filePath.c_str());

	// while not end of file
	while (!ifile.eof())
	{
		// get line
		std::string curLine;
		std::getline(ifile, curLine);

		// read the type of the line
		std::istringstream issLine(curLine);
		std::string lineType;

		issLine >> std::ws >> lineType;

		// based on the type parse data
		if (lineType == "v")
		{
			vertices.push_back(Utils::Vec3fFromStream(issLine));
		}
		else if (lineType == "vn")
		{
			normals.push_back(Utils::Vec3fFromStream(issLine));
		}
		else if (lineType == "vt")
		{
			//
		}
		else if (lineType == "f")
		{
			faces.push_back(Face(issLine));
		}
		else if (lineType == "#" || lineType == "")
		{
			continue;
		}
		else
		{
			std::cout << "Found unknown line Type: " << lineType << std::endl;
		}
	}

	return MeshModel(faces, vertices, normals, Utils::GetFileName(filePath));
}

std::string Utils::GetFileName(const std::string& filePath)
{
	if (filePath.empty()) {
		return {};
	}

	auto len = filePath.length();
	auto index = filePath.find_last_of("/\\");

	if (index == std::string::npos) {
		return filePath;
	}

	if (index + 1 >= len) {

		len--;
		index = filePath.substr(0, len).find_last_of("/\\");

		if (len == 0) {
			return filePath;
		}

		if (index == 0) {
			return filePath.substr(1, len - 1);
		}

		if (index == std::string::npos) {
			return filePath.substr(0, len);
		}

		return filePath.substr(index + 1, len - index - 1);
	}

	return filePath.substr(index + 1, len - index);
}