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
	this->matTransformations = { glm::mat4(1),  glm::mat4(1), glm::mat4(1)};
	this->cordinatesTransformations = { glm::vec3(0),glm::vec3(0), glm::vec3(0) };
	this->vertices = vertices;
	this->faces = faces;
	this->normals = normals;
}

MeshModel::MeshModel(const MeshModel& model)
{
	this->vertices = model.vertices;
	this->faces = model.faces;
	this->normals = model.normals;
	this->worldTransform = glm::mat4(1);
	this->matTransformations = { glm::mat4(1),  glm::mat4(1), glm::mat4(1) };
	this->cordinatesTransformations = { glm::vec3(0),glm::vec3(0), glm::vec3(0) };
	this->modelName = model.modelName;
}

MeshModel::~MeshModel()
{

}

void MeshModel::SetWorldTransformation()
{
	this->worldTransform = matTransformations[2] * matTransformations[0] * matTransformations[1];
}




const glm::vec3 MeshModel::GetCordinates(std::string& name) const
{
	if (name == "translate")
		return cordinatesTransformations[2];
	else if (name == "scale")
		return cordinatesTransformations[0];
	else
		return cordinatesTransformations[1];
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
		this->matTransformations[2] = matrix;
	else if (name == "scale")
		this->matTransformations[0] = matrix;
	else
		this->matTransformations[1] = matrix;
}

void MeshModel::SetCordinates(const glm::vec3& cordinates, std::string& name)
{
	if (name == "scale") 
		this->cordinatesTransformations[0] += cordinates;
	if (name == "translate")
		this->cordinatesTransformations[2] += cordinates;
	else 
		this->cordinatesTransformations[1] += cordinates;
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


const std::vector<Face> MeshModel::GetFaces() const
{
	return faces;
}

const std::vector<glm::vec3> MeshModel::GetNormals() const
{
	return normals;
}
