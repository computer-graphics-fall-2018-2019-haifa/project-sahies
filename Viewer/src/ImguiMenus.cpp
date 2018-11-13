#pragma once
#define _USE_MATH_DEFINES

#include "ImguiMenus.h"
#include "MeshModel.h"
#include "Renderer.h"
#include "Utils.h"
#include <cmath>
#include <memory>
#include <stdio.h>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <nfd.h>
#include <random>

bool showDemoWindow = false;
bool ScaleWindow = false;

glm::vec4 clearColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.00f);

const glm::vec4& GetClearColor()
{
	return clearColor;
}

void DrawImguiMenus(ImGuiIO& io, Scene& scene, Renderer& renderer)
{
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (showDemoWindow)
	{
		ImGui::ShowDemoWindow(&showDemoWindow);
	}

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window

		static int counter = 0;
		static float scale_x = 1.0f, scale_y = 1.0f, scale_z = 1.0f;
		

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &showDemoWindow);      // Edit bools storing our window open/close state

		if (scene.GetModelCount()) {
			std::vector<std::shared_ptr<MeshModel>> models = scene.GetModels();
			int idx = scene.GetActiveModelIndex();
			std::shared_ptr<MeshModel> model = models[idx];

			{
				
				int changed = 0;
				changed += ImGui::SliderFloat("scale_x", &scale_x, -10.0f, 10.0f);
				changed += ImGui::SliderFloat("scale_y", &scale_y, -10.0f, 10.0f);
				changed += ImGui::SliderFloat("scale_z", &scale_z, -10.0f, 10.0f);

				if (changed) {         // Edit 1 float using a slider from 0.0f to 1.0f
					model->SetTransform("scale");
					model->SetCordinates({ scale_x, scale_y, scale_z });
					scale_x = 1.0f, scale_y = 1.0f, scale_z = 1.0f;
					renderer.Transform(*model);
				}
			}
			{
				int changed = 0;
				static float tr_x = 0.0f , tr_y = 0.0f, tr_z = 0.0f;
				changed += ImGui::SliderFloat("tr_x", &tr_x, -200.0f, 200.0f);
				changed += ImGui::SliderFloat("tr_y", &tr_y, -200.0f, 200.0f);
				changed += ImGui::SliderFloat("tr_z", &tr_z, -200.0f, 200.0f);

				if (changed) {         // Edit 1 float using a slider from 0.0f to 1.0f
					model->SetTransform("translate");
					model->SetCordinates({ tr_x, tr_y, tr_z });
					tr_x = 0.0f, tr_y = 0.0f, tr_z = 0.0f;
					renderer.Transform(*model);
				}
			}
			{
				int changed = 0;
				static float angle = 0.0f, y = 0.0f, z = 0.0f;
				changed += ImGui::SliderFloat("angle", &angle, -200.0f, 200.0f);

				if (changed) {         // Edit 1 float using a slider from 0.0f to 1.0f
					model->SetTransform("rotate");
					model->SetCordinates({ angle, y, z });
					angle = 0.0f, y = 0.0f, z = 0.0f;
					renderer.Transform(*model);
				}
			}


			//if () {
			//	scene.SetActiveModelIndex(index_by_name);   // TODO change model

			//}

		}



		

		ImGui::ColorEdit3("clear color", (float*)&clearColor); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	

	// 3. Show another simple window.
	if (ScaleWindow)
	{

		ImGui::Begin("Another Window", &ScaleWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Scale Transform");

		if (ImGui::Button("Close Me"))
		{
			ScaleWindow = false;
		}
		ImGui::End();
	}

	// 4. Demonstrate creating a fullscreen menu bar and populating it.
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoFocusOnAppearing;
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Load Model...", "CTRL+O"))
				{
					nfdchar_t *outPath = NULL;
					nfdresult_t result = NFD_OpenDialog("obj;png,jpg", NULL, &outPath);
					if (result == NFD_OKAY) {
						scene.AddModel(std::make_shared<MeshModel>(Utils::LoadMeshModel(outPath)));
						scene.SetActiveModelIndex(scene.GetModelCount() - 1);
						free(outPath);
					}
					else if (result == NFD_CANCEL) {
					}
					else {
						//TODO
					}

				}

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}
}