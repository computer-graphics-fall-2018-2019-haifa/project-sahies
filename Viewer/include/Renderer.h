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


	void putPixel(int x, int y, const glm::vec3& color);
	void createBuffers(int viewportWidth, int viewportHeight);

	GLuint glScreenTex;
	GLuint glScreenVtc;

	void createOpenGLBuffer();
	void initOpenGLRendering();

public:
	Renderer(int viewportWidth, int viewportHeight, int viewportX = 0, int viewportY = 0);
	~Renderer();

	void Render(const Scene& scene);
	void SwapBuffers();
	void ClearColorBuffer(const glm::vec3& color);
	void SetViewport(int viewportWidth, int viewportHeight, int viewportX = 0, int viewportY = 0);

<<<<<<< HEAD
	void Renderer::bresenham_line(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, const glm::vec3& color);
	void Renderer::Transform(MeshModel& model);
	void Renderer::DrawTriangle(MeshModel& model);
=======
	void Renderer::bresenham_line(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
	void Renderer::SetTransformation(MeshModel& model);
	void Renderer::DrawTriangle(std::vector<glm::vec3> triangle);
	std::vector<glm::vec3> Renderer::VerticesXmat(std::vector<glm::vec3> vertices, glm::mat4 matrix);
	std::vector<glm::vec3> Renderer::FromVecToTriangle(Face& face, std::vector<glm::vec3>& new_vec);
>>>>>>> 6eafb69db38614fcb94b865b4543400e6465d216

	// Add more methods/functionality as needed...
};
