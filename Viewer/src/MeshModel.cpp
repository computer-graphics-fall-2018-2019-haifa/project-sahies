#include "MeshModel.h"
#include "Utils.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

MeshModel::MeshModel(const std::vector<Face>& faces, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::string& modelName) :
	transformCordinates({0,0,0}),
	modelName(modelName),
	worldTransform(glm::mat4x4(1))
{
	this->vertices = vertices;
	this->faces = faces;
	this->normals = normals;
}

MeshModel::MeshModel(const MeshModel &model)
{
	this->vertices = model.vertices;
	this->faces = model.faces;
	this->normals = model.normals;
	this->worldTransform = glm::mat4(1);
	this->transformCordinates = { 0,0,0 };
	this->translate = glm::mat4(1);
	this->rotate = glm::mat4(1);
	this->scale = glm::mat4(1);
}

MeshModel::~MeshModel()
{

}

void MeshModel::SetWorldTransformation()
{
	this->worldTransform = translate * rotate * scale;
}

void MeshModel::SetDefatultWorldTransformation()
{
	this->translate = glm::mat4(1);
	this->rotate = glm::mat4(1);
	this->scale = glm::mat4(1);
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

void MeshModel::SetModelName(const std::string & name)
{
	this->modelName = name;
}

void MeshModel::setMatrix(const glm::mat4 matrix, std::string name)
{
	if (name == "translate")
		this->translate = matrix;
	else if (name == "scale")
		this->scale = matrix;
	else
		this->rotate = matrix;
}

void MeshModel::SetCordinates(const glm::vec3 & cordinates)
{
	this->transformCordinates += cordinates;
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
