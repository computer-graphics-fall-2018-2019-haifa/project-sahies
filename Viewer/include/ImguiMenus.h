#pragma once
#include <imgui/imgui.h>
#include "Scene.h"
#include "Renderer.h"

void DrawImguiMenus(ImGuiIO& io, Scene& scene, Renderer& renderer, int& change);
const glm::vec4& GetClearColor();
void SubmitTransform(std::shared_ptr<MeshModel> model, Renderer& renderer, float x, float y, float z, std::string name, std::string genreTransformation, int& change);
