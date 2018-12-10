#define _USE_MATH_DEFINES
#include "Light.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Utils.h"
#include "MeshModel.h"
#include "Renderer.h"


Light::Light(MeshModel& model, int k) :
	MeshModel(model)
{
	this->k = k;

}

Light::Light(const Light& other) :MeshModel(other)
{
	this->k = other.k;

}

Light::~Light()
{
}
