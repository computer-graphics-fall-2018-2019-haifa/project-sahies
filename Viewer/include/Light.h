#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "MeshModel.h"


class Light : public MeshModel {
private:


public:
	int k;
	Light(MeshModel& model, int k);
	Light(const Light& other);
	~Light();

};