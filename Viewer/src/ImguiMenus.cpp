#pragma once
#define _USE_MATH_DEFINES

#include "ImguiMenus.h"
#include "MeshModel.h"
#include "Light.h"
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

std::vector<glm::vec3> colors = { glm::vec3(0.87,0.58,0.98), glm::vec3(0.8 , 0.498039 , float(0.196078)),
		glm::vec3(0,1,0),glm::vec3(0,0,1), glm::vec3(1,0,1), glm::vec3(1,1,0), glm::vec3(0,1,1) };
glm::vec4 clearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.00f);
float count_ambient = 0;

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


void DrawImguiMenus(ImGuiIO& io, Scene& scene, Renderer& renderer)
{
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (showDemoWindow)
	{
		ImGui::ShowDemoWindow(&showDemoWindow);
	}


	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window

	std::string camera_path = "C:\\project-sahies2\\Data\\camera.obj";
	std::string light_path = "C:\\project-sahies2\\Data\\light.obj";
	static int CameraCounter = 0;
	static int LightCounter = 0;
	static int counter = 0;
	static float scale_x = 1.0f, scale_y = 1.0f, scale_z = 1.0f, world_y = 0.0f, world_z = 0.0f, scale_all = 1.0f, tr_all = 1.0f, rotate_all = 0.0, left, right, bottom, top, zFar, aspect, normal_size;
	std::string projectionType, draw_genre;
	int e;

	if (scene.GetCameraCount() == 0) {
		std::shared_ptr<MeshModel> cam_obj = std::make_shared<MeshModel>(Utils::LoadMeshModel(camera_path));
		cam_obj->SetModelName("Camera " + std::to_string(CameraCounter));
		scene.AddModel(cam_obj);
		CameraCounter++;
		scene.AddCamera(std::make_shared<Camera>(glm::vec3(0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), *cam_obj));
		scene.SetActiveCameraIndex(scene.GetCameraCount() );
	}


	static std::shared_ptr<Camera> camera = scene.GetCamera(scene.GetActiveCameraIndex());

	std::vector <std::shared_ptr<MeshModel>> models = scene.GetModels();
	std::shared_ptr<MeshModel> model = scene.GetModel(scene.GetActiveModelIndex());


	ImGui::Begin("Or Berger AND Mor Vaknin PlayGround");
	ImGui::Text("Background Color:");
	ImGui::ColorEdit3("Clear Color", (float*)&clearColor); // Edit 3 floats representing a color


	/*****************   Light Menu   *****************/
	if (ImGui::CollapsingHeader("Light"))
	{
		static glm::vec4 lColor(1.0f, 1.0f, 1.0f,1.0f);

		enum Light_Source
		{
			Point,
			Parallel,
			Ambient
		};

		static int light_s = 0;
		static std::string sour;

		if (ImGui::RadioButton("Point", light_s == Point))
		{
			light_s = Point;
			sour = "point";
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Parallel", light_s == Parallel))
		{
			light_s = Parallel;
			sour = "parallel";
		}
		ImGui::SameLine();
		if (count_ambient == 0)
		{
			if (ImGui::RadioButton("Ambient", light_s == Ambient))
			{
				light_s = Ambient;
				sour = "ambient";
			}
		}


		if (ImGui::Button("Add Light"))
		{
			std::shared_ptr<MeshModel> light_obj = std::make_shared<MeshModel>(Utils::LoadMeshModel(light_path));
			
			light_obj->SetModelName("Light " + std::to_string(LightCounter));
			LightCounter++;
			std::shared_ptr<Light> lig = std::make_shared<Light>(*light_obj);
			SubmitTransform(lig, renderer, -6, 3, 0, "translate", "object");
			SubmitTransform(lig, renderer, 0.1, 0.1, 1, "scale", "object");
			SubmitTransform(lig, renderer, 1, -5, 1, "rotate", "object");
			//lig->matTransformations = { Utils::GetMatrix("scale",0.1,0.1,1), Utils::GetMatrix("rotate",1, -5, 1), Utils::GetMatrix("translate",-6,3,0) };
			lig->SetColor(glm::vec3(1));
			lig->source = sour;
			scene.AddLight(lig);
			scene.SetActiveLightIndex(scene.lights.size() - 1);
			std::shared_ptr<Light> active_light = scene.GetLight(scene.activeLightIndex);
	
		}

		//make a list of the cameras names
		std::vector<std::shared_ptr<Light>> lights = scene.GetLights();
		std::vector<std::string> lights_names = scene.GetLightsNames();
		if (lights_names.size() > 0) {




			char** c_light_names = new char*[lights.size()];
			for (int i = 0; i < lights.size(); i++)
				c_light_names[i] = const_cast<char*>(lights_names[i].c_str());

			int active_light_idx = scene.activeLightIndex;
			ImGui::Combo("Active Light", &active_light_idx, c_light_names, lights_names.size());
			scene.SetActiveLightIndex(active_light_idx);
			std::shared_ptr<Light> active_light = scene.GetLight(active_light_idx);
			delete[] c_light_names;

	
		}

		if (scene.lights.size() > 0)
		{
			std::shared_ptr<Light> active_light = scene.lights[scene.activeLightIndex];

			/*static float spec_vault = 0.0f, diffuse_vault = 0.2f;
			if (ImGui::SliderFloat("Specular Vault", &spec_vault, 0.0f, 1.0f) ||
				ImGui::SliderFloat("Diffuse Vault", &diffuse_vault, 0.0f, 1.0f)) {
				glm::vec3 m = glm::vec3(diffuse_vault, 0, spec_vault);
				active_light->SetMaterialVault(m);
			}static float ambient_vault = 0.0f;
			ImGui::SliderFloat("Ambient Vault", &active_light->vault_ambient, 0.0f, 1.0f);*/

			if (ImGui::SliderFloat("Light Power", &active_light->light_power, 0.0f, 1.0f));
		
			if (ImGui::ColorEdit3("Light Color", (float*)&lColor))
				active_light->SetColor(lColor);

		
			static bool fog = false;
			static glm::vec3 fogColor = glm::vec3(1);
			ImGui::Checkbox("Fog", &fog);
			if (fog) {
				scene.fog = true;
				if (ImGui::ColorEdit3("Fog Color", (float*)&fogColor))
					scene.fog_color = fogColor;
			}
			else
				scene.fog = false;

			enum Mode
			{
				Flat,
				Gouraud,
				Phong
			};

			static int style = 0;
		
			if (ImGui::RadioButton("Flat", style == Flat)) {
				style = Flat;
				scene.draw_style = "flat";
			} ImGui::SameLine();
			if (ImGui::RadioButton("Gouraud", style == Gouraud)) {
				style = Gouraud; 
				scene.draw_style = "gouraud";
			}
			if (ImGui::RadioButton("Phong", style == Phong)) {
				style = Phong;
				scene.draw_style = "phong";
			}

			static float scale_light = 1.0f;
			if (ImGui::DragFloat("Scale Light", &active_light->scale.x, 0.1f)) {
				SubmitTransform(active_light, renderer, active_light->scale.x, active_light->scale.x, active_light->scale.x, "scale", "object");
				active_light->scale.y = active_light->scale.x;
				active_light->scale.z = active_light->scale.x;
			}

			static float   translate_light_y = 0.0f, translate_light_z = 0.0f;
			if (ImGui::DragFloat("translate x light", &active_light->translate.x, 0.5f) ||
				ImGui::DragFloat("translate y light", &active_light->translate.y, 0.5f) ||
				ImGui::DragFloat("translate z light", &active_light->translate.z, 0.5f))
				SubmitTransform(active_light, renderer, active_light->translate.x, active_light->translate.y, active_light->translate.z, "translate", "object");

			//rotate model around axis
			static float light_xrotate = 0.0f, light_yrotate = 0.0f, light_zrotate = 0.0f;
			if (ImGui::DragFloat("rotate around x light", &active_light->rotate.x, 0.5f) ||
				ImGui::DragFloat("rotate around y light", &active_light->rotate.y, 0.5f) ||
				ImGui::DragFloat("rotate around z light", &active_light->rotate.z, 0.5f))
			{
				if (active_light->rotate.x >= 180 || active_light->rotate.x <= -180) light_xrotate = 0;
				if (active_light->rotate.y >= 360 || active_light->rotate.y <= -360) light_yrotate = 0;
				if (active_light->rotate.z >= 180 || active_light->rotate.z <= -180) light_zrotate = 0;
				SubmitTransform(active_light, renderer, active_light->rotate.x, active_light->rotate.y, active_light->rotate.z, "rotate", "object");
			}

		/*	if (active_light->source == "ambient")
			{
				count_ambient++;
				static float ambient_vault = 0.0f;
				ImGui::SliderFloat("Ambient Vault", &active_light->vault_ambient, 0.0f, 1.0f);
			}*/
		}
	}

	/*****************   Camera Menu   *****************/
	if (ImGui::CollapsingHeader("Cameras"))
	{
		//add camera btn
		if (ImGui::Button("Add Camera")) {
			std::shared_ptr<MeshModel> cam_obj = std::make_shared<MeshModel>(Utils::LoadMeshModel(camera_path));
			cam_obj->SetModelName("Camera " + std::to_string(CameraCounter));
			CameraCounter++;
			scene.AddModel(cam_obj);
			scene.AddCamera(std::make_shared<Camera>(glm::vec3(0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), *cam_obj));
			scene.SetActiveCameraIndex(scene.GetCameraCount() - 1);
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
			if (active_cam != scene.activeCameraIndex) 
				active_cam = scene.activeCameraIndex;
			
			
			
			


			// Need to change scene veiwport here
			delete[] c_camera_names;
		}

		//Top,Bttm,Left,Right Slide Bar
		static float top = 1.0f, bttm = 1.0f, left = 1.0f, right=1.0f;
		if (ImGui::SliderFloat("Top", &top, 0.0f, 250.0f)) {
			camera->top = top;
			camera->byTopBttm = true;
		}
		if (ImGui::SliderFloat("Bottom", &bttm, 0.0f, 250.0f)) {
			camera->bottom = bttm;
			camera->byTopBttm = true;
		}
		if (ImGui::SliderFloat("Left", &left, 0.0f, 250.0f)) {
			camera->left = left;
			camera->byTopBttm = true;
		}
		if (ImGui::SliderFloat("Right", &right, 0.0f, 250.0f)) {
			camera->right = right;
			camera->byTopBttm = true;
		}


		//Near, Far and Ratio Slide Bar
		static float _near = 10.0f, _far = 100.0f, _ratio = 1.0f;
		if (ImGui::SliderFloat("Near", &_near, 0.0f, 250.0f)) {
			camera->zNear = _near;
			camera->byTopBttm = false;
		}
		if (ImGui::SliderFloat("Far", &_far, 0.0f, 250.0f)) {
			camera->zFar = _far;
			camera->byTopBttm = false;
		}
		if (ImGui::SliderFloat("Ratio", &_ratio, 0.0f, 250.0f)) {
			camera->aspect = _ratio;
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
		} ImGui::SameLine();
		if (ImGui::RadioButton("Orthographic", mode == Orthographic)) {
			mode = Orthographic; ;
		}




		if (mode == Perspective) {
			//Fovy Slide Bar
			static float fovy = 1.0f;
			if (ImGui::SliderFloat("Fovy", &fovy, 0.0f, 250.0f)) {
				camera->fovy = fovy;
				camera->byTopBttm = false;
			}
			scene.isPerspective = true;
			camera->SetPerspectiveProjection();
		}
		else
		{
			//Height Slide Bar
			static float height = 2.5f;
			scene.isPerspective = false;
			if (ImGui::SliderFloat("Height", &height, 0.0f, 250.0f))
			{
				camera->height = height;
				camera->byTopBttm = false;
			}
			camera->SetOrthographicProjection();
		}

		

		
		static float rt_w_x = 0.0f, rt_w_y = 0.0f, rt_l_x = 0.0f, rt_l_y = 0.0f;
		if (ImGui::DragFloat("Rotate Camera World X", &rt_w_x, 0.2f)) {
			camera->at.x = rt_w_x;
			camera->SetCameraLookAt();
		}

		if (ImGui::DragFloat("Rotate Camera World Y", &rt_w_y, 0.2f)) {
			camera->at.y = rt_w_y;
			camera->SetCameraLookAt();
		}

		

		if (ImGui::DragFloat("Rotate Camera Local", &rt_l_x, 0.2f)) {
			camera->up.x = rt_l_x;
			camera->SetCameraLookAt();
		}




		//Zoom Slide Bar
		static float z = 0.0f;
		if (ImGui::DragFloat("Zoom", &z, 0.1f)) {
			camera->zoom = {1/z, 1/z, 1};
			camera->SetCameraLookAt();
		}



		//Focus Slide Bar
		static bool focus = false;
		ImGui::Checkbox("Focus", &focus);
		if (focus) {
			camera->SetFocus(*(scene.GetModel(scene.GetActiveModelIndex())));
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
			SubmitTransform(camera, renderer, worldrotate_x, worldrotate_y, worldrotate_z, "rotate", "world");


		static float worldscale_x = 1.0f, worldscale_y = 1.0f, worldscale_z = 1.0f, worldtranslate_x = 0.0f, worldtranslate_y = 0.0f, worldtranslate_z = 0.0f;
		if (ImGui::SliderFloat("worldscale_x", &worldscale_x, -20.0f, 20.0f) || 
			ImGui::SliderFloat("worldscale_y", &worldscale_y, -20.0f, 20.0f) ||
			ImGui::SliderFloat("worldscale_z", &worldscale_z, -20.0f, 20.0f))
		{
			SubmitTransform(camera, renderer, worldscale_x, worldscale_y, worldscale_z, "scale", "world");
		}

		if (ImGui::SliderFloat("worldtranslate_x", &worldtranslate_x, -20.0f, 20.0f) ||
			ImGui::SliderFloat("worldtranslate_y", &worldtranslate_y, -20.0f, 20.0f) ||
			ImGui::SliderFloat("worldtranslate_z", &worldtranslate_z, -20.0f, 20.0f))
		{
			SubmitTransform(camera, renderer, worldtranslate_x, worldtranslate_y, worldtranslate_z, "translate", "world");
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

		
		/*static glm::vec4 mColor(colors[rand() % 6],0);*/
		static glm::vec4 mColor(1.0f, 1.0f, 1.0f, 0.0f);
		if (ImGui::ColorEdit3("Model Color", (float*)&mColor))
			model->SetColor(mColor);

		//static float scale_model = 1.0f; 
		if (ImGui::DragFloat("Scale model", &model->scale_model, 0.1f)) {
			SubmitTransform(model, renderer, model->scale_model, model->scale_model, model->scale_model, "scale", "object");
			model->scale.x = model->scale_model;
			model->scale.y = model->scale_model;
			model->scale.z = model->scale_model;
		}

		//	static float  scale_x = 0.0f, scale_y = 0.0f, scale_z = 0.0f;
		if (ImGui::DragFloat("scale x", &model->scale.x, 0.05f) ||
			ImGui::DragFloat("scale y", &model->scale.y, 0.05f) ||
			ImGui::DragFloat("scale z", &model->scale.z, 0.05f))
			SubmitTransform(model, renderer, model->scale.x, model->scale.y, model->scale.z, "scale", "object");

		static float  translate_x = 0.0f, translate_y = 0.0f, translate_z = 0.0f;
		if (ImGui::DragFloat("translate x", &model->translate.x, 0.5f) ||
			ImGui::DragFloat("translate y", &model->translate.y, 0.5f) ||
			ImGui::DragFloat("translate z", &model->translate.z, 0.5f))
			SubmitTransform(model, renderer, model->translate.x, model->translate.y, model->translate.z, "translate", "object");

		//rotate model around axis
		static float xrotate = 0.0f, yrotate = 0.0f, zrotate = 0.0f;
		if (ImGui::DragFloat("rotate around x", &xrotate, 0.5f) ||
			ImGui::DragFloat("rotate around y", &yrotate, 0.5f) ||
			ImGui::DragFloat("rotate around z", &zrotate, 0.5f))
		{
			if (xrotate >= 180 || xrotate <= -180) xrotate = 0;
			if (yrotate >= 360 || yrotate <= -360) yrotate = 0;
			if (zrotate >= 180 || zrotate <= -180) zrotate = 0;
			SubmitTransform(model, renderer, xrotate, yrotate, zrotate, "rotate", "object");
		}

		ImGui::SliderFloat("Diffuse", &model->diffuse, 0.0f, 1.0f);

		ImGui::SliderFloat("Specular", &model->specular, 0.0f, 1.0f);
		ImGui::DragInt("Specular exponent", &model->exponent, 1);

		ImGui::SliderFloat("Ambient", &model->ambient, 0.0f, 1.0f);


		/*     Draw Box     */
		static bool drawBox = false;
		ImGui::Checkbox("Draw bounding box", &drawBox);
		if (drawBox) 
			scene.toDrawBox = true;
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
			ImGui::SliderFloat("Normals size", &normal_size, 0.00f, 50.0f);

			if (normals_mode == Vertex) 
				scene.SetDrawNormals(true, "vertex", normal_size);
			
			if (normals_mode == Face) 
				scene.SetDrawNormals(true, "face", normal_size);
			

		}
		else 
			scene.SetDrawNormals(false, "vertex", normal_size);




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
						std::shared_ptr<MeshModel> addM = std::make_shared<MeshModel>(Utils::LoadMeshModel(outPath));
						addM->SetColor(glm::vec3(1));
						scene.AddModel(addM);
						scene.SetActiveModelIndex(scene.GetModelCount() - 1);
						//(scene.GetModel(scene.GetActiveModelIndex()))->SetColor(colors[rand() % 6]);
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

void SubmitTransform(std::shared_ptr<MeshModel> model, Renderer& renderer, float x, float y, float z, std::string name, std::string genreTransformation)
{
	model->SetTransform(name, { x,y,z });
	model->SetCordinates(glm::vec3(x, y, z), name);
	renderer.SetTransformation(*model, genreTransformation);
	model->InitCordinate();
}

