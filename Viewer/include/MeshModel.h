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

	std::vector<glm::vec3> normals;
	std::string modelName;
	std::string transformName;
	
	std::map<std::string, glm::vec3> cube;
	
	std::vector<glm::vec3> newNormalVertices;

protected:
	glm::mat4x4 worldTransform; 
	glm::mat4x4 objectTransform;
	std::vector<glm::mat4> matWorldTransformations;
	std::vector<glm::vec3> vertices;
	std::vector<Face> faces;


public:
	MeshModel(const std::vector<Face>& faces, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::string& modelName = "");
	virtual ~MeshModel();
	MeshModel(const MeshModel& model);
	glm::vec3 translate;
	glm::vec3 rotate;
	glm::vec3 scale;
	std::vector<glm::vec3> cordinatesTransformations;
	std::vector<glm::vec3> newVerticesRender;
	std::vector<glm::vec3> newVertices;
	float scale_model = 1.0f;
	glm::vec3 color = glm::vec3(0);

	std::vector<glm::mat4> matTransformations;
	std::string type;
	float ambient = 0.0f;
	float specular = 0.0f;
	float diffuse = 0.2f;
	int exponent = 1.0f;
	glm::mat4 model_transform_without_projection = glm::mat4(1);
	glm::mat4 model_transform = glm::mat4(1);

	static glm::vec3 findMax(const std::vector<glm::vec3>& vertices);
	static glm::vec3 findMin(const std::vector<glm::vec3>& vertices);

	glm::vec2 MeshModel::ScaleSize(const std::vector<glm::vec3>& vertices, float h, float w);
	const glm::vec3& GetColor() const;
	const std::string& GetTransform() const;
	void SetColor(const glm::vec3& color);
	void setVertices(const std::vector<glm::vec3> vertices);
	void SetTransform(const std::string& name, glm::vec3 cor);
	void SetCordinates(const glm::vec3& cordinates, std::string& name);
	void SetModelName(const std::string& name);
	void setMatrix(const glm::mat4 matrix, std::string name, std::string transformation);
	virtual void MeshModel::SetWorldTransformation();
	const glm::vec3 MeshModel::GetCordinates(std::string& name) const;
	const std::map<std::string, glm::vec3> MeshModel::GetCube() const;
	void SetObjectTransformation();

	const std::string& GetModelName();
	 std::vector<glm::vec3> MeshModel::GetVertices() const;
	std::vector<Face> MeshModel::GetFaces() const;
	 std::vector<glm::vec3> GetNormals() const;
	void MeshModel::CreateCube(std::vector<glm::vec3>& vertices,float h, float w, glm::mat4& modelTransformations);
	const glm::mat4x4& GetWorldTransformation() const;
	const glm::mat4x4& GetObjectTransformation() const;
	 void InitCordinate() ;
	 void MeshModel::SetNewVertices(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& n_vertices);
	 std::vector<glm::vec3> GetNewVertices();
	 std::vector<glm::vec3> GetNewNormalVertices();
	 void UpdateChangesModel(glm::mat4 projection,glm::mat4 view_transform, glm::mat4 worldTransformation);
	
	// Add more methods/functionality as needed...
};
