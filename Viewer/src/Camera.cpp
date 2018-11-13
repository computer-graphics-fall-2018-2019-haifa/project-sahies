#define _USE_MATH_DEFINES

#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Utils.h"
#include "MeshModel.h"

Camera::Camera(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up, MeshModel& model) :
	zoom(1.0),
	MeshModel(model)
{

	SetCameraLookAt(eye, at, up);
}

Camera::~Camera()
{
}

void Camera::SetCameraLookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up)
{
	auto z = glm::normalize(eye - at);
	auto x = glm::normalize(glm::cross(up, z));
	auto y = glm::normalize(glm::cross(x, z));

	//glm::mat4 t = Utils::GetMatrix("translate",eye.x, eye.y, eye.z);
	//this->SetTransformation(t);

	glm::mat4 lookat_mat = {
		x.x, x.y, x.z, 0,
		y.x, y.y, z.z, 0,
		z.x, z.y, z.z, 0,
		eye.x, eye.y, eye.z, 1
	};
	//viewTransformation = glm::inverse(c) * eyeT;
	viewTransformation = lookat_mat;
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
