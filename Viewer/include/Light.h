#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "MeshModel.h"
#include <glm/gtc/type_ptr.hpp>

class Light : public MeshModel {
private:


public:
	//float vault_diffuse = 0.2;
	//float vault_ambient = 0.0;
	//float vault_specular = 0.0;
	int exponent = 1;
	float light_power = 0.5;

	Light(MeshModel& model);
	~Light();
	void Light::SetMaterialVault(glm::vec3 mat);
	glm::vec3 Light::SetIlum(glm::vec3 point,glm::vec3 normal, glm::vec3 eye, std::string shade_style, std::shared_ptr<MeshModel>& model);
	glm::vec3 Light::CenterShift(glm::vec3 vec);
	glm::mat4 transformations;
	glm::vec3 light_point;
	std::string source;
	static glm::vec3 Light::setFog(glm::vec3 reflect_color,glm::vec3 fog_color, float z, float z_near, float z_far);
};