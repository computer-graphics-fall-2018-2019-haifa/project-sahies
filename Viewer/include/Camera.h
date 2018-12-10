#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "MeshModel.h"

/*
 * Camera class. This class takes care of all the camera transformations and manipulations.
 */

class Camera: public MeshModel
{
private:
	glm::mat4 viewTransformation;
	glm::mat4 projectionTransformation;
	


public:
	bool byTopBttm;
	glm::vec3 eye;
	glm::vec3 at;
	glm::vec3 up;
	glm::vec3 zoom;

	float left;
	float right;
	float bottom;
	float top;
	float zNear;
	float zFar;
	float fovy;
	float aspect;
	float height;

	Camera( glm::vec3& eye,  glm::vec3& at,  glm::vec3& up, MeshModel& model);
	Camera(const Camera& other);
	~Camera();

	void SetCameraLookAt( glm::vec3& eye,  glm::vec3& at,  glm::vec3& up);

	void SetOrthographicProjection(
		const float height,
		const float aspectRatio,
		const float near,
		const float far);

	void SetPerspectiveProjection(
		const float fovy,
		const float aspect,
		const float near,
		const float far);

	void SetZoom(const float zoom);

	void Camera::SetEyePlace();
	void SetCamTransformation();
	void SetWorldTransformation();
	const glm::mat4 GetViewTransformation();
	const glm::mat4 GetProjection() const;
	void Camera::SetFocus(MeshModel& model);
	void Camera::SetPerspectiveProjection();
	void Camera::SetOrthographicProjection();
	void Camera::SetCameraLookAt();
	// Add more methods/functionality as needed...
};
