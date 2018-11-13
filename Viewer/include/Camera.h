#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "MeshModel.h"

/*
 * Camera class. This class takes care of all the camera transformations and manipulations.
 *
 * Implementation suggestion:
 * --------------------------
 * Make the Camera class be a subclass of MeshModel, so you can easily and elegantly render 
 * the cameras you have added to the scene.
 */
class Camera: public MeshModel
{
private:
	glm::mat4 viewTransformation;
	glm::mat4 projectionTransformation;
	float zoom;

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


	const glm::mat4 GetViewTransformation() const;
	// Add more methods/functionality as needed...
};
