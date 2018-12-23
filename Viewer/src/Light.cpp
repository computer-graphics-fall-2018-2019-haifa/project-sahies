#define _USE_MATH_DEFINES
#include "Light.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Utils.h"
#include "MeshModel.h"
#include "Renderer.h"
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <cmath>


Light::Light(MeshModel& model):
	MeshModel(model)
{
	this->color = glm::vec3(0,1,1);
}



Light::~Light()
{
}

//void Light::SetMaterialVault(glm::vec3 mat)
//{
//	vault_diffuse = mat[0];
//	vault_ambient = mat[1];
//	vault_specular = mat[2];
//}


glm::vec3 Light::CenterShift(glm::vec3 vec)
{
	int x_center = 1280.0f / 2;
	int y_center = 720.0f / 2;
	glm::vec3 center_shift = glm::vec3(x_center, y_center, 0);
	return vec + center_shift;
}


glm::vec3 Light::setFog(glm::vec3 reflect_color, glm::vec3 fog_color, float z, float z_near, float z_far)
{
	float distance = glm::abs(z);
	float factor = glm::clamp((z_far  - distance) / (z_far - z_near),0.0f,1.0f);
	return (1 - factor) * reflect_color + factor * fog_color;
}



glm::vec3 Light::SetIlum(glm::vec3 point, glm::vec3 normal, glm::vec3 eye, std::string shade_style, std::shared_ptr<MeshModel>& model)
{
	// ambient 

	//diffuse
	glm::vec3 min = MeshModel::findMin(newVertices);
	glm::vec3 max = MeshModel::findMax(newVertices);

	glm::vec3 light_pos = /*glm::vec3(max.x,min.y,min.z)*/ Utils::Vertex4to3(model_transform_without_projection * glm::vec4(translate, 1));
	
	//glm::vec3 model_pos = Utils::Vertex4to3(model->model_transform_without_projection * glm::vec4(model->translate, 1));
	glm::vec3 light_normal = glm::normalize(point - light_pos);
	if (this->source == "parallel")
		light_normal = glm::normalize(-Utils::Vertex4to3(glm::vec4(light_pos,0)));
	if (shade_style == "flat")
		light_normal = glm::normalize(light_pos - point);
	glm::vec3 model_normal = glm::normalize(normal);
	//light_normal = glm::normalize(normal);
	float LN = std::max(glm::dot(light_normal, model_normal), 0.0f);


	// specular
	glm::vec3 v_normal = glm::normalize(eye - point);
	glm::vec3 R = glm::normalize((2.0f * (glm::dot(model_normal, light_normal)) * model_normal) - light_normal);
	//glm::vec3 R = glm::normalize(2.0f * light_normal * model_normal - light_normal);
	//glm::vec3 to_camera = glm::normalize (-1.0f * point);
	//float VR = std::max(glm::dot(R, v_normal),1.0f);
	float VR = glm::dot(R, v_normal);


	//glm::vec3 Ia = vault_ambient * color;
	glm::vec3 Id = model->diffuse * LN  * color;
	glm::vec3 Is = model->specular  /** light_normal */ * glm::pow(VR,model->exponent)* color;


	return light_power * (Id + Is);
}
