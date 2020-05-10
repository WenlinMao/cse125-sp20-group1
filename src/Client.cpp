//
//  Client.cpp
//  Gaia
//
//  Created by Cain on 4/16/20.
//  Copyright © 2020 SphereEnix. All rights reserved.
//

#include "Client.h"
namespace pt = boost::property_tree;

Sphere* Client::sphere_player1;
Sphere* Client::sphere_player2;
Sphere* Client::sphere_mouse; // testing only
Terrain* Client::terrain;
Skybox* Client::skybox;

Camera* Client::camera;
glm::vec3 Client::sphere1_pos = glm::vec3(0.0f);
glm::vec3 Client::sphere2_pos = glm::vec3(0.0f);
glm::vec2 Client::mousePos = glm::vec2(INFINITY, INFINITY);
bool Client::mouseControl = false;
int Client::isMouseButtonDown = 0;
glm::vec2 Client::clickPos = glm::vec2(INFINITY, INFINITY);
glm::vec2 Client::releasePos = glm::vec2(INFINITY, INFINITY);
IO_handler* Client::io_handler;

Client::Client(int width, int height) {
    window = new Window(width, height, "Window");
    std::pair<int, int> windowSize = window->getFrameBufferSize();
    this->width = windowSize.first;
    this->height = windowSize.second;
    //camera = new Camera(glm::vec3(75, 10, -75), glm::vec3(30, 5, -30));
    camera = new Camera(glm::vec3(60, 59, 21), glm::vec3(60, 5, -30));
    projection = glm::perspective(glm::radians(60.0), double(width) / (double)height, 1.0, 1000.0);

    // Print OpenGL and GLSL versions.
    printVersions();
    // Setup OpenGL settings.
    setupOpenglSettings();

    setupCallbacks();
}

Client::~Client() {
    // Deallcoate the objects.
    delete sphere_player1;
    delete sphere_player2;
    delete sphere_mouse;
    delete terrain;
    delete camera;


    // Delete the shader program.
    glDeleteProgram(shaderProgram);

    delete window;
}

bool Client::initializeProgram() {
    // Create a shader program with a vertex shader and a fragment shader.
    shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");
    skyboxProgram = LoadShaders("shaders/skybox.vert", "shaders/skybox.frag");
    terrainProgram = LoadShaders("shaders/terrain.vert", "shaders/terrain.frag");
    
    // Check the shader program.
    if (!shaderProgram)
    {
        std::cerr << "Failed to initialize shader program" << std::endl;
        return false;
    }
    
    // Check the shader program.
    if (!skyboxProgram)
    {
        std::cerr << "Failed to initialize skybox program" << std::endl;
        return false;
    }


    // Create io_handler (0 for balls)
    io_handler = new IO_handler(0);
    
    return true;
}

bool Client::initializeObjects()
{
    vector<std::string> faces =
    {
        "textures/sky-right.png",
        "textures/sky-left.png",
        "textures/sky-top.png",
        "textures/sky-bottom.png",
        "textures/sky-back.png",
        "textures/sky-front.png",
    };
    skybox = new Skybox(faces);
    sphere_player1 = new Sphere(5.0f, 2.0f);
    sphere_player2 = new Sphere(5.0f, 2.0f);
    // testing only
    sphere_mouse = new Sphere(1.0f, 2.0f);
    
    terrain = new Terrain(251, 251, 0.5f);
    std::vector<glm::vec2> tmp = {
        glm::vec2(1.0f, 1.0f),
        glm::vec2(125.0f, 125.0f),
        glm::vec2(135.0f, 125.0f),
        glm::vec2(250.0f, 250.0f)
    };
    terrain->edit(tmp, 10);
    
    //terrain->setHeightsFromTexture("textures/terrain-heightmap-01.png",0.0f, 12.0f);
    //terrain->terrainBuildMesh();
    return true;
}

void Client::idleCallback() {
  
}

void Client::displayCallback() {
    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the objects
    sphere_player1->draw(camera->getView(), projection, shaderProgram);
    sphere_player2->draw(camera->getView(), projection, shaderProgram);
    terrain->draw(camera->getView(), projection, terrainProgram);
    skybox->draw(camera->getView(), projection, skyboxProgram);
    sphere_mouse->draw(camera->getView(), projection, shaderProgram);

}

bool Client::initialize() {
    return initializeProgram() && initializeObjects();
}

void Client::setupOpenglSettings()
{

    // Set the viewport size.
    glViewport(0, 0, width, height);

    // Enable depth buffering.
    glEnable(GL_DEPTH_TEST);

    // Related to shaders and z value comparisons for the depth buffer.
    glDepthFunc(GL_LEQUAL);

    // Set polygon drawing mode to fill front and back of each polygon.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Set clear color to black.
    glClearColor(0.0, 0.0, 0.0, 0.0);
}

void Client::printVersions()
{
  // Get info of GPU and supported OpenGL version.
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
  std::cout << "OpenGL version supported: " << glGetString(GL_VERSION)
    << std::endl;

  //If the shading language symbol is defined.
#ifdef GL_SHADING_LANGUAGE_VERSION
  std::cout << "Supported GLSL version is: " <<
  glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
#endif
}

void Client::run() {
    if (!initialize())
    throw "Unable to initialize client";

    // Client Try
    try
    {
        boost::asio::io_service io_service;
        tcp::endpoint endpoint(ip::address::from_string("127.0.0.1"),8888);

        chat_client c(io_service, endpoint);

        boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

        std::string msg;

        // Loop while GLFW window should stay open.
        while (!glfwWindowShouldClose(window->getWindow()))
        {
            // Main render display callback. Rendering of objects is done here. (Draw)
            displayCallback();
            window->displayCallback();
            //camera = new Camera(glm::vec3(60, 59, 21), glm::vec3(60, 5, -30));

            // Sphere player and Terrian player Camera Logic
            if(c.get_id() == 1){
                camera->setPos(glm::vec3(sphere1_pos.x, sphere1_pos.y + 10,sphere1_pos.z+15));
                camera->setLookAt(glm::vec3(sphere1_pos.x, sphere1_pos.y,sphere1_pos.z));
            }
            else if(c.get_id() == 2){
                camera->setPos(glm::vec3(sphere2_pos.x, sphere2_pos.y + 10,sphere2_pos.z+15));
                camera->setLookAt(glm::vec3(sphere2_pos.x, sphere2_pos.y,sphere2_pos.z));
            }
            else{
                // camera for terrian player is fixed
            }
            
            //cout << camera->getLookAtPos().x << " " << camera->getLookAtPos().y << " " << camera->getLookAtPos().z << endl;
            //cout << camera->getPos().x << " " << camera->getPos().y << " " << camera->getPos().z << endl;

            // Idle callback. Updating objects, etc. can be done here. (Update)
            idleCallback();
            io_handler -> SendPackage(&c);
            updateFromServer(c.getMsg());
        }


        c.close();
        t.join();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}

void Client::errorCallback(int error, const char* description)
{
    // Print error.
    std::cerr << description << std::endl;
}

void Client::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

    /*
      * TODO: Modify below to add your key callbacks.
      */

      // Check for a key press.
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
            case GLFW_KEY_ESCAPE:{
                // Close the window. This causes the program to also terminate.
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            }
            // take user's io
            case GLFW_KEY_W:{
                io_handler->SendKeyBoardInput(0);
                break;
            }
            case GLFW_KEY_A:{
                io_handler->SendKeyBoardInput(1);
                break;
            }
            case GLFW_KEY_S:{
                io_handler->SendKeyBoardInput(2);
                break;
            }
            case GLFW_KEY_D:{
                io_handler->SendKeyBoardInput(3);
                break;
            }
            default:
                break;
        }
    }

    // Check for holding key
    else if (action == GLFW_REPEAT)
    {
        switch (key)
        {
            // Contineous movement
            case GLFW_KEY_W:{
                io_handler->SendKeyBoardInput(0);
                break;
            }
            case GLFW_KEY_A:{
                io_handler->SendKeyBoardInput(1);
                break;
            }
            case GLFW_KEY_S:{
                io_handler->SendKeyBoardInput(2);
                break;
            }
            case GLFW_KEY_D:{
                io_handler->SendKeyBoardInput(3);
                break;
            }
            default:
                break;
        }
    }
}

void Client::setupCallbacks()
{
    // Set the error callback.
    glfwSetErrorCallback(errorCallback);

    // Set the key callback.
    glfwSetKeyCallback(window->getWindow(), keyCallback);
  
    // glfwSetInputMode(window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED); // hide cursor
    glfwSetCursorPosCallback(window->getWindow(), cursorPositionCallback);
    
    glfwSetMouseButtonCallback(window->getWindow(), setMouseButtonCallback);
}


void Client::setMouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
    double xpos, ypos;
    //getting cursor position
    glfwGetCursorPos(window, &xpos, &ypos);
    
    if(action == GLFW_PRESS){
        // Check to see if we need to set click start position
        if(clickPos.x == INFINITY && clickPos.y == INFINITY){
            clickPos = glm::vec2((float)xpos, (float)ypos);
        }
        
        if(button==GLFW_MOUSE_BUTTON_RIGHT){
            //cout << "RIGHT!!" << endl;
            isMouseButtonDown = 2;
        }
        else if(button==GLFW_MOUSE_BUTTON_LEFT){
            //cout << "LEFT!!" << endl;
            isMouseButtonDown = 1;
        }
        else{
            cout << "unknown click?" << endl;
        }
            
//        if(isMouseButtonDown > 0){
//            string leftOrRight = "left";
//            if(isMouseButtonDown == 2){
//                leftOrRight = "right";
//            }
//            cout << leftOrRight << " click on: " << xpos << ", " << ypos << endl;
//        }
    }
    else if (action == GLFW_RELEASE){
        releasePos = glm::vec2((float)xpos, (float)ypos);
        //cout << "drag start: " << clickPos[0] << ", " << clickPos[1] << endl;
        //cout << "drag end: " << releasePos[0] << ", " << releasePos[1] << endl;
        
        // send i/o to server
        //io_handler->SendMouseInput(isMouseButtonDown, clickPos, releasePos);

        glm::vec2 translatedCPos = screenPointToWorld(clickPos);
        glm::vec2 translatedRPos = screenPointToWorld(releasePos);
        io_handler->SendMouseInput(isMouseButtonDown, translatedCPos, translatedRPos);

        cout << "finalPos x: " << translatedRPos.x << " finalPos y: " << translatedRPos.y << endl;
        isMouseButtonDown = 0;
        clickPos = glm::vec2(INFINITY, INFINITY);
        releasePos = glm::vec2(INFINITY, INFINITY);
    }
}

glm::vec2 Client::screenPointToWorld(glm::vec2 mousePos){
    glm::vec3 w;
    glm::vec3 u;
    glm::vec3 v;
    float fov = glm::radians(60.0f);
    float a, b, t;
    float wWidth = 640.0f;
    float wHeight = 480.0f;
    glm::vec3 rayDir;
    glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 finalPos;
    
    w = glm::normalize(camera->getPos() - camera->getLookAtPos());
    u = glm::normalize(glm::cross(camera->getUpVector(), w));
    v = glm::cross(w,u);
    
    // Finally worked version
    a = glm::tan(fov/2) * (wWidth /wHeight) * ((mousePos.x - (float)wWidth/2) / (wWidth/2));
    b = glm::tan(fov/2) * (((wHeight/2) - mousePos.y)/ (wHeight/2));
    
    rayDir = glm::normalize(a*u + b*v - w);
    
    t = glm::dot((glm::vec3(0.0f) - camera->getPos()), normal)/glm::dot(rayDir, normal);
    
    finalPos = camera->getPos()+t * rayDir;
    
    //cout << "finalPos x: " << finalPos.x << " finalPos y: " << finalPos.y << " finalPos z: " << finalPos.z << endl;
    
    return glm::vec2(finalPos.x, finalPos.z);
}


void Client::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    
    glm::mat4 mtx = glm::mat4(1.0f);
    glm::vec2 translatedMPos = screenPointToWorld(glm::vec2(xpos, ypos));
    if(translatedMPos.x >= 0 && translatedMPos.x <= 125 && translatedMPos.y <= 0 && translatedMPos.y >= -125){
        mtx[3] = glm::vec4(translatedMPos.x,-1,translatedMPos.y,1);
        sphere_mouse->move(mtx);
    }
    
    if (mousePos.x  == INFINITY || mousePos.y == INFINITY) {
        mousePos.x = xpos;
        mousePos.y = ypos;
        
        return;
    }
    
    
//    if(isMouseButtonDown > 0){
//        string leftOrRight = "left";
//        if(isMouseButtonDown == 2){
//            leftOrRight = "right";
//        }
//        cout << leftOrRight << " click on: " << xpos << ", " << ypos << endl;
//    }


    if (mouseControl) {
        float xoffset = xpos - mousePos.x;
        float yoffset = ypos - mousePos.y;
        mousePos.x = xpos;
        mousePos.y = ypos;
        camera->rotateAround(xoffset, yoffset);
    }
}


void Client::updateFromServer(string msg) {
    try{
        if(msg != ""){
            stringstream ss;
            ss << msg;
            
            pt::ptree tar;
            pt::read_json(ss, tar);

            glm::mat4 matrix1, matrix2;
            
            vector <float> height_map;
            
            int id = 1;
            BOOST_FOREACH(const pt::ptree::value_type& child,
                          tar.get_child("Obj")) {
                
                if (id == 1){
                    int i=0;
                    BOOST_FOREACH(const pt::ptree::value_type& m,
                                  child.second.get_child("transformation")) {
                
                        matrix1[i/4][i%4] = stof(m.second.data());
                        i++;
                        //cout << matrix1[i/4][i%4] << endl;
                    }
                    
                    // Store the absolute position
                    sphere1_pos = glm::vec3(matrix1[3][0], matrix1[3][1], matrix1[3][2]);
                    sphere_player1->move(matrix1);
                }
                else if(id == 2){
                    int i=0;
                    BOOST_FOREACH(const pt::ptree::value_type& m,
                                  child.second.get_child("transformation")) {
                        matrix2[i/4][i%4] = stof(m.second.data());
                        i++;
                    }
                    // Store the absolute position
                    sphere2_pos = glm::vec3(matrix2[3][0], matrix2[3][1], matrix2[3][2]);
                    sphere_player2->move(matrix2);
                }
                else{
                    // id 3, 4 for terrian
                }
                id++;
            }
            int i=0;
            BOOST_FOREACH(const pt::ptree::value_type& v,
            tar.get_child("height_map")) {
                height_map.push_back(stof(v.second.data()));
                i++;
            }
            
            //build mesh based on height map from server
            //TODO: different signature
            //setHeightsFromTexture(height_map, offset, scale);

        }
    } catch (...){
        
    }
    
}
