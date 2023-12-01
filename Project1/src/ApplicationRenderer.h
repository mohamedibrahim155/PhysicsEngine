#pragma once
#include "Shader.h"
#include "Camera.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Renderer.h"
#include "VertexArray.h"
#include  "model.h"
#include "material.h"
#include "Light.h"
#include "Transform.h"
#include"LightManager.h"
#include "Random.h"
#include "PhysicsEngine.h"
#include "Skybox.h"

#include "GraphicsRender.h"



class ApplicationRenderer
{
public:
	ApplicationRenderer();
	~ApplicationRenderer();

	static void SetViewPort(GLFWwindow* window, int width, int height);
	void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
	void MouseCallBack(GLFWwindow* window, double xposIn, double yposIn);
	void MouseScroll(GLFWwindow* window, double xoffset, double yoffset);
	
	void WindowInitialize(int width, int height,  std::string windowName ="Window");
	
	void Start();
	void PreRender();
	void Render();
	void PostRender();
	void Clear();

	void ProcessInput(GLFWwindow* window);
	



private:
	GLFWwindow* window;
	Camera camera;

	GraphicsRender render;
	LightManager lightManager;

	Shader* defaultShader;
	Shader* lightShader;
	Shader* StencilShader;
	Shader* SkyboxShader;

	Skybox* skybox;

	PhysicsEngine PhysicsEngine;

	 int windowWidth;
	 int  WindowHeight;
	 int selectedModelCount = 0;

	 float lastX;
	 float lastY;
	 float deltaTime;
	 double lastTime;
	 float lastFrame;

	 float scrollTime;

	 bool firstMouse;

};

