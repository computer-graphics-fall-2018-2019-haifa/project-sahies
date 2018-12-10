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
static int c_idx = 0;
static float rotate_cam = 1.0f;



glm::vec4 clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.00f);

const glm::vec4& GetClearColor()
{
	return clearColor;
}

void cleanCamerasFromModels(std::vector<std::string>& models_names) {
	for (int i = 0; i < models_names.size(); i++) {
		if (models_names[i].find("Camera") != std::string::npos) {
			models_names.erase(models_names.begin() + i);
			i--;
		}
	}
}


void DrawImguiMenus(ImGuiIO& io, Scene& scene, Renderer& renderer, int& change)
{
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (showDemoWindow)
	{
		ImGui::ShowDemoWindow(&showDemoWindow);
	}


	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window

	std::string camera_path = "C:\\project-sahies\\project-sahies\\Data\\camera.obj";
	static int CameraCounter = 0;
	static int counter = 0;
	static float scale_x = 1.0f, scale_y = 1.0f, scale_z = 1.0f, world_y = 0.0f, world_z = 0.0f, scale_all = 1.0f, tr_all = 1.0f, rotate_all = 0.0,
		zoom, left, right, bottom, top, zFar, aspect, normal_size;
	std::string projectionType, draw_genre;
	int e;

	if (scene.GetCameraCount() == 0) {
		std::shared_ptr<MeshModel> cam_obj = std::make_shared<MeshModel>(Utils::LoadMeshModel(camera_path));
		cam_obj->SetModelName("Camera" + std::to_string(CameraCounter));
		scene.AddModel(cam_obj);
		CameraCounter++;
		scene.AddCamera(std::make_shared<Camera>(glm::vec3(2, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), *cam_obj));
		scene.SetActiveCameraIndex(scene.GetCameraCount() - 1);
		change = 1;
	}


	std::vector<std::shared_ptr<Camera>>cameras = scene.GetCameras();
	std::shared_ptr<Camera> camera = cameras[scene.GetActiveCameraIndex()];
	

	std::vector <std::shared_ptr<MeshModel>> models = scene.GetModels();
	std::shared_ptr<MeshModel> model = scene.GetModel(scene.GetActiveModelIndex());


	ImGui::Begin("Or Berger AND Mor Vaknin PlayGround");
	ImGui::Text("Background Color:");
	ImGui::ColorEdit3("Clear Color", (float*)&clearColor); // Edit 3 floats representing a color



	/*****************   Camera Menu   *****************/
	if (ImGui::CollapsingHeader("Cameras"))
	{
		//add camera btn
		if (ImGui::Button("Add Camera")) {
			std::shared_ptr<MeshModel> cam_obj = std::make_shared<MeshModel>(Utils::LoadMeshModel(camera_path));
			cam_obj->SetModelName("Camera" + std::to_string(CameraCounter));
			CameraCounter++;
			scene.AddModel(cam_obj);
			scene.AddCamera(std::make_shared<Camera>(glm::vec3(2, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), *cam_obj));
			scene.SetActiveCameraIndex(scene.GetCameraCount() - 1);
			change = 1;
			c_idx++;
		}

		//make a list of the cameras names
		std::vector<std::string> cameras_names = scene.GetCamerasNames();
		if (cameras_names.size() > 0) {

			char** c_cameras_names = new char*[cameras_names.size()];
			for (int i = 0; i < cameras_names.size(); i++)
				c_cameras_names[i] = (char*)(cameras_names[i].c_str());

			static int acc = c_idx;
			ImGui::Combo("Active Camera", &c_idx, c_cameras_names, cameras_names.size());
			scene.SetActiveCameraIndex(c_idx);
			camera = scene.GetCamera(scene.GetActiveCameraIndex());
			if (acc != c_idx) {
				change = 1;
				acc = c_idx;
			}
				


			// Need to change scene veiwport here
			delete[] c_cameras_names;
		}



		//Near, Far and Ratio Slide Bar
		static float _near = 1.0f, _far = 1.0f, _ratio = 1.0f;
		if (ImGui::SliderFloat("Near", &_near, 0.0f, 250.0f)) {
			camera->zNear = _near;
			change = 1;
		}
		if (ImGui::SliderFloat("Far", &_far, 0.0f, 250.0f)) {
			camera->zFar = _far;
			change = 1;
		}
		if (ImGui::SliderFloat("Ratio", &_ratio, 0.0f, 250.0f)) {
			camera->aspect = _ratio;
			change = 1;
		}

		static float _eye[3] = {
			camera->eye.x,
			camera->eye.y,
			camera->eye.z
		};
		if (ImGui::InputFloat3("Eye", _eye)) {
			camera->eye.x = _eye[0];
			camera->eye.y = _eye[1];
			camera->eye.z = _eye[2];
			change = 1;
			camera->SetCameraLookAt(camera->eye, camera->at, camera->up);
		}

		static float _at[3] = {
			camera->at.x,
			camera->at.y,
			camera->at.z
		};
		if (ImGui::InputFloat3("At", _at)) {
			camera->at.x = _at[0];
			camera->at.y = _at[1];
			camera->at.z = _at[2];
			change = 1;
			camera->SetCameraLookAt(camera->eye, camera->at, camera->up);

		}

		static float _up[3] = {
			camera->up.x,
			camera->up.y,
			camera->up.z
		};
		if (ImGui::InputFloat3("Up", _up)) {
			camera->up.x = _up[0];
			camera->up.y = _up[1];
			camera->up.z = _up[2];
			change = 1;
			camera->SetCameraLookAt(camera->eye, camera->at, camera->up);

		}
		



		enum Mode
		{
			Perspective,
			Orthographic
		};

		static int mode = 1;
		if (ImGui::RadioButton("Perspective", mode == Perspective)) {
			mode = Perspective; 
			change = 1;
		} ImGui::SameLine();
		if (ImGui::RadioButton("Orthographic", mode == Orthographic)) {
			mode = Orthographic; 
			change = 1;
		}

		if (ImGui::SliderFloat("Rotate Camera", &rotate_cam, 0.0f,360.0f)) {
			change = 1;
			SubmitTransform(camera, renderer, rotate_cam, rotate_cam, rotate_cam, "rotate", "object", change);
			camera->SetCameraLookAt(camera->eye, camera->at, camera->up);
		}








		if (mode == Perspective) {
			//Fovy Slide Bar
			static float fovy = 1.0f;
			if (ImGui::SliderFloat("Fovy", &fovy, 0.0f, 250.0f)) {
				camera->fovy = fovy;
				change = 1;
			}
			camera->SetPerspectiveProjection();
		}
		else
		{
			//Height Slide Bar
			static float height = 2.5f;
			if (ImGui::SliderFloat("Height", &height, 0.0f, 250.0f))
			{
				camera->height = height;
				change = 1;
			}
			camera->SetOrthographicProjection();
		}

		

		


		//Zoom Slide Bar
		static float zoom = 0.0f;
		if (ImGui::SliderFloat("Zoom", &zoom, 0.0f, 250.0f))
			for (auto m : models)
				SubmitTransform(m, renderer, zoom, zoom, zoom, "scale", "world", change);




		//Focus Slide Bar
		static float focus = 0;
		if (ImGui::SliderFloat("Focus", &focus, 0.0f, 250.0f)) {
			camera->SetFocus(focus, *model);
			change = 1;
		}



		if (ImGui::SliderFloat("rotate_all", &rotate_all, -20.0f, 20.0f))
			SubmitTransform(model, renderer, rotate_all, rotate_all, rotate_all, "rotate", "object", change);


		static float worldscale_x = 1.0f, worldscale_y = 1.0f, worldscale_z = 1.0f, worldtranslate_x = 0.0f, worldtranslate_y = 0.0f, worldtranslate_z = 0.0f;
		if (ImGui::SliderFloat("worldscale_x", &worldscale_x, -20.0f, 20.0f)||
			ImGui::SliderFloat("worldscale_y", &worldscale_y, -20.0f, 20.0f) ||
			ImGui::SliderFloat("worldscale_z", &worldscale_z, -20.0f, 20.0f))
		{
			SubmitTransform(model, renderer, worldscale_x, worldscale_x, worldscale_x, "scale", "world", change);
			worldscale_x = 1.0f, worldscale_y = 1.0f, worldscale_z = 1.0f;
		}

		if (ImGui::SliderFloat("worldtranslate_x", &worldtranslate_x, -20.0f, 20.0f) ||
			ImGui::SliderFloat("worldtranslate_y", &worldtranslate_y, -20.0f, 20.0f) ||
			ImGui::SliderFloat("worldtranslate_z", &worldtranslate_z, -20.0f, 20.0f))
		{
			SubmitTransform(model, renderer, worldtranslate_x, worldtranslate_y, worldtranslate_z, "translate", "world", change);
			worldtranslate_x = 0.0f, worldtranslate_y = 0.0f, worldtranslate_z = 0.0f;
		}

		static float world_x = 0.0f;
		if (ImGui::SliderFloat("world_x", &world_x, -20.0f, 20.0f) ||
			ImGui::SliderFloat("world_y", &world_y, -20.0f, 20.0f) ||
			ImGui::SliderFloat("world_z", &world_z, -20.0f, 20.0f))
		{
			for (auto model : models) {
				SubmitTransform(model, renderer, world_x, world_z, world_z, "rotate", "world", change);
				world_x = 0.0f, world_y = 0.0f, world_y = 0.0f;
			}

		}






	}


	/*****************   Models Menu   *****************/
	if (ImGui::CollapsingHeader("Models"))
	{
		//make a list of the cameras names
		std::vector<std::string> models_names = scene.GetModelsNames();
		cleanCamerasFromModels(models_names);
		if (models_names.size() > 0) {

			char** c_models_names = new char*[models_names.size()];
			for (int i = 0; i < models_names.size(); ++i)
				c_models_names[i] = (char*)(models_names[i].c_str());

			static int modelIdx = 0;
			ImGui::Combo("Choose Model", &modelIdx, c_models_names, models_names.size());
			scene.SetActiveModelIndex(models_names[modelIdx]);
			model = scene.GetModel(scene.GetActiveModelIndex());
			delete[] c_models_names;
		}

		static float scale_model = 1.0f; //>0
		if (ImGui::DragFloat("Scale model", &scale_model, 0.1f)) {
			SubmitTransform(model, renderer, scale_model, scale_model, scale_model, "scale", "object", change);
			scale_x = scale_model;
			scale_y = scale_model;
			scale_z = scale_model;
		}

		if (ImGui::DragFloat("scale x", &scale_x, 0.05f) ||
			ImGui::DragFloat("scale y", &scale_y, 0.05f) ||
			ImGui::DragFloat("scale z", &scale_z, 0.05f))
			SubmitTransform(model, renderer, scale_x, scale_y, scale_z, "scale", "object", change);

		static float  translate_x = 0.0f, translate_y = 0.0f, translate_z = 0.0f;
		if (ImGui::DragFloat("translate x", &translate_x, 0.05f) ||
			ImGui::DragFloat("translate y", &translate_y, 0.05f) ||
			ImGui::DragFloat("translate z", &translate_z, 0.05f))
			SubmitTransform(model, renderer, translate_x, translate_y, translate_z, "translate", "object", change);

		//rotate model around axis
		static float xrotate = 0.0f, yrotate = 0.0f, zrotate = 0.0f;
		if (ImGui::DragFloat("rotate around x", &xrotate, 0.5f)) {
			if (xrotate >= 360 || xrotate <= -360)
				xrotate = 0;
			SubmitTransform(model, renderer, xrotate, yrotate, zrotate, "rotate", "object", change);

		}
		if (ImGui::DragFloat("rotate around y", &yrotate, 0.5f)) {
			if (yrotate >= 360 || yrotate <= -360)
				yrotate = 0;
			SubmitTransform(model, renderer, xrotate, yrotate, zrotate, "rotate", "object", change);

		}
		if (ImGui::DragFloat("rotate around z", &zrotate, 0.5f)) {
			if (zrotate >= 360 || zrotate <= -360)
				zrotate = 0;
			SubmitTransform(model, renderer, xrotate, yrotate, zrotate, "rotate", "object", change);

		}
		/*	ImGui::DragFloat("rotate around y", &yrotate, 0.5f) ||
			ImGui::DragFloat("rotate around z", &zrotate, 0.5f))
		{
			if (xrotate >= 180 || xrotate <= -180) xrotate = 0;
			if (yrotate >= 360 || yrotate <= -360) yrotate = 0;
			if (zrotate >= 180 || zrotate <= -180) zrotate = 0;
			SubmitTransform(model, renderer, xrotate, yrotate, zrotate, "rotate", "object", change);
		}
*/

		/*     Draw Box     */
		static bool drawBox = false;
		ImGui::Checkbox("Draw bounding box", &drawBox);
		if (drawBox) {
			scene.toDrawBox = true;
			change = 1;
		}
		else
			scene.toDrawBox = false;



		/*     Normals     */
		static bool drawNormals = false;
		ImGui::Checkbox("Draw Normals", &drawNormals);
		if (drawNormals) {

			enum norMode
			{
				Vertex,
				Face
			};
			static int normals_mode = 0;
			if (ImGui::RadioButton("Vertex", normals_mode == Vertex)) { normals_mode = Vertex; } ImGui::SameLine();
			if (ImGui::RadioButton("Face", normals_mode == Face)) { normals_mode = Face; }
			ImGui::SliderFloat("Normals size", &normal_size, 0.00f, 3.0f);

			if (normals_mode == Vertex) {// TODO change = 1;
				change = 1;
				scene.SetDrawNormals(true, "vertex", normal_size);
			}
			if (normals_mode == Face) {// TODO change = 1;
				change = 1;
				scene.SetDrawNormals(true, "face", normal_size);
			}

		}
		else 
			scene.SetDrawNormals(false, "vertex", normal_size);



		/*if (toDrawNormals) {
			

			ImGui::RadioButton("face", &e, 0); ImGui::SameLine();
			ImGui::RadioButton("vertex", &e, 1);
			if (e)
				draw_genre = "vertex";
			else
				draw_genre = "face";
			scene.SetDrawNormals(toDrawNormals, draw_genre, normal_size);
		}
		*/


	}
	

	if (ImGui::Button("Button"))  // Buttons return true when clicked (most widgets return true when edited/activated)
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
				if (ImGui::MenuItem("Load Model", "CTRL+O"))
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


			if (ImGui::BeginMenu("Help"))
			{
				ImGui::MenuItem("Demo Window", "CTRL+D", &showDemoWindow);
				ImGui::EndMenu();
			}



			ImGui::EndMainMenuBar();
		}
	}

	

}

void SubmitTransform(std::shared_ptr<MeshModel> model, Renderer& renderer, float x, float y, float z, std::string name, std::string genreTransformation, int& change)
{
	model->SetTransform(name);
	model->SetCordinates(glm::vec3(x, y, z), name);
	renderer.SetTransformation(*model, genreTransformation);

	// the reason z models cant rotate around z axis..
	model->InitCordinate();
	change = 1;
}

