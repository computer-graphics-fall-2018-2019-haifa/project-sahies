#pragma once
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include "Face.h"

/*
 * MeshModel class.
 * This class represents a mesh model (with faces and normals informations).
 * You can use Utils::LoadMeshModel to create instances of this class from .obj files.
 */
class MeshModel
{
private:
	std::vector<Face> faces;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	glm::mat4x4 worldTransform;
	glm::vec4 color;
	std::string modelName;
	glm::vec3 transformCordinates;
	std::string transformName;

public:
	MeshModel(const std::vector<Face>& faces, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::string& modelName = "");
	virtual ~MeshModel();

	void SetWorldTransformation(const glm::mat4x4& worldTransform);
	const glm::mat4x4& GetWorldTransformation() const;

	const glm::vec4& GetColor() const;
	const std::string& GetTransform() const;
	const glm::vec3& GetCordinates() const;
	void SetColor(const glm::vec4& color);
	void setVertices(const std::vector<glm::vec3> vertices);
	void SetTransform(const std::string& name);
	void SetCordinates(const glm::vec3& cordinates);

	const std::string& GetModelName();
	const std::vector<glm::vec3> MeshModel::GetVertices() const;
	const std::vector<Face> MeshModel::GetFaces() const;
	const std::vector<glm::vec3> GetNormals() const;

	// Add more methods/functionality as needed...
};
