#define _USE_MATH_DEFINES

#include "Renderer.h"
#include "InitShader.h"
#include "MeshModel.h"
#include "Utils.h"
#include <imgui/imgui.h>
#include <vector>
#include <cmath>
#include "ImguiMenus.h"



#define sign(x) ((x > 0) ? 1 : ((x < 0) ? -1 : 0))
// TODO ^
#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)

Renderer::Renderer(int viewportWidth, int viewportHeight, int viewportX, int viewportY) :
	colorBuffer(nullptr),
	zBuffer(nullptr)
{
	initOpenGLRendering();
	SetViewport(viewportWidth, viewportHeight, viewportX, viewportY);
}


void Renderer::bresenham_line(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) {
	// Bresenham's line algorithm
	const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));

	if (steep)
	{
		std::swap(x1, y1);
		std::swap(x2, y2);
	}

	if (x1 > x2)
	{
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	const float dx = x2 - x1;
	const float dy = fabs(y2 - y1);

	float error = dx / 2.0f;
	const int ystep = (y1 < y2) ? 1 : -1;
	int y = (int)y1;

	const int maxX = (int)x2;

	for (int x = (int)x1; x < maxX; x++)
	{
		if (steep)
		{
			putPixel(y, x, glm::vec3(0,0,0));
		}
		else
		{
			putPixel(x, y, glm::vec3(0, 0, 0));
		}

		error -= dy;
		if (error < 0)
		{
			y += ystep;
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
}


void Renderer::putPixel(int i, int j, const glm::vec3& color)
{
	if (i < 0) return; if (i >= viewportWidth) return;
	if (j < 0) return; if (j >= viewportHeight) return;
	colorBuffer[INDEX(viewportWidth, i, j, 0)] = color.x;
	colorBuffer[INDEX(viewportWidth, i, j, 1)] = color.y;
	colorBuffer[INDEX(viewportWidth, i, j, 2)] = color.z;
}


void Renderer::createBuffers(int viewportWidth, int viewportHeight)
{
	if (colorBuffer)
	{
		delete[] colorBuffer;
	}

	colorBuffer = new float[3* viewportWidth * viewportHeight];
	for (int x = 0; x < viewportWidth; x++)
	{
		for (int y = 0; y < viewportHeight; y++)
		{
			putPixel(x, y, glm::vec3(0.0f, 0.0f, 0.0f));
		}
	}
}

void Renderer::ClearColorBuffer(const glm::vec3& color)
{
	for (int i = 0; i < viewportWidth; i++)
	{
		for (int j = 0; j < viewportHeight; j++)
		{
			putPixel(i, j, color);
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


void Renderer::Render(const Scene& scene)
{
	int x_center = viewportWidth / 2;
	int y_center = viewportHeight / 2;

	// X-line Y-line
	bresenham_line(0, y_center, viewportWidth, y_center);
	bresenham_line(x_center, 0, x_center, viewportHeight);

	if (scene.GetModelCount()) {  // maybe > 1?

		Camera active_camera = scene.GetCamera(scene.GetActiveCameraIndex());
		active_camera.SetCamTransformation();
		active_camera.SetWorldTransformation();

		for (auto model : scene.GetModels()) {

			//if (active_camera.GetModelName() == model->GetModelName())
			//	continue;

			model->SetObjectTransformation();
			model->SetWorldTransformation();
			std::vector<glm::vec3> new_vec_n = VerticesXmat(model->GetNormals(),  model->GetObjectTransformation());
			std::vector<glm::vec3> new_vec = VerticesXmat(model->GetVertices(), model->GetObjectTransformation());
			//std::vector<glm::vec3> new_vec = VerticesXmat(model->GetVertices(), glm::inverse(active_camera.GetViewTransformation()) * model->GetWorldTransformation() * model->GetObjectTransformation());
			//DrawCube(model);

			for (auto face : model->GetFaces()) {
				DrawTriangle(FromVecToTriangle(face, new_vec));
				DrawNormals(face, new_vec_n, new_vec);
			}

		}
	}
}


void Renderer::DrawCube(std::shared_ptr<MeshModel>& model)
{
	std::map<std::string, glm::vec3> cube =model->GetCube();
	bresenham_line(cube["fbl"].x, cube["fbl"].y, cube["fbr"].x, cube["fbr"].y);
	bresenham_line(cube["fbl"].x, cube["fbl"].y, cube["ftr"].x, cube["ftr"].y);
	bresenham_line(cube["ftr"].x, cube["ftr"].y, cube["ftl"].x, cube["ftl"].y);
	bresenham_line(cube["ftl"].x, cube["ftl"].y, cube["fbl"].x, cube["fbl"].y);

	bresenham_line(cube["bbl"].x, cube["bbl"].y, cube["bbr"].x, cube["bbr"].y);
	bresenham_line(cube["bbr"].x, cube["bbr"].y, cube["btr"].x, cube["btr"].y);
	bresenham_line(cube["btr"].x, cube["btr"].y, cube["btl"].x, cube["btl"].y);
	bresenham_line(cube["btl"].x, cube["btl"].y, cube["bbl"].x, cube["bbl"].y);

	bresenham_line(cube["fbl"].x, cube["fbl"].y, cube["bbl"].x, cube["bbl"].y);
	bresenham_line(cube["fbr"].x, cube["fbr"].y, cube["bbr"].x, cube["bbr"].y);
	bresenham_line(cube["ftl"].x, cube["ftl"].y, cube["btl"].x, cube["btl"].y);
	bresenham_line(cube["ftr"].x, cube["ftr"].y, cube["btr"].x, cube["btr"].y);
}

void Renderer::DrawTriangle(std::vector<glm::vec3> triangle)
	{
		bresenham_line(triangle[0].x, triangle[0].y, triangle[1].x, triangle[1].y);
		bresenham_line(triangle[0].x, triangle[0].y, triangle[2].x, triangle[2].y);
		bresenham_line(triangle[1].x, triangle[1].y, triangle[2].x, triangle[2].y);
	}

//std::vector<glm::vec3> Renderer::FromVecToPoint(Face& face, std::vector<glm::vec3>& new_vec, std::string name)
//{
//	
//	int x_center = viewportWidth / 2;
//	int y_center = viewportHeight / 2;
//	int a = face.GetVertexIndex(0, name) - 1;
//	int b = face.GetVertexIndex(1, name) - 1;
//	int c = face.GetVertexIndex(2, name) - 1;
//
//
//	new_vec[a] += glm::normalize(normal);
//	new_vec[b] += glm::normalize(normal);
//	new_vec[c] += glm::normalize(normal);
//
//	return { new_vec[a], new_vec[b] ,new_vec[c] };
//
//}


std::vector<glm::vec3> Renderer::FromVecToTriangle(Face& face, std::vector<glm::vec3>& new_vec)
{
	
	int x_center = viewportWidth / 2;
	int y_center = viewportHeight / 2;
	int a = face.GetVertexIndex(0) - 1;
	int b = face.GetVertexIndex(1) - 1;
	int c = face.GetVertexIndex(2) - 1;

	glm::vec3 center_shift = glm::vec3(x_center, y_center, 0);

	return { new_vec[a] + center_shift, new_vec[b] + center_shift, new_vec[c] + center_shift };
	//return { new_vec[a], new_vec[b] , new_vec[c] };
}

void Renderer::DrawNormals(Face face, std::vector<glm::vec3> normals, std::vector<glm::vec3>  vertices)
{
		int x_center = viewportWidth / 2;
		int y_center = viewportHeight / 2;
		int a = face.GetVertexIndex(0) - 1;
		int b = face.GetVertexIndex(1) - 1;
		int c = face.GetVertexIndex(2) - 1;
		int a_n = face.GetNormalIndex(0) - 1;
		int b_n = face.GetNormalIndex(1) - 1;
		int c_n = face.GetNormalIndex(2) - 1;
		glm::vec3 center_shift = glm::vec3(x_center, y_center, 0);

		vertices[a] += center_shift;
		vertices[b] += center_shift;
		vertices[c] += center_shift;
		normals[a_n] += center_shift;
		normals[b_n] += center_shift;
		normals[c_n] += center_shift;

		bresenham_line(vertices[a].x, vertices[a].y, normals[a_n].x, normals[a_n].y);
		bresenham_line(vertices[b].x, vertices[b].y, normals[b_n].x, normals[b_n].y);
		bresenham_line(vertices[c].x, vertices[c].y, normals[c_n].x, normals[c_n].y);
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
	const GLfloat vtc[]={
		-1, -1,
		 1, -1,
		-1,  1,
		-1,  1,
		 1, -1,
		 1,  1
	};

	const GLfloat tex[]={
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtc)+sizeof(tex), NULL, GL_STATIC_DRAW);

	// memcopy vtc to buffer[0,sizeof(vtc)-1]
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);

	// memcopy tex to buffer[sizeof(vtc),sizeof(vtc)+sizeof(tex)]
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);

	// Loads and compiles a sheder.
	GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );

	// Make this program the current one.
	glUseProgram(program);

	// Tells the shader where to look for the vertex position data, and the data dimensions.
	GLint  vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition,2,GL_FLOAT,GL_FALSE,0,0 );

	// Same for texture coordinates data.
	GLint  vTexCoord = glGetAttribLocation( program, "vTexCoord" );
	glEnableVertexAttribArray( vTexCoord );
	glVertexAttribPointer( vTexCoord,2,GL_FLOAT,GL_FALSE,0,(GLvoid *)sizeof(vtc) );

	//glProgramUniform1i( program, glGetUniformLocation(program, "texture"), 0 );

	// Tells the shader to use GL_TEXTURE0 as the texture id.
	glUniform1i(glGetUniformLocation(program, "texture"),0);
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