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

void Light::SetMaterialVault(glm::vec3 mat)
{
	vault_diffuse = mat[0];
	vault_ambient = mat[1];
	vault_specular = mat[2];
}


glm::vec3 Light::CenterShift(glm::vec3 vec)
{
	int x_center = 1280.0f / 2;
	int y_center = 720.0f / 2;
	glm::vec3 center_shift = glm::vec3(x_center, y_center, 0);
	return vec + center_shift;
}

glm::vec3 Light::SetIlum(glm::vec3 point, glm::vec3 normal)
{
	const auto PI = std::acos(-1);

	glm::vec3 Ia = light_power * color; // TODO:  its constant for every light (not need to add twice for two lights)
	//glm::vec3 light_pos = Light::CenterShift(translate); // model * object_T * world_T?
	//glm::vec4 t = glm::vec4(vertices[0], 1) *(Utils::GetMatrix("translate", translate) *  Utils::GetMatrix("rotate", rotate) *  Utils::GetMatrix("scale", scale));
	glm::vec3 light_pos = translate + Light::CenterShift(translate);
	glm::vec3 light_normal = glm::normalize(normal*(light_pos - point)) ;
	glm::vec3 model_normal = glm::normalize(normal);
	float LN = glm::dot(light_normal, model_normal);


	float angle = std::acos(LN) * PI / 180;
	//glm::vec3 Id(0);
	if (angle < 0 || angle > 90)
		return glm::vec3(0);
	glm::vec3 Id = glm::vec3(light_power) * vault_diffuse * LN  * color; // cos >= 0 to see stuff


	glm::vec3 light_h = light_normal - LN * normal;
	glm::vec3 light_w = LN * normal;
	glm::vec3 R = glm::normalize(light_w - light_h);
	glm::vec3 new_pos = Utils::Vertex4to3(glm::vec4(R, 1) * Utils::GetMatrix("translate", 4, 1, 1));
	glm::vec3 v_normal = glm::normalize(normal*(new_pos - point));
	float VR = glm::dot(R, v_normal);
	float angleVR = std::acos(VR * PI / 180);
	if (angleVR < 0)
		VR = 1;
	//VR = VR > 0.0f ? VR : 0.0f;

	float a = 200; // TODO
	glm::vec3 Is = /*vault_specular*/ 0.1f *  glm::vec3(light_power)  * light_normal * glm::pow(VR,/*a*/ exponent)/** color*/;


	return /*Ia + */Id /*+ Is*/;
}

