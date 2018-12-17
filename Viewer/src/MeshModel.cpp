#include "MeshModel.h"
#include "Utils.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>


std::vector<glm::vec3> colorss = { glm::vec3(0.87,0.58,0.98), glm::vec3(0.8 , 0.498039 , float(0.196078)),
		glm::vec3(0,1,0),glm::vec3(0,0,1), glm::vec3(1,0,1), glm::vec3(1,1,0), glm::vec3(0,1,1) };

MeshModel::MeshModel(const std::vector<Face>& faces, const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, const std::string& modelName) :
	modelName(modelName),
	worldTransform(glm::mat4x4(1))
{
	translate = glm::vec3(0);
	scale = glm::vec3(1);
	rotate = glm::vec3(0);
	this->matTransformations = { glm::mat4(1),  glm::mat4(1), glm::mat4(1)};
	this->cordinatesTransformations = { glm::vec3(1),glm::vec3(0), glm::vec3(0) };
	this-> matWorldTransformations = { glm::mat4(1),  glm::mat4(1), glm::mat4(1) };
	this->vertices = vertices;
	this->faces = faces;
	this->normals = normals;
	SetColor(glm::vec3(1));
	this->newVertices = vertices;
	ambient = 0.2;
	specular = 0.2;
	diffuse = 0.2;

	
}

MeshModel::MeshModel(const MeshModel& model)
{
	this->vertices = model.vertices;
	this->faces = model.faces;
	this->normals = model.normals;
	this->worldTransform = model.worldTransform;
	this->matTransformations = model.matTransformations;
	this->cordinatesTransformations = model.cordinatesTransformations;
	this->modelName = model.modelName;
	this->matWorldTransformations = model.matTransformations;
	SetColor(glm::vec3(1));
	translate = glm::vec3(0);
	scale = glm::vec3(1);
	rotate = glm::vec3(0);
	this->newVertices = vertices;
}



MeshModel::~MeshModel()
{

}

void MeshModel::SetWorldTransformation()
{
	this->worldTransform = matWorldTransformations[2] * matWorldTransformations[0] * matWorldTransformations[1];
}

 void MeshModel::InitCordinate()  {
	this->cordinatesTransformations = { glm::vec3(0),glm::vec3(0), glm::vec3(0) };
}

 void MeshModel::SetNewVertices(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& n_vertices)
 {
	 this->newVertices = vertices;
	 this->newNormalVertices = n_vertices;
 }

 std::vector<glm::vec3> MeshModel::GetNewVertices()
 {
	 return newVertices;
 }

 std::vector<glm::vec3> MeshModel::GetNewNormalVertices()
 {
	 return this->newNormalVertices;
 }

void MeshModel::SetObjectTransformation()
{
	this->objectTransform = matTransformations[2] * matTransformations[0] * matTransformations[1];
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

const glm::mat4x4 & MeshModel::GetObjectTransformation() const
{
	return objectTransform;
}




void MeshModel::SetColor(const glm::vec3& color)
{
	this->color = color;
}


void MeshModel::setVertices(const std::vector<glm::vec3> vertices) {

	this->vertices = vertices;

}

void MeshModel::SetTransform(const std::string & name, glm::vec3 cor)
{
	this->transformName = name;
	if (name == "translate")
		translate = cor;
	else if (name == "scale")
		scale = cor;
	else
		rotate = cor;
}

void MeshModel::SetModelName(const std::string & name)
{
	this->modelName = name;
}

void MeshModel::setMatrix(const glm::mat4 matrix, std::string name, std::string transformation)
{
	if (transformation == "world") {
		if (name == "translate")
			this->matWorldTransformations[2] = matrix;
		else if (name == "scale")
			this->matWorldTransformations[0] = matrix;
		else
			this->matWorldTransformations[1] = matrix;
	}
	else {
		if (name == "translate")
			this->matTransformations[2] = matrix;
		else if (name == "scale")
			this->matTransformations[0] = matrix;
		else
			this->matTransformations[1] = matrix;
	}
}

void MeshModel::SetCordinates(const glm::vec3& cordinates, std::string& name)
{
	if (name == "scale") 
		this->cordinatesTransformations[0] += cordinates;
	else if (name == "translate") 
		this->cordinatesTransformations[2] += cordinates;
	
	else 
		this->cordinatesTransformations[1] += cordinates;
}

 std::vector<glm::vec3> MeshModel::GetVertices() const
{
	return vertices;
}

const glm::vec3& MeshModel::GetColor() const
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


std::vector<Face> MeshModel::GetFaces() const
{
	return faces;
}

 std::vector<glm::vec3> MeshModel::GetNormals() const
{
	return normals;
}

const std::map<std::string, glm::vec3> MeshModel::GetCube() const
{
	return cube;
}


glm::vec2 MeshModel::ScaleSize(const std::vector<glm::vec3>& vertices, float h, float w)
{

	glm::vec3 max = findMax(vertices);
	glm::vec3 min = findMin(vertices);

	float right = max.x;
	float left = min.x;
	float bottom = min.y;
	float top = max.y;
	float front = min.z;
	float back = max.z;

	return glm::vec2((2000*(right - left)/w),(2000*(top - bottom)/h)) ;

}

void MeshModel::CreateCube(std::vector<glm::vec3>& vertices, float h, float w, glm::mat4& modelTransformations)
{
	glm::vec4 center_shift = glm::vec4(h, w, 0,0);

	glm::vec3 max = findMax(vertices);
	glm::vec3 min = findMin(vertices);

	float right = max.x;
	float left = min.x;
	float bottom = min.y;
	float top = max.y;
	float front = min.z;
	float back = max.z;
	

	cube["lbf"] = modelTransformations * glm::vec4(left, bottom, front, 1) + center_shift;
	cube["rbf"] = modelTransformations * glm::vec4(right, bottom, front, 1) + center_shift;

	cube["ltf"] = modelTransformations * glm::vec4(left, top, front, 1) + center_shift;
	cube["rtf"] = modelTransformations * glm::vec4(right, top, front, 1) + center_shift;

	cube["lbb"] = modelTransformations * glm::vec4(left, bottom, back, 1) + center_shift;
	cube["rbb"] = modelTransformations * glm::vec4(right, bottom, back, 1) + center_shift;

	cube["ltb"] = modelTransformations * glm::vec4(left, top, back, 1) + center_shift;
	cube["rtb"] = modelTransformations * glm::vec4(right, top, back, 1) + center_shift;

}

glm::vec3 MeshModel::findMax(const std::vector<glm::vec3>& vertices)
{
	float max_x = vertices[0].x, max_z = vertices[0].z, max_y = vertices[0].y;
	for (auto vertex : vertices)
	{
		if (vertex.x > max_x)
			max_x = vertex.x;
		if (vertex.y > max_y)
			max_y = vertex.y;
		if (vertex.z > max_z)
			max_z = vertex.z;
	}
	return { max_x,max_y,max_z };
}

glm::vec3 MeshModel::findMin(const std::vector<glm::vec3>& vertices)
{
	float min_x = vertices[0].x, min_z = vertices[0].z, min_y = vertices[0].y;
	for (auto vertex : vertices)
	{
		if (vertex.x < min_x)
			min_x = vertex.x;
		if (vertex.y < min_y)
			min_y = vertex.y;
		if (vertex.z < min_z)
			min_z = vertex.z;
	}
	return { min_x,min_y,min_z };
}