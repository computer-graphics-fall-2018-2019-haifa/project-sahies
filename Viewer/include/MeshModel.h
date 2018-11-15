#pragma once
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include "Face.h"
#include "map"

/*
 * MeshModel class.
 * This class represents a mesh model (with faces and normals informations).
 * You can use Utils::LoadMeshModel to create instances of this class from .obj files.
 */
class MeshModel
{
private:
	std::vector<Face> faces;
	std::vector<glm::vec3> normals;
	glm::vec4 color;
	std::string modelName;
	std::string transformName;
	std::vector<glm::vec3> cordinatesTransformations;
	std::map<std::string, glm::vec3> cube;

protected:
	glm::mat4x4 worldTransform; 
	glm::mat4x4 objectTransform;
	std::vector<glm::mat4> matTransformations;
	std::vector<glm::mat4> matWorldTransformations;
	std::vector<glm::vec3> vertices;


public:
	MeshModel(const std::vector<Face>& faces, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::string& modelName = "");
	virtual ~MeshModel();
	MeshModel(const MeshModel& model);

	glm::vec3 findMax(std::vector<glm::vec3>& vertices);
	glm::vec3 findMin(std::vector<glm::vec3>& vertices);

	const glm::vec4& GetColor() const;
	const std::string& GetTransform() const;
	void SetColor(const glm::vec4& color);
	void setVertices(const std::vector<glm::vec3> vertices);
	void SetTransform(const std::string& name);
	void SetCordinates(const glm::vec3& cordinates, std::string& name);
	void SetModelName(const std::string& name);
	void setMatrix(const glm::mat4 matrix, std::string name, std::string transformation);
	virtual void MeshModel::SetWorldTransformation();
	const glm::vec3 MeshModel::GetCordinates(std::string& name) const;
	const std::map<std::string, glm::vec3> MeshModel::GetCube() const;
	void SetObjectTransformation();

	const std::string& GetModelName();
	const std::vector<glm::vec3> MeshModel::GetVertices() const;
	const std::vector<Face> MeshModel::GetFaces() const;
	const std::vector<glm::vec3> GetNormals() const;
	void MeshModel::CreateCube(std::vector<glm::vec3>& vertices,float h, float w);
	const glm::mat4x4& GetWorldTransformation() const;
	const glm::mat4x4& GetObjectTransformation() const;
	 void InitCordinate() ;

	// Add more methods/functionality as needed...
};
