#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>
#include "MeshModel.h"
#include "Camera.h"

/*
 * Scene class.
 * This class holds all the scene information (models, cameras, lights, etc..)
 */
class Scene {
private:
	std::vector<std::shared_ptr<MeshModel>> models;
	std::vector<Camera> cameras;



public:
	Scene();

	int activeCameraIndex;
	int activeModelIndex;

	bool toDrawNormals = false;
	bool toDrawBox = false;
	std::string draw_genre;
	float normal_size;

	void AddModel(const std::shared_ptr<MeshModel>& model);
	void Scene::PopModel();
	const int GetModelCount() const;

	const std::shared_ptr<MeshModel> Scene::GetModel(int index) const;
	const std::vector<std::shared_ptr<MeshModel>> Scene::GetModels() const;
	void AddCamera(const Camera& camera);
	const int GetCameraCount() const;
	const std::vector<Camera> Scene::GetCameras() const;

	void SetActiveCameraIndex(int index);
	const int GetActiveCameraIndex() const;
	const Camera GetCamera(int index) const;
	std::vector<std::string>Scene::GetModelsNames();
	std::vector<std::string>Scene::GetCamerasNames();

	

	void SetDrawNormals(bool draw, std::string genre, float normal_size);
	void SetActiveModelIndex(int index);
	const int GetActiveModelIndex() const;
	const float GetDrawNormals(std::string& type) const;
	void Scene::SetActiveModelIndex(std::string name);
	// Add more methods as needed...
};