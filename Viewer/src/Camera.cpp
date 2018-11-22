#define _USE_MATH_DEFINES

#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Utils.h"
#include "MeshModel.h"
#include "Renderer.h"

Camera::Camera(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up, MeshModel& model) :
	zoom(1.0),
	MeshModel(model)
{
	this->eye = eye;
	this->at = at;
	this->up = up;
	this->projectionType = "persp";
	this->left = -1;
	this->right = 1;
	this->bottom = -1;
	this->top = 1;
	this->zNear = 10;
	this->zFar = 1000;
	this->fovy = 45;
	this->aspect = 1;
	this->height = 2.5f;
	SetCameraLookAt(eye, at, up);
	this->SetPerspectiveProjection();
}

Camera::Camera(const Camera & other):MeshModel(other)
{
	this->eye = other.eye;
	this->at = other.at;
	this->up = other.up;
	SetCameraLookAt(eye, at, up);
	this->projectionType = other.projectionType;
	this->left = other.left;
	this->right = other.right;
	this->bottom = other.bottom;
	this->top = other.top;
	this->zNear = other.zNear;
	this->zFar = other.zFar;
	this->fovy = other.fovy;
	this->aspect = other.aspect;
	this->height = other.height;
	SetOrthographicProjection(2.5, 1, 10, 150);
	
}

Camera::~Camera()
{
}

void Camera::SetCameraLookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up)
{

	auto z = glm::normalize(eye - at);
	auto x = glm::normalize(glm::cross(up, z));
	auto y = glm::normalize(glm::cross(z, x));
	

	glm::vec4 z4(z[0], z[1], z[2], 0);
	glm::vec4 x4(x[0], x[1], x[2], 0);
	glm::vec4 y4(y[0], y[1], y[2], 0);
	glm::vec4 t = glm::vec4(0.0, 0.0, 0.0, 1.0);

	glm::mat4 c = glm::mat4(x4, y4, z4, t);


	/*glm::mat4 translate_eye = Utils::GetMatrix("translate", -1*(eye.x), -1*(eye.y), -1*( eye.z));
	viewTransformation = c * glm::transpose(translate_eye);
	viewTransformation = translate_eye * glm::inverse(c);
	*/

	glm::mat4 m(1);
	m[3] -= glm::vec4(eye,1) -glm::vec4(0, 0, 0, 1);
	this->viewTransformation = glm::transpose(c)*m;
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
	float top = 0.5 * height;
	float bottom = -0.5 * height;
	float left = -0.5 * width;
	float right = 0.5 * width;
	// T * S - move center and scale sides length
	glm::mat4 mat = {
			2.0 / (right - left), 0, 0, 	-(right + left) / (right - left),
			0, 2.0 / (top - bottom), 0, -(top + bottom) / (top - bottom),
			0, 0, 2.0 / (_near - _far), -(_far + _near) / (_far - _near),
			0, 0,0, 1
	};
	this->projectionTransformation = mat;
}

void Camera::SetPerspectiveProjection(
	const float fovy,
	const float aspectRatio,
	const float _near,
	const float _far)
{

	const float pi = 3.14159265;
	// shear * scale to make 45 angle * divide by z
	float height = (_far - _near) * glm::tan((fovy * pi)/180);
	float width = aspectRatio * height;
	float t = 0.5 * height;
	float b = -0.5 * height;
	float l = -0.5 * width;
	float r = 0.5 * width;

	glm::mat4 mat = {
		(2 * _near) / (r - l), 0, (r + l) / (r - l), 0,
		0, (2 * _near) / (t - b), (t + b) / (t - b), 0,
		0, 0, -1*(_far + _near) / (_far - _near), -2*(_far * _near) / (_far - _near),
		0, 0, -1, 0
	};
	this->projectionTransformation = glm::transpose(mat);
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
	glm::mat4 mat = Utils::GetMatrix("translate", -1 * (eye.x), -1 * (eye.y), -1 * (eye.z)) *  Utils::GetMatrix("rotate", up) *  Utils::GetMatrix("at", up);
	this->vertices = Renderer::VerticesXmat(vertices,mat);
}