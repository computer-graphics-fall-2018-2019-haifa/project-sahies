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
	float zoom;
	float left;
	float right;
	float bottom;
	float top;
	float zNear;
	float zFar;
	float fovy;
	float aspect;
	std::string projectionType;
	glm::vec3 eye;
	glm::vec3 at;
    glm::vec3 up;

public:
	Camera(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up, MeshModel& model);
	~Camera();

	void SetCameraLookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up);

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

	void SetCamTransformation(std::string name, glm::vec3 cordinates);
	void SetWorldTransformation();
	const glm::mat4 GetViewTransformation() const;
	const glm::mat4 GetProjection() const;
	void SetEyePlace();
	// Add more methods/functionality as needed...
};
