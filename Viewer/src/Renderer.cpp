#define _USE_MATH_DEFINES

#include "Renderer.h"
#include "InitShader.h"
#include "MeshModel.h"
#include "Utils.h"
#include <imgui/imgui.h>
#include <vector>
#include <cmath>
#include "ImguiMenus.h"
#include <regex>

#define inTriangle(a,b) (a >= 0 && b >= 0 && (a + b <= 1)) ? 1:0
#define sign(x) ((x > 0) ? 1 : ((x < 0) ? -1 : 0))
#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)
#define Z_INDEX(w,i,j) ((i)+(j)*(w))
#define INFINITE 100000
#define GRID_LENGTH 1000
#define GRID_DELTA 50


glm::vec3 gl_eye;
std::vector<glm::vec3> n;
bool fog;
float z_near;
float z_far;
glm::vec3 fog_color;
std::shared_ptr<MeshModel> mmodel;
float z_center;
bool isPerspective;

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


void Renderer::putPixel(int i, int j, const glm::vec3& color, float z, bool t)
{
	int w = GetActiveViewportWidth();
	int h = GetActiveViewportHeight();
	if (i < 0) return; if (i >= w) return;
	if (j < 0) return; if (j >= h) return;
	if (!t || z < zBuffer[Z_INDEX(w, i, j)])
	
	{
		colorBuffer[INDEX(w, i, j, 0)] = color.x;
		colorBuffer[INDEX(w, i, j, 1)] = color.y;
		colorBuffer[INDEX(w, i, j, 2)] = color.z;
		zBuffer[Z_INDEX(w, i, j)] = z;
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
			putPixel(x, y, glm::vec3(0.0f, 0.0f, 0.0f), INFINITE);
		}
	}
}

int Renderer::GetActiveViewportWidth()
{
	return alias ? aliasViewportWidth : viewportWidth;
}

int Renderer::GetActiveViewportHeight()
{
	return alias ? aliasViewportHeight : viewportHeight;
}

void Renderer::ClearColorBuffer(const glm::vec3& color)
{
	int w = GetActiveViewportWidth();
	int h = GetActiveViewportHeight();
	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			putPixel(i, j, color, INFINITE - 1, false);
			zBuffer[Z_INDEX(w, i, j)] = INFINITE -1;

		}
	}
}


void Renderer::SetViewport(int viewportWidth, int viewportHeight, int viewportX, int viewportY)
{
	this->viewportX = viewportX;
	this->viewportY = viewportY;
	this->viewportWidth = viewportWidth;
	this->viewportHeight = viewportHeight;

	if (alias) {
		this->aliasViewportWidth = this->viewportWidth * 2;
		this->aliasViewportHeight = this->viewportHeight * 2;
		createBuffers(this->aliasViewportWidth, this->aliasViewportHeight);
	}
	else 
	{
		createBuffers(this->viewportWidth, this->viewportHeight);
	}
	createOpenGLBuffer();

}

void Renderer::SetTransformation(MeshModel& model, std::string genreTransformation)
{
	std::string transform = model.GetTransform();
	glm::vec3 cordinates = model.GetCordinates(transform);
	glm::mat4 matrix = Utils::GetMatrix(transform, cordinates.x, cordinates.y, cordinates.z);
	model.setMatrix(matrix, transform, genreTransformation);
}


glm::vec3 Renderer::VertexXmat(glm::vec3 vertex, glm::mat4 matrix)
{
	glm::vec4 v = glm::vec4(vertex, 1);
	glm::vec3 new_vertex = Utils::Vertex4to3(v * matrix);
	return new_vertex;
}


void Renderer::SetViewPort()
{
	SetViewport(this->viewportWidth, this->viewportHeight);
}

void Renderer::Render(Scene& scene)
{
	std::shared_ptr<Camera> active_camera = scene.GetCamera(scene.GetActiveCameraIndex());
	gl_eye = active_camera->eye;
	fog = scene.fog;
	z_near = active_camera->zNear;
	z_far = active_camera->zFar;
	fog_color = scene.fog_color;
	isPerspective = scene.isPerspective;
	z_center = (z_far - z_near) / 2;

	if (scene.lights.size() > 0)
	{
		for (auto light : scene.lights)
		{
			light->UpdateChangesModel(active_camera->GetProjection(), glm::inverse(active_camera->GetViewTransformation()), active_camera->GetWorldTransformation());
			if (light->source == "parallel")
				continue;
			for (auto face : light->GetFaces())
				FillLight(FromVecToTriangle(face, light->GetNewVertices()), light->GetColor(), light->light_power);
		}
	}


	if (scene.GetModelCount())
	{

		active_camera->UpdateChangesModel(active_camera->GetProjection(), glm::inverse(active_camera->GetViewTransformation()), active_camera->GetWorldTransformation());
		DrawAxes(1000, /*active_camera->GetProjection() **/  glm::inverse(active_camera->GetViewTransformation()));

		for (auto model : scene.GetModels()) {
			if (active_camera->GetModelName() == model->GetModelName()) {
				active_camera->UpdateChangesModel(active_camera->GetProjection(), glm::inverse(active_camera->GetViewTransformation()), active_camera->GetWorldTransformation());
				continue;
			}
			else 
			{
				model->UpdateChangesModel(active_camera->GetProjection(), glm::inverse(active_camera->GetViewTransformation()), active_camera->GetWorldTransformation());
				//DrawAxes(5000, model->model_transform_without_projection);
				if (scene.toDrawBox) {
					model->CreateCube(std::vector<glm::vec3>(model->GetVertices()), viewportWidth / 2, viewportHeight / 2, model->model_transform_without_projection);
					DrawCube(model);
				}
			}
			//n = Utils::CalcNormals(model->GetNewVertices(),model->GetFaces());

			for (auto face : model->GetFaces()) {
				//DrawTriangle(FromVecToTriangle(face, model->newVerticesRender), model->GetColor());
				FillTriangle(face, model->newVerticesRender, model->GetNormals()/* model->GetNewNormalVertices()*/, scene.lights, model, scene.draw_style);
				if (scene.toDrawNormals)
					DrawNormals(face, model->GetNormals(), model->newVerticesRender, model->GetVertices(), scene.draw_genre, scene.normal_size, true);
			}
		}
	}

}



void Renderer::FillLight(std::vector<glm::vec3>&  vertices,  glm::vec3 mColor, float light_power)
{

	glm::vec3 min = MeshModel::findMin({ vertices[0], vertices[1], vertices[2] });
	glm::vec3 max = MeshModel::findMax({ vertices[0], vertices[1], vertices[2] });

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
				putPixel(x, y, light_power * mColor, 1 / z_reciprocal);
			}
		}
	}


}

void Renderer::FillTriangle(Face& face, std::vector<glm::vec3>&  all_vertices, std::vector<glm::vec3>&  normals, std::vector<std::shared_ptr<Light>>&  lights, std::shared_ptr<MeshModel>& model, std::string draw_style)
{
	std::vector<glm::vec3> vertices = FromVecToTriangle(face, all_vertices);
	//std::vector<glm::vec3> nor = FromNormalToTriangle(face, normals);

	glm::vec3 min = MeshModel::findMin(vertices);
	glm::vec3 max = MeshModel::findMax(vertices);

	std::vector<glm::vec3> face_normal = Utils::CalcNorm(face, normals, all_vertices, "face", 0.5, GetActiveViewportWidth() , GetActiveViewportHeight(), z_center); // {point, normal, end}
	if (isPerspective)
		face_normal = Utils::CalcNorm(face, normals, model->newVertices, "face", 0.5, GetActiveViewportWidth(), GetActiveViewportHeight(), z_center); // {point, normal, end}
	std::vector<glm::vec3>  vertices_normal = Utils::CalcNorm(face, normals, all_vertices, "vertex", 1, GetActiveViewportWidth(), GetActiveViewportHeight(), z_center); // TODO a problem!!!!! normals <=> all_vertices

	glm::vec3 point(0), normal(0);

	//flat normal
	normal = face_normal[1];


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
				float x_reciprocal = lambda1 * (1 / vertices[0].x) + lambda2 * (1 / vertices[1].x) + lambda3 * (1 / vertices[2].x);
				float y_reciprocal = lambda1 * (1 / vertices[0].y) + lambda2 * (1 / vertices[1].y) + lambda3 * (1 / vertices[2].y);

				if (!isPerspective) {
					 z_reciprocal = lambda1 * (vertices[0].z) + lambda2 * (vertices[1].z) + lambda3 * (vertices[2].z);
					 x_reciprocal = lambda1 * (vertices[0].x) + lambda2 * (vertices[1].x) + lambda3 * (vertices[2].x);
					 y_reciprocal = lambda1 * (vertices[0].y) + lambda2 * (vertices[1].y) + lambda3 * (vertices[2].y);
				}
		

				point = glm::vec3(x_reciprocal, y_reciprocal, z_reciprocal);

				glm::vec3 ilum_color(0);
				glm::vec3 phongColor(0);
				if ("gouraud" == draw_style)
						normal = lambda1 * (vertices_normal[0]) + lambda2 * (vertices_normal[1]) + lambda3 *  (vertices_normal[2]);

				if ("phong" == draw_style)
				{

					std::vector<glm::vec3> colorPhong;
					std::vector<float> lam = { lambda1,lambda2 ,lambda3 };
					if (lights.size() > 0)
					{
						for (int i = 0; i < 3; i++)
						{
							for (auto light : lights)
							{
								if (light->source == "ambient")
								{
									ilum_color += model->ambient * light->color;
									continue;
								}

								colorPhong.push_back(lam[i] * light->SetIlum(vertices[i], vertices_normal[i], gl_eye, draw_style, model, isPerspective));
							}
						}
						phongColor = colorPhong[0] + colorPhong[1] + colorPhong[2];
					}
				

				}

					glm::vec3 ambient(0);
					
					glm::vec3 final_color = model->color;

					if (model->marble) 
						if (model->uni_type == 0)
							final_color *= glm::mix(model->color1, model->color2, lambda1*lambda1 /*glm::clamp(glm::vec3(lambda1, 0.0f, 1.0f), 0.0f, 1.0f))*/);
						else if (model->uni_type == 1)
							final_color *= glm::mix(model->color1, model->color2, lambda1*lambda1 + lambda2 * lambda2 + lambda3 * lambda3);
						else 
							final_color *= glm::vec3(static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
					
					if (lights.size() > 0)
					{
						for (auto light : lights)
						{
							if (light->source == "ambient")
							{
								ambient = model->ambient * light->color;
								continue;
							}

							ilum_color += light->SetIlum(point, normal, gl_eye, draw_style, model, isPerspective);

						}
						ilum_color += ambient;

						if ("phong" == draw_style)
							ilum_color = phongColor;

						if (!fog)
						{


							float r = glm::min(ilum_color.x * final_color.x, 1.0f);
							float g = glm::min(ilum_color.y * final_color.y, 1.0f);
							float b = glm::min(ilum_color.z * final_color.z, 1.0f);

							putPixel(x, y, glm::vec3(r, g, b), 1 / z_reciprocal);
						}
						else
							putPixel(x, y, Light::setFog(ilum_color, fog_color, /*1 /*/ z_reciprocal, z_near, z_far) * model->color, 1 / z_reciprocal);
					}
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
	if (alias)
	{
		x_center *= 2;
		y_center *= 2;
	}
	glm::vec3 triangleIndex = Utils::FaceToVertexIndex(face);
	glm::vec3 center_shift = glm::vec3(x_center, y_center, /*z_center*/0 );

	return { new_vec[triangleIndex.x] + center_shift, new_vec[triangleIndex.y] + center_shift, new_vec[triangleIndex.z] + center_shift };
}

std::vector<glm::vec3> Renderer::FromNormalToTriangle(Face& face, std::vector<glm::vec3>& new_vec)
{

	int x_center = viewportWidth / 2;
	int y_center = viewportHeight / 2;
	if (alias)
	{
		x_center *= 2;
		y_center *= 2;
	}
	glm::vec3 triangleIndex = Utils::FaceToNormalIndex(face);
	glm::vec3 center_shift = glm::vec3(x_center, y_center, 0);

	return { new_vec[triangleIndex.x]/* + center_shift*/, new_vec[triangleIndex.y]/* + center_shift*/, new_vec[triangleIndex.z]/* + center_shift */};
}

glm::vec3 Renderer::CenterShift(glm::vec3& point)
{
	int x_center = viewportWidth / 2;
	int y_center = viewportHeight / 2;
	if (alias)
	{
		x_center *= 2;
		y_center *= 2;
	}
	glm::vec3 center_shift = glm::vec3(x_center, y_center, /*z_center*/0);
	return point + center_shift;

}

void Renderer::DrawNormals(Face& face, std::vector<glm::vec3>& normals, std::vector<glm::vec3>&  vertices, std::vector<glm::vec3>&  old_vertices, std::string draw_genre, float size_normal, bool user_want_draw)
{
	int x_center = viewportWidth / 2;
	int y_center = viewportHeight / 2;
	if (alias)
	{
		x_center *= 2;
		y_center *= 2;
	}
	glm::vec3 center_shift = glm::vec3(x_center, y_center, /*z_center*/ 0);

	int a = face.GetVertexIndex(0) - 1;
	int b = face.GetVertexIndex(1) - 1;
	int c = face.GetVertexIndex(2) - 1;

	int a_n = face.GetNormalIndex(0) - 1;
	int b_n = face.GetNormalIndex(1) - 1;
	int c_n = face.GetNormalIndex(2) - 1;

	

	if (draw_genre == "face") {

		glm::vec3 start = (vertices[a] + vertices[b] + vertices[c]) / glm::vec3(3),
				  normal = glm::normalize(glm::cross(vertices[b] - vertices[a], vertices[c] - vertices[a])),
				  end = start + normal * size_normal * 50.0f;

		start = Utils::Vertex4to3(glm::vec4(start, 1));
		end = Utils::Vertex4to3(glm::vec4(end, 1));

		glm::vec2 start_center_shift = glm::vec2(start.x, start.y) + glm::vec2(x_center, y_center);
		glm::vec2 end_center_shift = glm::vec2( end.x, end.y) + glm::vec2(x_center, y_center);

		bresenham_line(start_center_shift.x, start_center_shift.y, end_center_shift.x, end_center_shift.y, glm::vec3(0, 1, 0));

	}
	else
	{
		glm::vec3 a_end = glm::vec3(vertices[a].x + size_normal * normals[a_n].x, vertices[a].y + size_normal * normals[a_n].y, vertices[a].z + size_normal * normals[a_n].z);
		glm::vec3 b_end = glm::vec3(vertices[b].x + size_normal * normals[b_n].x, vertices[b].y + size_normal * normals[b_n].y, vertices[b].z + size_normal * normals[b_n].z);
		glm::vec3 c_end = glm::vec3(vertices[c].x + size_normal * normals[c_n].x, vertices[c].y + size_normal * normals[c_n].y, vertices[c].z + size_normal * normals[c_n].z);

		std::vector<glm::vec3> start = {vertices[a]+ center_shift ,vertices[b] + center_shift ,vertices[c] + center_shift };

		a_end += center_shift;
		b_end += center_shift;
		c_end += center_shift;

		bresenham_line(start[0].x, start[0].y, a_end.x, a_end.y, glm::vec3(0, 1, 0));
		bresenham_line(start[1].x, start[1].y, b_end.x, b_end.y, glm::vec3(0, 1, 0));
		bresenham_line(start[2].x, start[2].y, c_end.x, c_end.y, glm::vec3(0, 1, 0));
	}
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
	if (alias)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, viewportWidth/2, viewportHeight/2, 0, GL_RGB, GL_FLOAT, NULL);
		glViewport(0, 0, viewportWidth/2, viewportHeight/2);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, viewportWidth, viewportHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, viewportWidth, viewportHeight);
}

void Renderer::SwapBuffers()
{

	float* tempBuffer = colorBuffer;

	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, glScreenTex);

	if (alias) {
		tempBuffer = new float[3 * viewportWidth * viewportHeight];
		//InitZBuffer();
		int idx;
		for (int x = 0; x < viewportWidth; x++) {
			for (int y = 0; y < viewportHeight; y++) {
				int nw = INDEX(aliasViewportWidth, 2 * x, 2 * y, 0),
					ne = INDEX(aliasViewportWidth, 2 * x + 1, 2 * y, 0),
					sw = INDEX(aliasViewportWidth, 2 * x, 2 * y + 1, 0),
					se = INDEX(aliasViewportWidth , 2 * x + 1, 2 * y + 1, 0);
				tempBuffer[INDEX(viewportWidth, x, y, 0)] = (colorBuffer[nw + 0] + colorBuffer[ne + 0] + colorBuffer[sw + 0] + colorBuffer[se + 0]) / 4;
				tempBuffer[INDEX(viewportWidth, x, y, 1)] = (colorBuffer[nw + 1] + colorBuffer[ne + 1] + colorBuffer[sw + 1] + colorBuffer[se + 1]) / 4;
				tempBuffer[INDEX(viewportWidth, x, y, 2)] = (colorBuffer[nw + 2] + colorBuffer[ne + 2] + colorBuffer[sw + 2] + colorBuffer[se + 2]) / 4;
			}
		}
	}

	// memcopy's colorBuffer into the gpu.
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, viewportWidth, viewportHeight, GL_RGB, GL_FLOAT, tempBuffer);

	if (alias) {
		delete[] tempBuffer;
	}

	// Tells opengl to use mipmapping
	glGenerateMipmap(GL_TEXTURE_2D);

	// Make glScreenVtc current VAO
	glBindVertexArray(glScreenVtc);

	// Finally renders the data.
	glDrawArrays(GL_TRIANGLES, 0, 6);
}