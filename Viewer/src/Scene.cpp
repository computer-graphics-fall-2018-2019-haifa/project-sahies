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