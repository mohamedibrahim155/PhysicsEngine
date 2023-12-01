#include"ApplicationRenderer.h"



ApplicationRenderer::ApplicationRenderer()
{
}

ApplicationRenderer::~ApplicationRenderer()
{
}



void ApplicationRenderer::WindowInitialize(int width, int height,  std::string windowName)
{
    windowWidth = width;
    WindowHeight = height;
    lastX = windowWidth / 2.0f;
    lastY= WindowHeight / 2.0f;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(width, height, windowName.c_str(), NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* w, int x, int y)
        {
            static_cast<ApplicationRenderer*>(glfwGetWindowUserPointer(w))->SetViewPort(w, x, y);
        });

    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            static_cast<ApplicationRenderer*>(glfwGetWindowUserPointer(window))->KeyCallBack(window, key, scancode, action, mods);
        });


    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xposIn, double yposIn)
        {
            static_cast<ApplicationRenderer*>(glfwGetWindowUserPointer(window))->MouseCallBack(window, xposIn, yposIn);
        });

    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset)
        {
            static_cast<ApplicationRenderer*>(glfwGetWindowUserPointer(window))->MouseScroll(window, xoffset, yoffset);
        });
   
    
    //Init GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }




  
    defaultShader = new Shader("Shaders/Light_VertexShader.vert", "Shaders/Light_FragmentShader.frag");
    lightShader = new Shader("Shaders/lighting.vert", "Shaders/lighting.frag");
    StencilShader = new Shader("Shaders/StencilOutline.vert", "Shaders/StencilOutline.frag");
   
    SkyboxShader = new Shader("Shaders/SkyboxShader.vert", "Shaders/SkyboxShader.frag");


    //ScrollShader = new Shader("Shaders/ScrollTexture.vert", "Shaders/ScrollTexture.frag");
    render.AssignStencilShader(StencilShader);
    camera.Position = glm::vec3(0, 0, - 1.0f);
}



void ApplicationRenderer::Start()
{
   // GLCALL(glEnable(GL_DEPTH_TEST));
    GLCALL(glDepthFunc(GL_LESS));
    GLCALL(glEnable(GL_STENCIL_TEST));
    GLCALL(glStencilFunc(GL_NOTEQUAL, 1, 0xFF));
    GLCALL(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    skybox = new Skybox(); 
    
    skybox->AssignSkyboxShader(SkyboxShader);
    skybox->SkyboxPrerender();
    

    render.AssignCamera(&camera);

    Model* Sphere = new Model((char*)"Models/DefaultSphere/Sphere_1_unit_Radius.ply", true);
   // Model* Sphere = new Model();

    Model* scroll = new Model((char*)"Models/Scroll/scroll.obj", true);
    scroll->transform.position.y -= 5;
    scroll->meshes[0]->TextureScrolling(true);


     Model* Pokeball = new Model((char*)"Models/Pokeball/pokeball.obj", true);
     Model* Pokeball2 = new Model((char*)"Models/Pokeball/pokeball.obj", true);


     Model* Grass = new Model((char*)"Models/Grass/Grass.obj", true,false, true);
    
     Model* Window = new Model();
     Window->alphaMask = new Texture();
     
     Window->alphaMask->LoadTexture("Models/Window/WindowAlphaMask.png", "material.alphaMask");
     Window->loadModel((char*)"Models/Window/Window.obj");
     Window->isTransparant = true;
     Window->isCutOut = false;

     Model* Window2 = new Model();
     Window2->alphaMask = new Texture();
     Window2->isTransparant = true;
     Window2->isCutOut = false;
     Window2->alphaMask->LoadTexture("Models/Window/WindowAlphaMask.png", "material.alphaMask");
     Window2->loadModel((char*)"Models/Window/Window.obj");
   
   
     


     Sphere->transform.position.x += 2;
     Pokeball->transform.position.x -= 2;
    
     Grass->transform.position.y += 5;
     Window->transform.position.y += 8;
     Window2->transform.position.y += 6;

     Pokeball2->transform.position.x -= 5;
     Pokeball2->transform.position.y -= 0.3f;
     Pokeball2->transform.SetScale(glm::vec3(1.2f));
    Pokeball2->transform.position = Pokeball->transform.position;
         Pokeball2->transform.SetScale(glm::vec3(0.5f));

     Model* dir = new Model();
    // Model* spotlight = new Model(*Sphere);
     //spotlight->transform.SetPosition(glm::vec3(-2.0f, 0.0f, -3.0f));

     Light directionLight;
     directionLight.lightType = LightType::DIRECTION_LIGHT;
     directionLight.lightModel = dir;
     directionLight.ambient =  glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
     directionLight.diffuse =  glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
     directionLight.specular = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);


    /* Light spot;
     spot.lightType = LightType::SPOT_LIGHT;
     spot.lightModel = spotlight;
     spot.ambient =  glm::vec4(0.7f, 0.7f, 0.7f,1.0f);
     spot.diffuse =  glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
     spot.specular = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);*/

     //Mesh Renderer
     render.AddModelsAndShader(Sphere, defaultShader);
     render.AddModelsAndShader(scroll, defaultShader);

     render.AddModelsAndShader(Grass, defaultShader);
    

     render.AddModelsAndShader(Pokeball, defaultShader);
  

     
     render.selectedModel = Sphere;

     render.AddModelsAndShader(dir,lightShader);
    // render.AddModelsAndShader(spotlight, lightShader);

     render.AddTransparentModels(Window, defaultShader);
     render.AddTransparentModels(Window2, defaultShader);

     //LightRenderer
     lightManager.AddNewLight(directionLight);
   //  lightManager.AddNewLight(spot);
     lightManager.SetUniforms(defaultShader->ID);
   //  PhysicsObject* SpherePhyiscs = new PhysicsObject(Sphere);
   //  SpherePhyiscs->Initialize(false, true, DYNAMIC);

   //  PhysicsEngine.AddPhysicsObjects(SpherePhyiscs);

     defaultShader->Bind();
     defaultShader->setInt("skybox", 0);
}

void ApplicationRenderer::PreRender()
{

}

void ApplicationRenderer::Render()
{
    Start();
    Material material(128.0f);
   // glEnable(GL_BLEND);
  //  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    while (!glfwWindowShouldClose(window))
    {
        Clear();


        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        scrollTime += deltaTime;

        ProcessInput(window);


        glm::mat4 _projection = glm::perspective(glm::radians(camera.Zoom), (float)windowWidth / (float)WindowHeight, 0.1f, 100.0f);
        glm::mat4 _view = camera.GetViewMatrix();
        glm::mat4 _skyboxview = glm::mat4(glm::mat3(camera.GetViewMatrix()));


        PreRender(); //Update call BEFORE  DRAW

        glDepthFunc(GL_LEQUAL);
        SkyboxShader->Bind();
        SkyboxShader->setMat4("view", _skyboxview);
        SkyboxShader->setMat4("projection", _projection);

        skybox->Skyboxrender();
        glDepthFunc(GL_LESS); 


        defaultShader->Bind();
       // material.SetMaterialProperties(*defaultShader);
     //   lightManager.UpdateUniformValuesToShader(defaultShader);
        lightManager.UpdateUniformValues(defaultShader->ID);
       

         defaultShader->setMat4("projection", _projection);
         defaultShader->setMat4("view", _view);
         defaultShader->setVec3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);
         defaultShader->setFloat("time", scrollTime);
         defaultShader->setBool("isDepthBuffer", false);

         lightShader->Bind();
         lightShader->setVec3("objectColor", glm::vec3(1, 1, 1));
         lightShader->setMat4("projection", _projection);
         lightShader->setMat4("view", _view);

         StencilShader->Bind();
         StencilShader->setMat4("projection", _projection);
         StencilShader->setMat4("view", _view);

        /* ScrollShader->Bind();
         ScrollShader->setMat4("ProjectionMatrix", _projection);*/
        

       

         
  
         
         // make models that it should not write in the stencil buffer
         render.Draw();





         PostRender(); // Update Call AFTER  DRAW

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
}

void ApplicationRenderer::PostRender()
{
   // glDisable(GL_BLEND);

 //   PhysicsEngine.UpdatePhysics(deltaTime);
}

void ApplicationRenderer::Clear()
{
    GLCALL(glClearColor(0.1f, 0.1f, 0.1f, 0.1f));
    GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
}

void ApplicationRenderer::ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed=25;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, deltaTime * cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime * cameraSpeed);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime * cameraSpeed);

    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime * cameraSpeed);

    }


}

 void ApplicationRenderer::SetViewPort(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

 void ApplicationRenderer::KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
 {  
         if (key == GLFW_KEY_V && action == GLFW_PRESS)
         {

            
             std::cout << "V pressed" << std::endl;

             std::vector<Model*> listOfModels = render.GetModelList();
            
          

             selectedModelCount++;

             if (selectedModelCount > listOfModels.size()-1)
             {
                 selectedModelCount = 0;
             }

            
             render.selectedModel = listOfModels[selectedModelCount];


         }
     
 }

 void ApplicationRenderer::MouseCallBack(GLFWwindow* window, double xposIn, double yposIn)
 {

    float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);
     
        if (firstMouse)
        {

        }

         if (firstMouse)
         {
             lastX = xpos;
             lastY = ypos;
             firstMouse = false;
         }
     
         float xoffset = xpos - lastX;
         float yoffset = lastY - ypos;
     
         lastX = xpos;
         lastY = ypos;
     
         if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
         {
             camera.ProcessMouseMovement(xoffset, yoffset);
         }
 }

 void ApplicationRenderer::MouseScroll(GLFWwindow* window, double xoffset, double yoffset)
 {
     camera.ProcessMouseScroll(static_cast<float>(yoffset));
 }
