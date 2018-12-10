#define _USE_MATH_DEFINES

#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Utils.h"

Camera::Camera(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up, MeshModel& model) :
	zoom(1.0),
	MeshModel(model)
{

	SetCameraLookAt(eye, at, up);

	this->projectionType = "Ortho";
	this->left = -1;
	this->right = 1;
	this->bottom = -1;
	this->top = 1;
	this->zNear = -100;
	this->zFar = 100;
	this->fovy = 1;
	this->aspect = 1;
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


	glm::mat4 translate_eye = Utils::GetMatrix("translate", -1*(eye.x), -1*(eye.y), -1*( eye.z));
	viewTransformation = c * glm::transpose(translate_eye);

	//glm::mat4 c = {
	//	x.x, x.y, x.z, 0,
	//	y.x, y.y, y.z, 0,
	//	z.x, z.y, z.z, 0,
	//	eye.x, eye.y, eye.z, 1
	//};
	//viewTransformation = glm::inverse(c);



}

void Camera::SetCamTransformation()
{
	std::string rt = "rotate";
	std::string tr = "translate";

	this->eye = Utils::GetMatrix("translate", this->GetCordinates(tr)) * glm::vec4(eye[0], eye[1], eye[2], 1);
	this->up = Utils::GetMatrix("rotate", this->GetCordinates(rt)) * glm::vec4(up[0], up[1], up[2], 1);
	this->at = Utils::GetMatrix("translate", this->GetCordinates(tr)) * glm::vec4(up[0], up[1], up[2], 1);
	
	SetCameraLookAt(eye, at, up);
}


void Camera::SetWorldTransformation()
{
	this->worldTransform = glm::inverse(matTransformations[2] * matTransformations[0] * matTransformations[1]);
}


void Camera::SetOrthographicProjection(
	const float height,
	const float aspectRatio,
	const float near,
	const float far)
{
	    glm::mat4 mat = {
			 2.0 / (right - left), 0, 0, 0,
			 0, 2.0 / (top - bottom), 0, 0,
			 0, 0, 2.0 / (zNear - zFar), 0,
			 -(right + left) / (right - left), -(top + bottom) / (top - bottom), -(zFar + zNear) / (zFar - zNear), 1 
		};
		this->projectionTransformation = mat;
}

void Camera::SetPerspectiveProjection(
	const float fovy,
	const float aspectRatio,
	const float near,
	const float far)
{
	float top = zNear * (sin(fovy * M_PI / 180.0) / cos(fovy * M_PI / 180.0));
	float right = top * aspect;
	glm::mat4 mat = {
		 zNear / right, 0, 0, 0 ,
		 0, zNear / top, 0, 0 ,
		 0,0, (-zFar + zNear) / (zFar - zNear), -1 ,
		 0, 0, -2 * zFar * zNear / (zFar - zNear), 0 
	};
	this->projectionTransformation = mat;
}

void Camera::SetZoom(const float zoom)
{

}

const glm::mat4x4 Camera::GetViewTransformation() 
{
	//SetCameraLookAt(eye, at, up);
	return viewTransformation;
}


const glm::mat4 Camera::GetProjection() const
{
	return projectionTransformation;
}


void Camera::SetEyePlace() 
{
	this->vertices = Utils::Vec4to3(Utils::Vec3to4Xmat(this->GetVertices(), Utils::GetMatrix("translate", glm::vec3(2,0,0))));
}