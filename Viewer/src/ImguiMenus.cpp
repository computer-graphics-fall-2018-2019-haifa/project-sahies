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
bool toDrawNormals = 0;
int mul = 1;


glm::vec4 clearColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.00f);

const glm::vec4& GetClearColor()
{
	return clearColor;
}

void DrawImguiMenus(ImGuiIO& io, Scene& scene, Renderer& renderer, int& change)
{
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (showDemoWindow)
	{
		ImGui::ShowDemoWindow(&showDemoWindow);
	}

	

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window

		std::string camera_path = "C:\\Users\\Berger\\Documents\\GitHub\\project-sahies\\Data\\camera.obj";
		static int counter = 0;
		static float scale_x = 0.0f, scale_y = 0.0f, scale_z = 0.0f, tr_x = 0.0f, tr_y = 0.0f, tr_z = 0.0f, x = 0.0f, y = 0.0f, z = 0.0f, CAM_x = 0.0f, CAM_y = 0.0f, CAM_z = 0.0f, scale_all = 1.0f, tr_all = 1.0f, rotate_all = 0.0,
		zoom, left, right, bottom, top, zNear, zFar, fovy, aspect, normal_size;
		std::string projectionType, draw_genre;
		int e, c, camera_active_idx, model_active_idx;

		ImGui::Begin("Or Berger AND Mor Vaknin PlayGround");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some notuseful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &showDemoWindow);      // Edit bools storing our window open/close state


		//*************add option for camera another eye at up*****************
/*
		ImGui::Combo("Select Model", &model_active_idx, models, scene.GetModelCount());
		ImGui::Combo("Select Cameras", &camera_active_idx, cameras, scene.GetCameraCount());
		SetActiveCameraIndex(model_active_idx);
		SetActiveModelIndex(camera_active_idx);*/

		if (ImGui::Button("add camera") || scene.GetCameraCount() == 0) {
			std::shared_ptr<MeshModel> cam_obj = std::make_shared<MeshModel>(Utils::LoadMeshModel(camera_path));
			cam_obj->SetModelName("camera" + std::to_string(counter));
			counter++;
			scene.AddModel(cam_obj);
			scene.AddCamera(Camera(glm::vec3(2, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), *cam_obj));
			scene.SetActiveCameraIndex(scene.GetCameraCount() - 1);
			change = 1;
		}
			
			Camera camera = scene.GetCamera(scene.GetActiveCameraIndex());
			std::vector <std::shared_ptr<MeshModel>> models = scene.GetModels();
			std::vector<Camera> cameras = scene.GetCameras();
			std::shared_ptr<MeshModel> model = scene.GetModel(scene.GetActiveModelIndex());
	
			if (ImGui::SliderFloat("scale_all", &scale_all, 0.0f, 250.0f))
				SubmitTransform(model, renderer, scale_all, scale_all, scale_all, "scale", "object", change);

				

			if (ImGui::SliderFloat("scale_x", &scale_x, 0.0f, 250.0f) ||
				ImGui::SliderFloat("scale_y", &scale_y, 0.0f, 250.0f) ||
				ImGui::SliderFloat("scale_z", &scale_z, 0.0f, 250.0f))  
					SubmitTransform(model, renderer, scale_x, scale_y, scale_z, "scale", "object", change);


			if (ImGui::SliderFloat("tr_all", &tr_all, -20.0f, 20.0f))
				SubmitTransform(model, renderer, tr_all, tr_all, tr_all, "translate", "object", change);

			if (ImGui::SliderFloat("tr_x", &tr_x, -20.0f, 20.0f) ||
				ImGui::SliderFloat("tr_y", &tr_y, -20.0f, 20.0f) ||
				ImGui::SliderFloat("tr_z", &tr_z, -20.0f, 20.0f))
					SubmitTransform(model, renderer, tr_x, tr_y, tr_z, "translate", "object", change);


			if (ImGui::SliderFloat("x", &x, -20.0f, 20.0f) ||
				ImGui::SliderFloat("y", &y, -20.0f, 20.0f) ||
				ImGui::SliderFloat("z", &z, -20.0f, 20.0f))
					SubmitTransform(model, renderer, x, y, z, "rotate",  "object", change);
	


			if (ImGui::SliderFloat("rotate_all", &rotate_all, -20.0f, 20.0f))
				SubmitTransform(model, renderer, rotate_all, rotate_all, rotate_all, "rotate", "object", change);
			




			/*if (ImGui::SliderFloat("scale_x", &scale_x, -20.0f, 20.0f)
				std::shared_ptr<MeshModel> model = models[name];
			else */
			static float CAMscale_x = 1.0f, CAMscale_y = 1.0f, CAMscale_z = 1.0f, CAMtr_x = 0.0f, CAMtr_y = 0.0f, CAMtr_z = 0.0f;

			if (ImGui::SliderFloat("CAMscale_x", &CAMscale_x, -20.0f, 20.0f) ||
				ImGui::SliderFloat("CAMscale_y", &CAMscale_y, -20.0f, 20.0f) ||
				ImGui::SliderFloat("CAMscale_z", &CAMscale_z, -20.0f, 20.0f))
			{
				SubmitTransform(model, renderer, CAMscale_x, CAMscale_y, CAMscale_z, "scale", "world", change);
				CAMscale_x = 1.0f, CAMscale_y = 1.0f, CAMscale_z = 1.0f;
			}

			if (ImGui::SliderFloat("CAMtr_x", &CAMtr_x, -20.0f, 20.0f) ||
				ImGui::SliderFloat("CAMtr_y", &CAMtr_y, -20.0f, 20.0f) ||
				ImGui::SliderFloat("CAMtr_z", &CAMtr_z, -20.0f, 20.0f))
			{
				SubmitTransform(model, renderer, CAMtr_x, CAMtr_y, CAMtr_z, "translate", "world", change);
				CAMtr_x = 0.0f, CAMtr_y = 0.0f, CAMtr_z = 0.0f;
			}


			if (ImGui::SliderFloat("CAM_x", &CAM_x, -20.0f, 20.0f) ||
				ImGui::SliderFloat("CAM_y", &CAM_y, -20.0f, 20.0f) ||
				ImGui::SliderFloat("CAM_z", &CAM_z, -20.0f, 20.0f))
			{
				SubmitTransform(model, renderer, CAM_x, CAM_z, CAM_z, "rotate", "world", change);
				CAM_x = 0.0f, CAM_y = 0.0f, CAM_y = 0.0f;
			}

			if (ImGui::SliderFloat("zoom", &zoom, -20.0f, 20.0f)||
			ImGui::SliderFloat("left", &left, -20.0f, 20.0f)||
			ImGui::SliderFloat("right", &right, -20.0f, 20.0f)||
			ImGui::SliderFloat("bottom", &bottom, -20.0f, 20.0f)|| 
			ImGui::SliderFloat("top", &top, -20.0f, 20.0f)||
			ImGui::SliderFloat("zNear", &zNear, -20.0f, 20.0f)||
			ImGui::SliderFloat("zFar", &zFar, -20.0f, 20.0f)||
			ImGui::SliderFloat("fovy", &fovy, -20.0f, 20.0f)||
			ImGui::SliderFloat("aspect", &aspect, -20.0f, 20.0f))
				change = 1;

			ImGui::RadioButton("Perspective", &c, 0) ; ImGui::SameLine(); //TODO change = 1;
			ImGui::RadioButton("Orthographic", &c, 1);

			if (c) // TODO change = 1;
				camera.SetOrthographicProjection(1, 1, 10, 150);
				//camera.SetOrthographicProjection(fovy, aspect, zNear, zFar);
			else
				camera.SetPerspectiveProjection(45, 1, 100, 1000);
				//camera.SetPerspectiveProjection(fovy, aspect, zNear, zFar);


			if (ImGui::Button("Draw Normals")) {
				toDrawNormals += mul;
				mul *= -1;
				change = 1;
			}
			if (toDrawNormals) {
				ImGui::SliderFloat("normal_size", &normal_size, 0.001f, 1.0f);

				ImGui::RadioButton("face", &e, 0); ImGui::SameLine();
				ImGui::RadioButton("vertex", &e, 1);
				if (e)
					draw_genre = "vertex";
				else
					draw_genre = "face";
				scene.SetDrawNormals(toDrawNormals, draw_genre, normal_size);
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
						MeshModel addM = Utils::LoadMeshModel(outPath);
						scene.AddModel(std::make_shared<MeshModel>(addM));
						scene.SetActiveModelIndex(scene.GetModelCount() - 1);
						free(outPath);
						change = 1;
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

void SubmitTransform(std::shared_ptr<MeshModel> model, Renderer& renderer, float x, float y, float z, std::string name, std::string genreTransformation, int& change)
{
	model->SetTransform(name);
	model->SetCordinates({ x, y, z }, name);
	renderer.SetTransformation(*model, genreTransformation);
	model->InitCordinate();
	change = 1;
}


