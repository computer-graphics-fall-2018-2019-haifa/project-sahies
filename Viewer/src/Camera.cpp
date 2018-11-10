#define _USE_MATH_DEFINES

#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Utils.h"
#include "MeshModel.h"

Camera::Camera(const glm::vec4& eye, const glm::vec4& at, const glm::vec4& up, MeshModel& model) :
	zoom(1.0),
	MeshModel(model)
{

	SetCameraLookAt(eye, at, up);
}

Camera::~Camera()
{
}

void Camera::SetCameraLookAt(const glm::vec4& eye, const glm::vec4& at, const glm::vec4& up)
{
	/*auto z = glm::normalize(eye - at);
	auto x = glm::normalize(glm::cross(up, z));
	auto y = glm::normalize(glm::cross(x, z));*/
/*
	glm::mat4 t = Utils::GetMatrix("translate",eye.x, eye.y, eye.z);
	this->SetTransformation(t);*/

	glm::vec4 n = glm::normalize(glm::vec4(glm::vec3(eye / eye.w) - glm::vec3(at), 0));
	glm::vec4 u = glm::normalize(glm::vec4(glm::cross(glm::vec3(up / up.w), glm::vec3(n)), 0));
	glm::vec4 v = glm::normalize(glm::vec4(glm::cross(glm::vec3(n), glm::vec3(u)), 0));
	glm::vec4 t = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 c = glm::mat4(u, v, n, t);
	glm::mat4 eyeT = glm::mat4(
		{ 1, 0, 0, 0 },
		{ 0, 1, 0, 0 },
		{ 0, 0, 1, 0 },
		{ -eye.x / eye.w, -eye.y / eye.w, -eye.z / eye.w, 1 }
	);
	viewTransformation = glm::inverse(c) * eyeT;
}

void Camera::SetOrthographicProjection(
	const float height,
	const float aspectRatio,
	const float near,
	const float far)
{

}

void Camera::SetPerspectiveProjection(
	const float fovy,
	const float aspectRatio,
	const float near,
	const float far)
{

}

void Camera::SetZoom(const float zoom)
{

}

const glm::mat4x4 Camera::GetViewTransformation() const
{
	return viewTransformation;
}
