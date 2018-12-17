#define _USE_MATH_DEFINES

#include "Renderer.h"
#include "InitShader.h"
#include "MeshModel.h"
#include "Utils.h"
#include <imgui/imgui.h>
#include <vector>
#include <cmath>
#include "ImguiMenus.h"


#define inTriangle(a,b) (a >= 0 && b >= 0 && (a + b <= 1)) ? 1:0
#define sign(x) ((x > 0) ? 1 : ((x < 0) ? -1 : 0))
#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)
#define Z_INDEX(w,i,j) ((i)+(j)*(w))
#define INFINITE 100000
#define GRID_LENGTH 1000
#define GRID_DELTA 50

std::vector<glm::vec3> face_norm;
std::vector<glm::vec3> notFace_norm;

Renderer::Renderer(int viewportWidth, int viewportHeight, int viewportX, int viewportY) :
	colorBuffer(nullptr),
	zBuffer(nullptr)
{
	initOpenGLRendering();
	SetViewport(viewportWidth, viewportHeight, viewportX, viewportY);
	for (int i = 0; i < viewportWidth; ++i)
	{
		for (int j = 0; j < viewportHeight; ++j)
			zBuffer[Z_INDEX(viewportWidth, i, j)] = INFINITE;
	}
}


template <class T>
void swap(T& x, T& y)
{
	T temp;
	temp = x;
	x = y;
	y = temp;
}

void Renderer::bresenham_line(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, glm::vec3& color) {
	// Bresenham's line algorithm
	const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));

	bool m = (fabs(y2 - y1) > fabs(x2 - x1));
	if (m)
	{
		swap(x1, y1);
		swap(x2, y2);
	}

	if (x1 > x2)
	{
		swap(x1, x2);
		swap(y1, y2);
	}

	GLfloat dx = (x2 - x1), dy = fabs(y2 - y1), error = dx / 2.0f;
	int yIdx = (int)y1, x2int = (int)x2;
	for (int xIdx = (int)x1; xIdx < x2int; xIdx++)
	{
		if (m)
		{
			putPixel(yIdx, xIdx, color, 0);
		}
		else
		{
			putPixel(xIdx, yIdx, color, 0);
		}
		error -= dy;
		if (error < 0)
		{
			if (y1 < y2)
				yIdx++;
			else
				yIdx--;
			error += dx;
		}
	}
}

Renderer::~Renderer()
{
	if (colorBuffer)
	{
		delete[] colorBuffer;
	}
	delete[] zBuffer;
}


void Renderer::putPixel(int i, int j, const glm::vec3& color, float z)
{
	if (i < 0) return; if (i >= viewportWidth) return;
	if (j < 0) return; if (j >= viewportHeight) return;
	if (z < zBuffer[Z_INDEX(viewportWidth, i, j)])
	{
		colorBuffer[INDEX(viewportWidth, i, j, 0)] = color.x;
		colorBuffer[INDEX(viewportWidth, i, j, 1)] = color.y;
		colorBuffer[INDEX(viewportWidth, i, j, 2)] = color.z;
		zBuffer[Z_INDEX(viewportWidth, i, j)] = z;
	}
}


void Renderer::createBuffers(int viewportWidth, int viewportHeight)
{
	if (colorBuffer)
	{
		delete[] colorBuffer;
	}

	colorBuffer = new float[3 * viewportWidth * viewportHeight];
	zBuffer = new float[viewportWidth * viewportHeight];
	for (int x = 0; x < viewportWidth; x++)
	{
		for (int y = 0; y < viewportHeight; y++)
		{
			putPixel(x, y, glm::vec3(0.0f, 0.0f, 0.0f), 1.1);
		}
	}
}

void Renderer::ClearColorBuffer(const glm::vec3& color)
{
	for (int i = 0; i < viewportWidth; i++)
	{
		for (int j = 0; j < viewportHeight; j++)
		{
			putPixel(i, j, color, 1.1);
			zBuffer[Z_INDEX(viewportWidth, i, j)] = INFINITE;

		}
	}
}


void Renderer::SetViewport(int viewportWidth, int viewportHeight, int viewportX, int viewportY)
{
	this->viewportX = viewportX;
	this->viewportY = viewportY;
	this->viewportWidth = viewportWidth;
	this->viewportHeight = viewportHeight;
	createBuffers(viewportWidth, viewportHeight);
	createOpenGLBuffer();
}

void Renderer::SetTransformation(MeshModel& model, std::string genreTransformation)
{
	std::string transform = model.GetTransform();
	glm::vec3 cordinates = model.GetCordinates(transform);
	glm::mat4 matrix = Utils::GetMatrix(transform, cordinates.x, cordinates.y, cordinates.z);
	model.setMatrix(matrix, transform, genreTransformation);
}


std::vector<glm::vec3> Renderer::VerticesXmat(std::vector<glm::vec3> vertices, glm::mat4 matrix)
{
	std::vector<glm::vec3> new_vertices3d = Utils::Vec4to3(Utils::Vec3to4Xmat(vertices, matrix));
	return new_vertices3d;
}

glm::vec3 Renderer::VertexXmat(glm::vec3 vertex, glm::mat4 matrix)
{
	glm::vec4 v = glm::vec4(vertex, 1);
	glm::vec3 new_vertex = Utils::Vertex4to3(v * matrix);
	return new_vertex;
}

int count = 0;
void Renderer::Render(Scene& scene)
{

	if (scene.GetModelCount())
	{
		std::shared_ptr<Camera> active_camera = scene.GetCamera(scene.GetActiveCameraIndex());


		//std::vector<glm::vec3> light_normals(/*model->GetNewNormalVertices().size()*/200);
		//for (glm::vec3 l : light_normals)
		//	l = glm::vec3(0);
		//if (scene.lights.size() > 0)
		//{
		//	for (auto light : scene.lights)
		//	{
		//		glm::mat4 lightTransformations = UpdateChangesLight(light, active_camera);
		//		UpdateVertecisByTransformations(light, lightTransformations);
		//		light_normals = Utils::CalcNormals(light->GetVertices(), light->GetFaces()); // +=
		//	}
		//}


		glm::mat4 cameraTransformations = UpdateChangesCamera(active_camera);
		DrawAxes(1000, active_camera->GetProjection() *  glm::inverse(active_camera->GetViewTransformation()));
		//RenderGrid(active_camera->GetProjection() *  glm::inverse(active_camera->GetViewTransformation()),glm::vec3(0,1,0));
		for (auto model : scene.GetModels()) {
			glm::mat4 modelTransformations = UpdateChangesModel(model, active_camera);
			if (active_camera->GetModelName() == model->GetModelName()) {
				UpdateVertecisByTransformations(active_camera, cameraTransformations);
				continue;
			}
			else {
				UpdateVertecisByTransformations(model, modelTransformations);
				DrawAxes(1000, active_camera->GetProjection() * glm::inverse(active_camera->GetViewTransformation()) * model->GetWorldTransformation() * model->GetObjectTransformation());
				if (scene.toDrawBox) {
					model->CreateCube(std::vector<glm::vec3>(model->GetVertices()), viewportWidth / 2, viewportHeight / 2, modelTransformations);
					DrawCube(model);
				}
			}
			//std::vector<glm::vec3> n = Utils::CalcNormals(model->GetNewVertices(),model->GetFaces());
			face_norm = {};
			for (auto face : model->GetFaces())
				DrawNormals(face, model->GetNewNormalVertices(), model->GetNewVertices(), model->GetVertices(), "face", scene.normal_size, false);
			for (auto face : model->GetFaces()) {
				//DrawTriangle(FromVecToTriangle(face, model->GetNewVertices()), model->GetColor());
				FillTriangle(FromVecToTriangle(face, model->GetNewVertices()), /*model->GetNormals()*/ FromVecToTriangle(face, face_norm), scene.lights, model->GetColor(), scene.draw_style);
				if (scene.toDrawNormals)
					DrawNormals(face, model->GetNewNormalVertices(), model->GetNewVertices(), model->GetVertices(), scene.draw_genre, scene.normal_size, true);
			}
		}
	}

}





void Renderer::FillTriangle(std::vector<glm::vec3>&  vertices, std::vector<glm::vec3>&  normals, std::vector<std::shared_ptr<Light>>&  lights, glm::vec3 mColor, std::string draw_style)
{

	glm::vec3 min = MeshModel::findMin({ vertices[0], vertices[1], vertices[2] });
	glm::vec3 max = MeshModel::findMax({ vertices[0], vertices[1], vertices[2] });

	glm::vec3 normal(0);
	glm::vec3 point(0);


	for (int x = min.x; x <= max.x; x++)
	{
		for (int y = min.y; y <= max.y; y++)
		{
			glm::vec3 p = glm::vec3(x, y, 0);
			glm::vec2 bcCords = BcCords2D(vertices, p);

			if (inTriangle(bcCords[0], bcCords[1]))
			{
				float lambda1 = 1 - bcCords.x - bcCords.y;
				float lambda2 = bcCords.x;
				float lambda3 = bcCords.y;

				float z_reciprocal = lambda1 * (1 / vertices[0].z) + lambda2 * (1 / vertices[1].z) + lambda3 * (1 / vertices[2].z);



				if ("flat" == draw_style)
				{
					normal = (normals[0] + vertices[0]) + (normals[1] + vertices[1]) + (normals[2] + vertices[2]);
					point = (vertices[0] + vertices[1] + vertices[2]) / glm::vec3(3);
				}
				if ("phong" == draw_style)
				{
					normal = lambda1 * (normals[0] + vertices[0]) + lambda2 * (normals[1] + vertices[1]) + lambda3 * (normals[2] + vertices[2]);
					point = lambda1 *vertices[0] + lambda2 * vertices[1] + lambda3 * vertices[2];
				}
				if ("gouraud" == draw_style)
					normal = (normals[0] + vertices[0]) + (normals[1] + vertices[1]) + (normals[2] + vertices[2]);



				//glm::vec3 point = lambda1/* * light_normals[0] + lambda2 * light_normals[1] + lambda3 * light_normals[2]*/;
				// if ortho 1/z else z

				glm::vec3 ilum_color(0);
				if (lights.size() > 0)
				{
					for (auto light : lights)
					{
						/*std::vector<glm::vec3> n = light->CalcNormals();*/
						/*glm::vec3 point = lambda1 * n[count+0] + lambda2 * n[count+1] + lambda3 * n[count+2];*/
						ilum_color += light->SetIlum(point, normal); // CalcIlum r,g,b not exeed 1.0f
						//if (ilum_color.x > 1.0f)
						//	ilum_color.x = 1.0f;
						//if (ilum_color.y > 1.0f)
						//	ilum_color.y = 1.0f;
						//if (ilum_color.z > 1.0f)
						//	ilum_color.z = 1.0f;
					}
				}
				float r = glm::min(ilum_color.x * mColor.x, 1.0f);
				float g = glm::min(ilum_color.y * mColor.y, 1.0f);
				float b = glm::min(ilum_color.z * mColor.z, 1.0f);

				//putPixel(x, y, glm::vec3(r, g, b), 1 / z_reciprocal);
				putPixel(x, y, ilum_color * mColor, 1 / z_reciprocal);
			}
		}
	}
}

glm::vec2 Renderer::BcCords2D(const std::vector<glm::vec3>& vertices, const glm::vec3& p)
{
	glm::vec2 u = vertices[1] - vertices[0];
	glm::vec2 v = vertices[2] - vertices[0];
	glm::vec2 w = p - vertices[0];

	float lambda1 = (w.y * v.x - v.y * w.x) / (u.y * v.x - u.x * v.y);
	float lambda2 = (w.y - lambda1 * u.y) / (v.y);

	return glm::vec2(lambda1, lambda2);
}
//
//float Renderer::Edge(glm::vec3 v1, glm::vec3 v2, glm::vec3 p)
//{
//	glm::vec3 v = v2 - v1;
//	glm::vec3 u = p - v1;
//	return glm::cross(v,u).z;
//}


std::vector<glm::vec3> Renderer::FromVecToTriangle(Face& face, std::vector<glm::vec3>& new_vec)
{

	int x_center = viewportWidth / 2;
	int y_center = viewportHeight / 2;
	glm::vec3 triangleIndex = Utils::FaceToVertexIndex(face);
	glm::vec3 center_shift = glm::vec3(x_center, y_center, 0);

	return { new_vec[triangleIndex.x] + center_shift, new_vec[triangleIndex.y] + center_shift, new_vec[triangleIndex.z] + center_shift };
}

glm::vec3 Renderer::CenterShift(glm::vec3& point)
{
	int x_center = viewportWidth / 2;
	int y_center = viewportHeight / 2;
	glm::vec3 center_shift = glm::vec3(x_center, y_center, 0);
	return point + center_shift;

}

void Renderer::DrawNormals(Face& face, std::vector<glm::vec3>& normals, std::vector<glm::vec3>&  vertices, std::vector<glm::vec3>&  old_vertices, std::string draw_genre, float size_normal, bool user_want_draw)
{
	int x_center = viewportWidth / 2;
	int y_center = viewportHeight / 2;
	glm::vec3 triangleIndex = Utils::FaceToVertexIndex(face);
	glm::vec3 triangleNormalIndex = Utils::FaceToNormalIndex(face);
	glm::vec3 center_shift = glm::vec3(x_center, y_center, 0);

	if (draw_genre == "face") {

		glm::vec3 start3o = (vertices[triangleIndex.x] + vertices[triangleIndex.y] + vertices[triangleIndex.z]) / glm::vec3(3);
		glm::vec3 end_n3o = glm::normalize(glm::cross(old_vertices[triangleIndex.y] - old_vertices[triangleIndex.x], old_vertices[triangleIndex.z] - old_vertices[triangleIndex.x]));
		glm::vec3 start3 = start3o;
		glm::vec3 end_n3 = start3o + end_n3o * size_normal * 30.0f;
		glm::vec4 start4 = glm::vec4(start3, 1);
		glm::vec4 end_n4 = glm::vec4(end_n3, 1);
		glm::vec3 start = Utils::Vertex4to3(start4);
		glm::vec3 end_n = Utils::Vertex4to3(end_n4);

		glm::vec4 finish = { start.x, start.y , end_n.x , end_n.y };

		finish += glm::vec4(x_center, y_center, x_center, y_center);


		if (user_want_draw)
			bresenham_line(finish.x, finish.y, finish.z, finish.w, glm::vec3(0, 1, 0));
		else
			face_norm.push_back(end_n);
	}
	else {
		glm::vec3 a_end = glm::vec3(vertices[triangleIndex.x].x + size_normal * normals[triangleNormalIndex.x].x, vertices[triangleIndex.x].y + size_normal * normals[triangleNormalIndex.x].y, 0);
		glm::vec3 b_end = glm::vec3(vertices[triangleIndex.y].x + size_normal * normals[triangleNormalIndex.y].x, vertices[triangleIndex.y].y + size_normal * normals[triangleNormalIndex.y].y, 0);
		glm::vec3 c_end = glm::vec3(vertices[triangleIndex.z].x + size_normal * normals[triangleNormalIndex.z].x, vertices[triangleIndex.z].y + size_normal * normals[triangleNormalIndex.z].y, 0);

		vertices[triangleIndex.x] += center_shift;
		vertices[triangleIndex.y] += center_shift;
		vertices[triangleIndex.z] += center_shift;
		a_end += center_shift;
		b_end += center_shift;
		c_end += center_shift;

		notFace_norm.push_back((a_end + b_end + c_end) / glm::vec3(3));


		bresenham_line(vertices[triangleIndex.x].x, vertices[triangleIndex.x].y, a_end.x, a_end.y, glm::vec3(0, 1, 0));
		bresenham_line(vertices[triangleIndex.y].x, vertices[triangleIndex.y].y, b_end.x, b_end.y, glm::vec3(0, 1, 0));
		bresenham_line(vertices[triangleIndex.z].x, vertices[triangleIndex.z].y, c_end.x, c_end.y, glm::vec3(0, 1, 0));
	}
}



void Renderer::UpdateVertecisByTransformations(std::shared_ptr<MeshModel>& model, glm::mat4& transformation)
{
	std::vector<glm::vec3> new_vec_n, new_vec;
	new_vec_n = VerticesXmat(model->GetNormals(), Utils::GetMatrix("scale", 50 * 1280.0f / 720.0f, 50 * 1280.0f / 720.0f, 1));
	new_vec = VerticesXmat(model->GetVertices(), transformation);
	model->SetNewVertices(new_vec, new_vec_n);
}

void Renderer::UpdateVertecisByTransformations(std::shared_ptr<Light>& model, glm::mat4& transformation)
{
	std::vector<glm::vec3> new_vec_n, new_vec;
	new_vec_n = VerticesXmat(model->GetNormals(), Utils::GetMatrix("scale", 50 * 1280.0f / 720.0f, 50 * 1280.0f / 720.0f, 1));
	new_vec = VerticesXmat(model->GetVertices(), transformation);
	model->SetNewVertices(new_vec, new_vec_n);
}

void Renderer::UpdateVertecisByTransformations(std::shared_ptr<Camera>& cam, glm::mat4& transformation) {
	std::vector<glm::vec3> new_vec_n, new_vec;
	new_vec_n = VerticesXmat(cam->GetNormals(), Utils::GetMatrix("scale", 50 * 1280.0f / 720.0f, 50 * 1280.0f / 720.0f, 1));
	new_vec = VerticesXmat(cam->GetVertices(), transformation);
	cam->SetNewVertices(new_vec, new_vec_n);
}


glm::mat4 Renderer::UpdateChangesCamera(std::shared_ptr<Camera>& active_camera)
{
	active_camera->SetWorldTransformation();
	active_camera->SetObjectTransformation();
	//active_camera->SetCameraLookAt();
	return Utils::GetMatrix("scale", 50 * 1280.0f / 720.0f, 50 * 1280.0f / 720.0f, 1) * active_camera->GetProjection() *  glm::inverse(active_camera->GetViewTransformation()) * active_camera->GetWorldTransformation() * active_camera->GetObjectTransformation();

}


glm::mat4 Renderer::UpdateChangesModel(std::shared_ptr<MeshModel>& model, std::shared_ptr<Camera>& active_camera)
{
	model->SetWorldTransformation();
	model->SetObjectTransformation();
	//active_camera->SetCameraLookAt();
	//glm::vec2 x = model->ScaleSize(model->GetVertices(), 1280.0f, 720.0f);
	return Utils::GetMatrix("scale", 50 * 1280.0f / 720.0f, 50 * 1280.0f / 720.0f, 1) * active_camera->GetProjection() * glm::inverse(active_camera->GetViewTransformation()) * active_camera->GetWorldTransformation() * model->GetObjectTransformation();
}

glm::mat4 Renderer::UpdateChangesLight(std::shared_ptr<Light>& model, std::shared_ptr<Camera>& active_camera)
{
	model->SetWorldTransformation();
	model->SetObjectTransformation();
	//active_camera->SetCameraLookAt();
	//glm::vec2 x = model->ScaleSize(model->GetVertices(), 1280.0f, 720.0f);
	return Utils::GetMatrix("scale", 50 * 1280.0f / 720.0f, 50 * 1280.0f / 720.0f, 1) * active_camera->GetProjection() * glm::inverse(active_camera->GetViewTransformation()) * active_camera->GetWorldTransformation() * model->GetObjectTransformation();
}

void Renderer::DrawAxes(float size, glm::mat4 matrix) {
	glm::vec3
		x_neg = VertexXmat(glm::vec3(-size, 0, 0), matrix),
		x_pos = VertexXmat(glm::vec3(size, 0, 0), matrix),
		y_neg = VertexXmat(glm::vec3(0, -size, 0), matrix),
		y_pos = VertexXmat(glm::vec3(0, size, 0), matrix),
		z_neg = VertexXmat(glm::vec3(0, 0, -size), matrix),
		z_pos = VertexXmat(glm::vec3(0, 0, size), matrix);


	bresenham_line(CenterShift(x_pos).x, CenterShift(x_pos).y, CenterShift(x_neg).x, CenterShift(x_neg).y, glm::vec3(1, 0, 0));
	bresenham_line(CenterShift(y_pos).x, CenterShift(y_pos).y, CenterShift(y_neg).x, CenterShift(y_neg).y, glm::vec3(0, 1, 0));
	bresenham_line(CenterShift(z_pos).x, CenterShift(z_pos).y, CenterShift(z_neg).x, CenterShift(z_neg).y, glm::vec3(0, 0, 1));

}

void Renderer::DrawCube(std::shared_ptr<MeshModel>& model)
{
	std::map<std::string, glm::vec3> cube = model->GetCube();

	bresenham_line(cube["ltf"].x, cube["ltf"].y, cube["rtf"].x, cube["rtf"].y, glm::vec3(1, 0, 0));
	bresenham_line(cube["lbf"].x, cube["lbf"].y, cube["rbf"].x, cube["rbf"].y, glm::vec3(1, 0, 0));
	bresenham_line(cube["ltf"].x, cube["ltf"].y, cube["lbf"].x, cube["lbf"].y, glm::vec3(1, 0, 0));
	bresenham_line(cube["rtf"].x, cube["rtf"].y, cube["rbf"].x, cube["rbf"].y, glm::vec3(1, 0, 0));

	bresenham_line(cube["rtf"].x, cube["rtf"].y, cube["rtb"].x, cube["rtb"].y, glm::vec3(1, 0, 0));
	bresenham_line(cube["ltb"].x, cube["ltb"].y, cube["rtb"].x, cube["rtb"].y, glm::vec3(1, 0, 0));
	bresenham_line(cube["ltb"].x, cube["ltb"].y, cube["ltf"].x, cube["ltf"].y, glm::vec3(1, 0, 0));

	bresenham_line(cube["rbb"].x, cube["rbb"].y, cube["rbf"].x, cube["rbf"].y, glm::vec3(1, 0, 0));
	bresenham_line(cube["rbb"].x, cube["rbb"].y, cube["rtb"].x, cube["rtb"].y, glm::vec3(1, 0, 0));
	bresenham_line(cube["rbb"].x, cube["rbb"].y, cube["lbb"].x, cube["lbb"].y, glm::vec3(1, 0, 0));

	bresenham_line(cube["lbb"].x, cube["lbb"].y, cube["lbf"].x, cube["lbf"].y, glm::vec3(1, 0, 0));
	bresenham_line(cube["lbb"].x, cube["lbb"].y, cube["ltb"].x, cube["ltb"].y, glm::vec3(1, 0, 0));

}

void Renderer::DrawTriangle(std::vector<glm::vec3> triangle, glm::vec3 color)
{
	bresenham_line(triangle[0].x, triangle[0].y, triangle[1].x, triangle[1].y, color);
	bresenham_line(triangle[0].x, triangle[0].y, triangle[2].x, triangle[2].y, color);
	bresenham_line(triangle[1].x, triangle[1].y, triangle[2].x, triangle[2].y, color);
}

void Renderer::RenderGrid(glm::mat4x4 rotateMatrix, glm::vec3& color)
{
	for (int i = -GRID_LENGTH; i <= GRID_LENGTH; i += GRID_DELTA)
	{
		glm::vec4 x_near(-1 * GRID_LENGTH, 0, i, 1);
		glm::vec4 x_far(GRID_LENGTH, 0, i, 1);
		glm::vec4 z_near(i, 0, -1 * GRID_LENGTH, 1);
		glm::vec4 z_far(i, 0, GRID_LENGTH, 1);

		DrawEdge(rotateMatrix*x_near, rotateMatrix*x_far, color);
		DrawEdge(rotateMatrix*z_near, rotateMatrix*z_far, color);
	}
}

void Renderer::DrawEdge(glm::vec4& v1, glm::vec4& v2, glm::vec3& color)
{
	bresenham_line((v1[0] + 1) * (viewportWidth / 2),
		(v1[1] + 1) * (viewportHeight / 2),
		(v2[0] + 1) * (viewportWidth / 2),
		(v2[1] + 1) * (viewportHeight / 2),
		color);
}



//##############################
//##OpenGL stuff. Don't touch.##
//##############################

// Basic tutorial on how opengl works:
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/
// don't linger here for now, we will have a few tutorials about opengl later.
void Renderer::initOpenGLRendering()
{
	// Creates a unique identifier for an opengl texture.
	glGenTextures(1, &glScreenTex);

	// Same for vertex array object (VAO). VAO is a set of buffers that describe a renderable object.
	glGenVertexArrays(1, &glScreenVtc);

	GLuint buffer;

	// Makes this VAO the current one.
	glBindVertexArray(glScreenVtc);

	// Creates a unique identifier for a buffer.
	glGenBuffers(1, &buffer);

	// (-1, 1)____(1, 1)
	//	     |\  |
	//	     | \ | <--- The exture is drawn over two triangles that stretch over the screen.
	//	     |__\|
	// (-1,-1)    (1,-1)
	const GLfloat vtc[] = {
		-1, -1,
		 1, -1,
		-1,  1,
		-1,  1,
		 1, -1,
		 1,  1
	};

	const GLfloat tex[] = {
		0,0,
		1,0,
		0,1,
		0,1,
		1,0,
		1,1
	};

	// Makes this buffer the current one.
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// This is the opengl way for doing malloc on the gpu. 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtc) + sizeof(tex), NULL, GL_STATIC_DRAW);

	// memcopy vtc to buffer[0,sizeof(vtc)-1]
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);

	// memcopy tex to buffer[sizeof(vtc),sizeof(vtc)+sizeof(tex)]
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);

	// Loads and compiles a sheder.
	GLuint program = InitShader("vshader.glsl", "fshader.glsl");

	// Make this program the current one.
	glUseProgram(program);

	// Tells the shader where to look for the vertex position data, and the data dimensions.
	GLint  vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// Same for texture coordinates data.
	GLint  vTexCoord = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)sizeof(vtc));

	//glProgramUniform1i( program, glGetUniformLocation(program, "texture"), 0 );

	// Tells the shader to use GL_TEXTURE0 as the texture id.
	glUniform1i(glGetUniformLocation(program, "texture"), 0);
}

void Renderer::createOpenGLBuffer()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, glScreenTex);

	// malloc for a texture on the gpu.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, viewportWidth, viewportHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, viewportWidth, viewportHeight);
}

void Renderer::SwapBuffers()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, glScreenTex);

	// memcopy's colorBuffer into the gpu.
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, viewportWidth, viewportHeight, GL_RGB, GL_FLOAT, colorBuffer);

	// Tells opengl to use mipmapping
	glGenerateMipmap(GL_TEXTURE_2D);

	// Make glScreenVtc current VAO
	glBindVertexArray(glScreenVtc);

	// Finally renders the data.
	glDrawArrays(GL_TRIANGLES, 0, 6);
}