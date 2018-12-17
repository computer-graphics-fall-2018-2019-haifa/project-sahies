#pragma once
#include "Scene.h"
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

/*
 * Renderer class.
 */
class Renderer
{
private:
	float *colorBuffer;
	float *zBuffer;
	int viewportWidth;
	int viewportHeight;
	int viewportX;
	int viewportY;



	void Renderer::putPixel(int i, int j, const glm::vec3& color, float z);
	void createBuffers(int viewportWidth, int viewportHeight);

	GLuint glScreenTex;
	GLuint glScreenVtc;

	void createOpenGLBuffer();
	void initOpenGLRendering();

public:
	Renderer(int viewportWidth, int viewportHeight, int viewportX = 0, int viewportY = 0);
	~Renderer();

	void Render( Scene& scene);
	void SwapBuffers();
	void ClearColorBuffer(const glm::vec3& color);
	void SetViewport(int viewportWidth, int viewportHeight, int viewportX = 0, int viewportY = 0);
	void DrawNormals(Face& face, std::vector<glm::vec3>& normals, std::vector<glm::vec3>&  vertices, std::vector<glm::vec3>&  old_vertices, std::string draw_genre, float size_normal, bool drawing);
	void Renderer::bresenham_line(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, glm::vec3& color = glm::vec3(1, 1, 1));
	void Renderer::SetTransformation(MeshModel& model, std::string genreTransformation);
	void Renderer::DrawTriangle(std::vector<glm::vec3> triangle, glm::vec3 color);
	static std::vector<glm::vec3> Renderer::VerticesXmat(std::vector<glm::vec3> vertices, glm::mat4 matrix);
	//std::vector<glm::vec3> Renderer::FromVecToTriangle(Face& face, std::vector<glm::vec3>& new_vec, std::string name);
	void Renderer::DrawCube(std::shared_ptr<MeshModel>& model);
	std::vector<glm::vec3> Renderer::FromVecToTriangle(Face& face, std::vector<glm::vec3>& new_vec);
	glm::mat4 Renderer::UpdateChangesCamera(std::shared_ptr<Camera>& active_camera);
	glm::mat4 Renderer::UpdateChangesModel(std::shared_ptr<MeshModel>& model, std::shared_ptr<Camera>& active_camera);
	void Renderer::UpdateVertecisByTransformations(std::shared_ptr<MeshModel>& model, glm::mat4& transformation);
	void Renderer::UpdateVertecisByTransformations(std::shared_ptr<Camera>& model, glm::mat4& transformation);
	 glm::vec3 Renderer::CenterShift(glm::vec3& point);
	void Renderer::DrawAxes(float size, glm::mat4 matrix);
	glm::vec3 Renderer::VertexXmat(glm::vec3 vertex, glm::mat4 matrix);
	glm::vec2 Renderer::BcCords2D(const std::vector<glm::vec3>& vertices, const glm::vec3& p);
	void Renderer::FillTriangle(std::vector<glm::vec3>&  vertices, std::vector<glm::vec3>&  normals , std::vector<std::shared_ptr<Light>>&  light, glm::vec3 mColor, std::string draw_style);
	void Renderer::FillLight(std::vector<glm::vec3>&  vertices, glm::vec3 mColor, float light_power);
	glm::mat4 Renderer::UpdateChangesLight(std::shared_ptr<Light>& model, std::shared_ptr<Camera>& active_camera);
	void Renderer::UpdateVertecisByTransformations(std::shared_ptr<Light>& model, glm::mat4& transformation);
	void Renderer::RenderGrid(glm::mat4x4 rotateMatrix, glm::vec3& color);
	void Renderer::DrawEdge(glm::vec4& v1, glm::vec4& v2, glm::vec3& color = glm::vec3(0, 0, 0));
	// Add more methods/functionality as needed...
};
