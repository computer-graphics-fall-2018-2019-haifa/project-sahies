#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>
#include "MeshModel.h"
#include "Camera.h"
#include "Light.h"

/*
 * Scene class.
 * This class holds all the scene information (models, cameras, lights, etc..)
 */
class Scene {
private:




public:
	std::vector<std::shared_ptr<MeshModel>> models;
	std::vector<std::shared_ptr<Light>> lights;
	std::vector<std::shared_ptr<Camera>> cameras;
	int activeCameraIndex;
	int activeModelIndex;
	bool toDrawNormals = false;
	bool toDrawBox = false;
	std::string draw_genre;
	float normal_size;
	int activeLightIndex = 0;
	std::string draw_style = "flat";
	bool fog;
	glm::vec3 fog_color;
	bool isPerspective = false;
	//c'tor
	Scene();

	
	void AddCamera(const std::shared_ptr<Camera> camera);
	void AddModel(const std::shared_ptr<MeshModel>& model);
	void AddLight(const std::shared_ptr<Light>& light);
	void Scene::PopModel();
	
	
	// Getters
	const int GetModelCount() const;
	const std::shared_ptr<MeshModel> Scene::GetModel(int index) const;
	const std::shared_ptr<Light> Scene::GetLight(int index) const;
	//const std::shared_ptr<MeshModel> Scene::GetModel(int index) const;
	const std::vector<std::shared_ptr<MeshModel>> Scene::GetModels() const;
	const std::vector<std::shared_ptr<Camera>> Scene::GetCameras() const;
	const std::vector<std::shared_ptr<Light>> Scene::GetLights() const;
	const int GetCameraCount() const;
	//const std::vector<Camera> Scene::GetCameras() const;
	const int GetActiveCameraIndex() const;
	const std::shared_ptr<Camera> GetCamera(int index) const;
	std::vector<std::string>Scene::GetModelsNames();
	std::vector<std::string>Scene::GetCamerasNames();
	std::vector<std::string>Scene::GetLightsNames();
	const int GetActiveModelIndex() const;
	const float GetDrawNormals(std::string& type) const;


	//Setters
	void SetActiveCameraIndex(int index);
	void SetDrawNormals(bool draw, std::string genre, float normal_size);
	void SetActiveModelIndex(int index);
	void Scene::SetActiveModelIndex(std::string name);
	void Scene::SetActiveLightIndex(int index);

};