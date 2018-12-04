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

	std::string camera_path = "C:\\Users\\Berger\\Documents\\GitHub\\project-sahies2\\Data\\camera.obj";
	static int CameraCounter = 0;
	static int counter = 0;
	static float scale_x = 1.0f, scale_y = 1.0f, scale_z = 1.0f, world_y = 0.0f, world_z = 0.0f, scale_all = 1.0f, tr_all = 1.0f, rotate_all = 0.0, left, right, bottom, top, zFar, aspect, normal_size;
	std::string projectionType, draw_genre;
	int e;

	if (scene.GetCameraCount() == 0) {
		std::shared_ptr<MeshModel> cam_obj = std::make_shared<MeshModel>(Utils::LoadMeshModel(camera_path));
		cam_obj->SetModelName("Camera " + std::to_string(CameraCounter));
		scene.AddModel(cam_obj);
		CameraCounter++;
		scene.AddCamera(std::make_shared<Camera>(glm::vec3(360), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), *cam_obj));
		scene.SetActiveCameraIndex(scene.GetCameraCount() );
		change = 1;
	}


	static std::shared_ptr<Camera> camera = scene.GetCamera(scene.GetActiveCameraIndex());
	

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
			cam_obj->SetModelName("Camera " + std::to_string(CameraCounter));
			CameraCounter++;
			scene.AddModel(cam_obj);
			scene.AddCamera(std::make_shared<Camera>(glm::vec3(360), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), *cam_obj));
			scene.SetActiveCameraIndex(scene.GetCameraCount() - 1);
			change = 1;
		}

		//make a list of the cameras names
		std::vector<std::shared_ptr<Camera>>cameras = scene.GetCameras();
		std::vector<std::string> camera_names = scene.GetCamerasNames();
		if (camera_names.size() > 0) {

		
			char** c_camera_names = new char*[cameras.size()];
			for (int i = 0; i < cameras.size(); i++)
				c_camera_names[i] = const_cast<char*>(camera_names[i].c_str());
			
			static int active_cam = scene.activeCameraIndex;
			ImGui::Combo("Active Camera", &scene.activeCameraIndex, c_camera_names, camera_names.size());
			camera = scene.GetCamera(scene.activeCameraIndex);
			if (active_cam != scene.activeCameraIndex) {
				change = 1;
				active_cam = scene.activeCameraIndex;
			}
			
			
			


			// Need to change scene veiwport here
			delete[] c_camera_names;
		}

		//Top,Bttm,Left,Right Slide Bar
		static float top = 1.0f, bttm = 1.0f, left = 1.0f, right=1.0f;
		if (ImGui::SliderFloat("Top", &top, 0.0f, 250.0f)) {
			camera->top = top;
			change = 1;
			camera->byTopBttm = true;
		}
		if (ImGui::SliderFloat("Bottom", &bttm, 0.0f, 250.0f)) {
			camera->bottom = bttm;
			change = 1;
			camera->byTopBttm = true;
		}
		if (ImGui::SliderFloat("Left", &left, 0.0f, 250.0f)) {
			camera->left = left;
			change = 1;
			camera->byTopBttm = true;
		}
		if (ImGui::SliderFloat("Right", &right, 0.0f, 250.0f)) {
			camera->right = right;
			change = 1;
			camera->byTopBttm = true;
		}


		//Near, Far and Ratio Slide Bar
		static float _near = 1.0f, _far = 1.0f, _ratio = 1.0f;
		if (ImGui::SliderFloat("Near", &_near, 0.0f, 250.0f)) {
			camera->zNear = _near;
			change = 1;
			camera->byTopBttm = false;
		}
		if (ImGui::SliderFloat("Far", &_far, 0.0f, 250.0f)) {
			camera->zFar = _far;
			change = 1;
			camera->byTopBttm = false;
		}
		if (ImGui::SliderFloat("Ratio", &_ratio, 0.0f, 250.0f)) {
			camera->aspect = _ratio;
			change = 1;
			camera->byTopBttm = false;
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




		if (mode == Perspective) {
			//Fovy Slide Bar
			static float fovy = 1.0f;
			if (ImGui::SliderFloat("Fovy", &fovy, 0.0f, 250.0f)) {
				camera->fovy = fovy;
				change = 1;
				camera->byTopBttm = false;
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
				camera->byTopBttm = false;
			}
			camera->SetOrthographicProjection();
		}

		

		
		static float rt_w_x = 0.0f, rt_w_y = 0.0f, rt_l_x = 0.0f, rt_l_y = 0.0f;
		if (ImGui::DragFloat("Rotate Camera World X", &rt_w_x, 0.2f)) {
			camera->at.x = rt_w_x;
			change = 1;
			camera->SetCameraLookAt();
		}

		if (ImGui::DragFloat("Rotate Camera World Y", &rt_w_y, 0.2f)) {
			camera->at.y = rt_w_y;
			change = 1;
			camera->SetCameraLookAt();
		}

		

		if (ImGui::DragFloat("Rotate Camera Local X", &rt_l_x, 0.2f)) {
			SubmitTransform(model, renderer, rt_l_x, 1, 1, "rotate", "object", change);
			change = 1;
			camera->SetCameraLookAt(camera->eye, camera->at, camera->up);
		}

		if (ImGui::DragFloat("Rotate Camera Local Y", &rt_l_y, 0.2f)) {
			SubmitTransform(model, renderer, 1, rt_l_y, 1, "rotate", "object", change);
			change = 1;
			camera->SetCameraLookAt(camera->eye, camera->at, camera->up);
		}

		//Zoom Slide Bar
		static float z = 0.0f;
		if (ImGui::DragFloat("Zoom", &z, 0.1f)) {
			camera->zoom = {1/z, 1/z, 1};
			camera->SetCameraLookAt();
		}



		//Focus Slide Bar
		static bool focus = 0;
		ImGui::Checkbox("Focus", &focus);
		if (focus) {
			camera->SetFocus(*(scene.GetModel(scene.GetActiveModelIndex())));
			change = 1;
		}
		else {
			camera->SetCameraLookAt();
		}




		/*if (ImGui::SliderFloat("scale_all", &scale_all, 0.0f, 250.0f))
			SubmitTransform(model, renderer, scale_all, scale_all, scale_all, "scale", "object", change);
			*/

			// Scale only one axis
			/*if (ImGui::SliderFloat("scale_x", &scale_x, 0.0f, 250.0f) ||
				ImGui::SliderFloat("scale_y", &scale_y, 0.0f, 250.0f) ||
				ImGui::SliderFloat("scale_z", &scale_z, 0.0f, 250.0f))
				SubmitTransform(model, renderer, scale_x, scale_y, scale_z, "scale", "object", change);

	*/


		static float worldrotate_x = 0, worldrotate_y = 0, worldrotate_z = 0;
		if (ImGui::SliderFloat("worldrotate_x", &worldrotate_x, -20.0f, 20.0f) ||
			ImGui::SliderFloat("worldrotate_y", &worldrotate_y, -20.0f, 20.0f) ||
			ImGui::SliderFloat("worldrotate_z", &worldrotate_z, -20.0f, 20.0f))
			SubmitTransform(camera, renderer, worldrotate_x, worldrotate_y, worldrotate_z, "rotate", "world", change);


		static float worldscale_x = 1.0f, worldscale_y = 1.0f, worldscale_z = 1.0f, worldtranslate_x = 0.0f, worldtranslate_y = 0.0f, worldtranslate_z = 0.0f;
		if (ImGui::SliderFloat("worldscale_x", &worldscale_x, -20.0f, 20.0f) || 
			ImGui::SliderFloat("worldscale_y", &worldscale_y, -20.0f, 20.0f) ||
			ImGui::SliderFloat("worldscale_z", &worldscale_z, -20.0f, 20.0f))
		{
			SubmitTransform(camera, renderer, worldscale_x, worldscale_y, worldscale_z, "scale", "world", change);
		}

		if (ImGui::SliderFloat("worldtranslate_x", &worldtranslate_x, -20.0f, 20.0f) ||
			ImGui::SliderFloat("worldtranslate_y", &worldtranslate_y, -20.0f, 20.0f) ||
			ImGui::SliderFloat("worldtranslate_z", &worldtranslate_z, -20.0f, 20.0f))
		{
			SubmitTransform(camera, renderer, worldtranslate_x, worldtranslate_y, worldtranslate_z, "translate", "world", change);
		}

		//static float world_x = 0.0f;
		//if (ImGui::SliderFloat("world_x", &world_x, -20.0f, 20.0f) ||
		//	ImGui::SliderFloat("world_y", &world_y, -20.0f, 20.0f) ||
		//	ImGui::SliderFloat("world_z", &world_z, -20.0f, 20.0f))
		//{
		//	SubmitTransform(model, renderer, scale_x, scale_y, scale_z, "scale", "object", change);
		//}






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


	/*	ImGui::ColorEdit3("Model Color", (float*)&mcolor);
		model.SetColor(mcolor);*/

		static float scale_model = 1.0f; //>0
		if (ImGui::DragFloat("Scale model", &scale_model, 0.1f)) {
			SubmitTransform(model, renderer, scale_model, scale_model, scale_model, "scale", "object", change);
			scale_x = scale_model;
			scale_y = scale_model;
			scale_z = scale_model;
		}

		//	static float  scale_x = 0.0f, scale_y = 0.0f, scale_z = 0.0f;
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
		if (ImGui::DragFloat("rotate around x", &xrotate, 0.5f) ||
			ImGui::DragFloat("rotate around y", &yrotate, 0.5f) ||
			ImGui::DragFloat("rotate around z", &zrotate, 0.5f))
		{
			if (xrotate >= 180 || xrotate <= -180) xrotate = 0;
			if (yrotate >= 360 || yrotate <= -360) yrotate = 0;
			if (zrotate >= 180 || zrotate <= -180) zrotate = 0;
			SubmitTransform(model, renderer, xrotate, yrotate, zrotate, "rotate", "object", change);
		}


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


	for (int i = 0; i < scene.cameras.size(); i++) {
		if (scene.cameras[i]->GetModelName() == camera->GetModelName()) {
			scene.cameras[i] = camera;
			break;
		}
	}


	//scene.cameras = cameras;

}

void SubmitTransform(std::shared_ptr<MeshModel> model, Renderer& renderer, float x, float y, float z, std::string name, std::string genreTransformation, int& change)
{
	model->SetTransform(name, { x,y,z });
	model->SetCordinates(glm::vec3(x, y, z), name);
	renderer.SetTransformation(*model, genreTransformation);
	// the reason z models cant rotate around z axis..
	model->InitCordinate();
	change = 1;
}

