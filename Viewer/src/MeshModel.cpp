#include "MeshModel.h"
#include "Utils.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

MeshModel::MeshModel(const std::vector<Face>& faces, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::string& modelName) :
	modelName(modelName),
	worldTransform(glm::mat4x4(1))
{
	this->vertices = vertices;
	this->faces = faces;
	this->normals = normals;
}

MeshModel::~MeshModel()
{

}

void MeshModel::SetWorldTransformation(const glm::mat4x4& worldTransform)
{
	this->worldTransform = worldTransform;
}

const glm::mat4x4& MeshModel::GetWorldTransformation() const
{
	return worldTransform;
}


void MeshModel::SetColor(const glm::vec4& color)
{
	this->color = color;
}


void MeshModel::setVertices(const std::vector<glm::vec3> vertices) {

	this->vertices = vertices;

}

void MeshModel::SetTransform(const std::string & name)
{
	this->transformName = name;
}

void MeshModel::SetCordinates(const glm::vec3 & cordinates)
{
	this->transformCordinates = cordinates - this->transformCordinates;
}

const std::vector<glm::vec3> MeshModel::GetVertices() const
{
	return vertices;
}

const glm::vec4& MeshModel::GetColor() const
{
	return color;
}

const std::string & MeshModel::GetTransform() const
{
	return transformName;
}

const std::string& MeshModel::GetModelName()
{
	return modelName;
}

const glm::vec3& MeshModel::GetCordinates() const
{
	return transformCordinates;
}

const std::vector<Face> MeshModel::GetFaces() const
{
	return faces;
}

const std::vector<glm::vec3> MeshModel::GetNormals() const
{
	return normals;
}
