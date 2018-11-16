#include "Scene.h"
#include "MeshModel.h"
#include <string>
#include "Camera.h"

Scene::Scene() :
	activeCameraIndex(0),
	activeModelIndex(0)
{}

void Scene::AddModel(const std::shared_ptr<MeshModel>& model)
{
	models.push_back(model);
}

void Scene::PopModel()
{
	models.pop_back();
}

const int Scene::GetModelCount() const
{
	return models.size();
}

const std::shared_ptr<MeshModel> Scene::GetModel(int index) const
{
	return models[index];
}

const std::vector<std::shared_ptr<MeshModel>> Scene::GetModels() const
{
	return models;
}

const std::vector<Camera> Scene::GetCameras() const
{
	return cameras;
}

void Scene::AddCamera(const Camera& camera)
{
	cameras.push_back(camera);
}

const int Scene::GetCameraCount() const
{
	return cameras.size();
}

void Scene::SetActiveCameraIndex(int index)
{
	// implementation suggestion...
	if (index >= 0 && index < cameras.size())
	{
		activeCameraIndex = index;
	}
}

const int Scene::GetActiveCameraIndex() const
{
	return activeCameraIndex;
}

const Camera Scene::GetCamera(int index) const
{	
	if (cameras.size() > index)
		return cameras[index];
}

std::vector<std::string>Scene::GetModelsNames()
{
	std::vector<std::string> names;
	for (int i = 0; i < GetModelCount(); i++) 
		names.push_back(models[i]->GetModelName());
	return names;
}



void Scene::SetDrawNormals(bool draw, std::string genre, float normal_size)
{
	this->toDrawNormals = draw;
	this->draw_genre = genre;
	this->normal_size = normal_size;
}

void Scene::SetActiveModelIndex(int index)
{
	// implementation suggestion...
	if (index >= 0 && index < models.size())
	{
		activeModelIndex = index;
	}
}

const int Scene::GetActiveModelIndex() const
{
	return activeModelIndex;
}

const float Scene::GetDrawNormals(std::string& type) const
{
	if (this->toDrawNormals) {
		type = this->draw_genre;
		return normal_size;
	}
	return 0;
}
