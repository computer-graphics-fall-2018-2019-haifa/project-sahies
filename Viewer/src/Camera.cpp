#define _USE_MATH_DEFINES

#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Utils.h"
#include "MeshModel.h"
#include "Renderer.h"

Camera::Camera( glm::vec3& eye,  glm::vec3& at,  glm::vec3& up, MeshModel& model) :
	zoom(1.0),
	MeshModel(model)
{
	this->eye = eye;
	this->at = at;
	this->up = up;
	SetCameraLookAt(eye, at, up);
	this->left = -1;
	this->right = 1;
	this->bottom = -1;
	this->top = 1;
	this->zNear = 1;
	this->zFar = 10;
	this->fovy = 45;
	this->aspect = 1280.0f/720.0f;
	this->height = 1;
	this->SetOrthographicProjection();
}

Camera::Camera(const Camera & other):MeshModel(other)
{
	this->eye = other.eye;
	this->at = other.at;
	this->up = other.up;
	SetCameraLookAt(eye, at, up);
	this->left = other.left;
	this->right = other.right;
	this->bottom = other.bottom;
	this->top = other.top;
	this->zNear = other.zNear;
	this->zFar = other.zFar;
	this->fovy = other.fovy;
	this->aspect = other.aspect;
	this->height = other.height;

	
}

Camera::~Camera()
{
}

void Camera::SetCameraLookAt( glm::vec3& eye,  glm::vec3& at,  glm::vec3& up)
{

	glm::vec4 eye4 = glm::vec4(eye, 1) * Utils::GetMatrix("translate", eye);
	glm::vec4 at4 = glm::vec4(at, 1) *  (Utils::GetMatrix("translate", at) * Utils::GetMatrix("rotate", at));
	glm::vec4 up4 = glm::vec4(up, 1) * Utils::GetMatrix("rotate", up);

	auto z = glm::normalize(eye4 - at4);
	glm::vec3 z3 = Utils::Vertex4to3(z);
	auto x3 = glm::normalize(glm::cross(Utils::Vertex4to3(up4), z3));
	auto y3 = glm::normalize(glm::cross(z3, x3));

	glm::vec4 x4 = glm::vec4(x3, 0.0f);
	glm::vec4 y4 = glm::vec4(y3, 0.0f);
	glm::vec4 z4 = glm::vec4(z3, 0.0f);
	glm::vec4 t = glm::vec4(0.0, 0.0, 0.0, 1.0);
	glm::mat4 c(x4,y4,z4,t);
	c = glm::transpose(c);
	glm::mat4 identity(1);
	identity[3] -= glm::vec4(eye,1) - glm::vec4(0, 0, 0, 1);

	this->viewTransformation = c * identity;
}



void Camera::SetCamTransformation()
{
	std::string rt = "rotate";
	std::string tr = "translate";

	//this->eye = Utils::GetMatrix("translate", this->GetCordinates(tr)) * glm::vec4(eye[0], eye[1], eye[2], 1);
	//this->up = Utils::GetMatrix("rotate", this->GetCordinates(rt)) * glm::vec4(up[0], up[1], up[2], 1);
	//this->at = Utils::GetMatrix("translate", this->GetCordinates(tr)) * glm::vec4(up[0], up[1], up[2], 1);
	
	SetCameraLookAt(eye, at, up);
}


void Camera::SetWorldTransformation()
{
	this->worldTransform = matTransformations[2] * matTransformations[0] * matTransformations[1];
}


void Camera::SetOrthographicProjection(
	const float height,
	const float aspectRatio,
	const float _near,
	const float _far)
{
	float width = height * aspectRatio;
	float top =  height;
	float bottom = -1 * height;
	float left = -1 * width;
	float right =  width;
	// T * S - move center and scale sides length
	glm::mat4 mat = {
			2.0 / (right - left), 0, 0, 	-(right + left) / (right - left),
			0, 2.0 / (top - bottom), 0, -(top + bottom) / (top - bottom),
			0, 0, 2.0 / (_near - _far), -(_far + _near) / (_far - _near),
			0, 0,0, 1
	};
	this->projectionTransformation = glm::transpose(mat);
}

void Camera::SetPerspectiveProjection(
	const float fovy,
	const float aspectRatio,
	const float _near,
	const float _far)
{

	const float pi = 3.14159265;
	// shear * scale to make 45 angle * divide by z
	float height =  _near * tan(0.5*((fovy * pi)/ 180.0));
	float width = aspectRatio * height;
	float t =  height;
	float b =  -height;
	float l = -width;
	float r =  width;
	glm::mat4 mat;
	//glm::mat4 mat = {
	//	(2 * _near) / (r - l), 0, (r + l) / (r - l), 0,
	//	0, (2 * _near) / (t - b), (t + b) / (t - b), 0,
	//	0, 0, -1*(_far + _near) / (_far - _near), -2*(_far * _near) / (_far - _near),
	//	0, 0, -1, 0
	//};

	mat[0] = glm::vec4((2 * _near) / (r - l),0, 0, 0);
	mat[1] = glm::vec4(0, (2 * _near) / (t - b),0,0);
	mat[2] = glm::vec4((r + l) / (r - l), (t + b) / (t - b), -1 * (_far + _near) / (_far - _near),-1);
	mat[3] = glm::vec4(0,0, -2 * (_far * _near) / (_far - _near),0);
	this->projectionTransformation = mat;
}

void Camera::SetZoom(const float zoom)
{
	
}


void Camera::SetFocus(const float focus, MeshModel& model)
{
	std::string name = "transalte";
	SetCameraLookAt(eye, focus * model.GetCordinates(name), up);
}

const glm::mat4x4 Camera::GetViewTransformation() 
{
	//SetCameraLookAt(eye, at, up);
	return viewTransformation;
}

void Camera::SetOrthographicProjection() {
	SetOrthographicProjection(height, aspect, zNear, zFar);
}

void Camera::SetPerspectiveProjection()
{
	SetPerspectiveProjection(fovy,aspect,zNear,zFar);
}

const glm::mat4 Camera::GetProjection() const
{
	return projectionTransformation;
}


void Camera::SetEyePlace() {
	//glm::mat4 mat = Utils::GetMatrix("translate", -1 * (eye.x), -1 * (eye.y), -1 * (eye.z)) *  Utils::GetMatrix("rotate", up) *  Utils::GetMatrix("rotate", at);
	//glm::mat4 mat = Utils::GetMatrix("translate", at) * Utils::GetMatrix("rotate", at) *  Utils::GetMatrix("rotate", up);
	
	//this->vertices = Renderer::VerticesXmat(vertices,mat);
	//SetCameraLookAt(eye, at, up);
}