#define _USE_MATH_DEFINES
#include "Light.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Utils.h"
#include "MeshModel.h"
#include "Renderer.h"
#include <glm/gtc/type_ptr.hpp>

Light::Light(MeshModel& model):
	MeshModel(model)
{
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