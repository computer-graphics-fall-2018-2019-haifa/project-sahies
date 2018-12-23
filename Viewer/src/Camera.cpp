#define _USE_MATH_DEFINES

#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Utils.h"
#include "MeshModel.h"
#include "Renderer.h"

Camera::Camera( glm::vec3& eye,  glm::vec3& at,  glm::vec3& up, MeshModel& model) :
	zoom(glm::vec3(1)),
	MeshModel(model)
{
	this->eye = eye;
	this->at = at;
	this->up = up;
	this->left = -1;
	this->right = 1;
	this->bottom = -1;
	this->top = 1;
	this->zNear = 1;
	this->zFar = 10;
	this->fovy = 26;
	this->aspect = 1280.0f/720.0f;
	this->height = 1;
	this->byTopBttm = false;
	SetCameraLookAt(eye, at, up);
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
	this->byTopBttm = other.byTopBttm;
	zoom = glm::vec3(1);
	
}

Camera::~Camera()
{
}

void Camera::SetCameraLookAt( glm::vec3& e,  glm::vec3& a,  glm::vec3& u)
{

	/*glm::vec4 at4 = glm::vec4(a, 1) * (Utils::GetMatrix("translate", translate) * Utils::GetMatrix("rotate", rotate));
	glm::vec4 up4 = glm::vec4(u, 1) * Utils::GetMatrix("rotate", rotate);
	glm::vec4 eye4 = glm::vec4(e, 1) * Utils::GetMatrix("translate", translate);

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
	identity[3] -= glm::vec4(e,1) - glm::vec4(0, 0, 0, 1);

	this->viewTransformation = c * identity * Utils::GetMatrix("scale", zoom);*/
	glm::vec3 _eye = /*(Utils::GetMatrix("translate", translate)  * */glm::vec4(e[0], e[1], e[2], 1);
	glm::vec3 _up = /*Utils::GetMatrix("rotate", rotate) * */glm::vec4(u[0], u[1], u[2], 1);
	glm::vec3 _at = /*(Utils::GetMatrix("translate", translate)  **/ /*Utils::GetMatrix("rotate", rotate)) **/ glm::vec4(a[0], a[1], a[2], 1);

	glm::vec3 z = glm::normalize(_eye - _at);
	glm::vec3 x = glm::normalize(glm::cross(_up, z));
	glm::vec3 y = glm::normalize(glm::cross(z, x));

	glm::mat4x4 c = glm::transpose(glm::mat4x4({
		x[0], x[1], x[2], 0,
		y[0], y[1], y[2], 0,
		z[0], z[1], z[2], 0,
		0, 0, 0, 1
		}
	));

	glm::mat4x4 translationMatrix = glm::transpose(glm::mat4x4(
		{ 1	,	0	,	0	,	_eye[0],
			0	,	1	,	0	,	_eye[1],
			0	,	0	,	1	,	_eye[2],
			0	,	0	,	0	,	1 }));
	
	viewTransformation =glm::transpose(c) * translationMatrix * Utils::GetMatrix("scale", zoom);



}

void Camera::SetCameraLookAt() 
{
	SetCameraLookAt(eye,at,up);
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
	this->worldTransform = matWorldTransformations[2] * matWorldTransformations[0] * matWorldTransformations[1];
}


void Camera::SetOrthographicProjection(
	const float h,
	const float aspectRatio,
	const float _near,
	const float _far)
{
	if (byTopBttm) {
		height = top - bottom;
		float width = right - left;
	} 
	else {
		float width = h * aspectRatio;
		 top = h;
		 bottom = -1 * h;
		 left = -1 * width;
		 right = width;
	}
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
	if (byTopBttm) {
		float h = top - bottom;
		float width = right - left;
	} 
	else {
		const float pi = 3.14159265;
		// shear * scale to make 45 angle * divide by z
		float h = glm::abs(_near * glm::tan(0.5*(fovy *  (pi / 180.0))));
		float width = aspectRatio * h;
		 top = h;
		 bottom = -h;
		 left = -width;
		 right = width;
	}
	glm::mat4 mat;
	mat[0] = glm::vec4((2 * _near) / (right - left),0, 0, 0);
	mat[1] = glm::vec4(0, (2 * _near) / (top - bottom),0,0);
	mat[2] = glm::vec4((right + left) / (right - left), (top + bottom) / (top - bottom), -1 * (_far + _near) / (_far - _near),-1);
	mat[3] = glm::vec4(0,0, -2 * (_far * _near) / (_far - _near),0);
	this->projectionTransformation = mat;
	eye.z = 125;
	SetCameraLookAt();
}

void Camera::SetZoom(const float zoom)
{
	
}


void Camera::SetFocus(MeshModel& model)
{
	std::string name = "translate";
	auto new_at = glm::vec3(-model.translate.x, -model.translate.y, at.z);
	auto new_eye = glm::vec3(-model.translate.x, -model.translate.y, eye.z);
	SetCameraLookAt(new_eye, new_at, up);
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