#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "MeshModel.h"
#include <glm/gtc/type_ptr.hpp>

class Light: public MeshModel{
private:


public:
	float vault_diffuse;
	float vault_ambient;
	float vault_specular;
	
	Light(MeshModel& model);
	~Light();
	void Light::SetMaterialVault(glm::vec3 mat);
};