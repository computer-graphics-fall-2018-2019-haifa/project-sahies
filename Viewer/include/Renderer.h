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

	void Renderer::bresenham_line(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
	void Renderer::Transform(MeshModel& model,std::string& name);
	void Renderer::DrawTriangle(glm::vec3& a, glm::vec3& b, glm::vec3& c);
	std::vector<glm::vec3> Renderer::TransformMUL(MeshModel& model, glm::mat4 matrix);

	// Add more methods/functionality as needed...
};
