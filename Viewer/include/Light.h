#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "MeshModel.h"
#include <glm/gtc/type_ptr.hpp>

class Light : public MeshModel {
private:


public:
	float vault_diffuse;
	float vault_ambient;
	float vault_specular;

	float light_power = 0.5;

	Light(std::shared_ptr<MeshModel>& model);
	~Light();
	void Light::SetMaterialVault(glm::vec3 mat);
	glm::vec3 Light::SetIlum(glm::vec3 point,glm::vec3 normal);
	glm::vec3 Light::CenterShift(glm::vec3 vec);
};