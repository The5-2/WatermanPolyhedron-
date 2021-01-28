#include "../contrib/imgui/include/imgui.h" 
#include "../contrib/imgui/include/imgui_internal.h" 
#include "../contrib/imgui/include/imgui_impl_glfw.h"
#include "../contrib/imgui/include/imgui_impl_opengl3.h"

#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <algorithm>
#include <cstdint>

#include "../include/camera.h"
#include "../include/texture.h"
#include "../include/simpleGeometryCollection.h"
#include "../include/shader.h"
#include "../include/imguiHelper.h"
#include "../include/waterman_sphere.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>

#define WIDTH 640
#define HEIGHT 640

static void error_callback(int error, const char* description){
    fprintf(stderr, "Error %d: %s\n", error, description);
}

/*************************
 * http://paulbourke.net/geometry/waterman/
 * https://github.com/antiprism/antiprism/blob/master/src/waterman.cc
 * https://books.google.de/books?id=WLAFlr1_2S4C&pg=PA175&lpg=PA175&dq=waterman+polyhedra&source=bl&ots=y5EjAFhmQR&sig=ACfU3U3KC0kZaeZvQ_0_JLsbHOmBATl7PQ&hl=en&sa=X&ved=2ahUKEwjDrtLwk6DlAhURalAKHYFIDNE4ChDoATAIegQIBxAB#v=onepage&q=waterman%20polyhedra&f=false
*************************/
/*************************
 * FCC (Face centered packing): If all spheres have a diameter of sqrt(2) -> All sphere centers on integer values
 *                              Watermansphere radius is sqrt(2 * n)
*************************/
/*************************
 * Globals
*************************/

unsigned int g_wSphere_wireframe = 0;
unsigned int g_wSphere_cullBackface = 0;

unsigned int g_wSphere_polyeder = 0;
unsigned int g_wSphere_DrawPolyeder = 0;

float g_camera_distance = 50.0f;
float g_rotation_speed = 5.0f;
int g_point_size = 5;
int g_waterman_root = 24;

Shader* testShader = nullptr;
Shader* pcShader = nullptr;

Camera* cam = nullptr;
float rotation = 0.0f;

WatermanSphere* wsClass = nullptr;
WatermanSphere* wsClassPolyeder = nullptr;

void drawWaterman(){
    if(g_wSphere_wireframe){
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    
    if(g_wSphere_cullBackface){
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }else{
        glDisable(GL_CULL_FACE);
    }

    testShader->enable();
    testShader->uniform("uUseTexture", false);
    testShader->uniform("uViewMatrix", cam->viewMatrix);
    testShader->uniform("uProjMatrix", cam->projMatrix);

    glm::mat4 modelMatrix = glm::rotate(rotation, glm::vec3(0.0f, 1.0f, 0.0f));

    testShader->uniform("uModelMatrix", modelMatrix);
    if(g_wSphere_DrawPolyeder){
        testShader->uniform("uUseColor", true);
        testShader->uniform("uColor", glm::vec3(0.0f));
        wsClass->drawLine();
        
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.999f));
        testShader->uniform("uUseColor", false);
        testShader->uniform("uModelMatrix", modelMatrix);
        wsClassPolyeder->drawTriangle();
    }else{
        wsClass->drawTriangle();
    }

    // wsClass->drawLine();
    // modelMatrix = glm::scale(modelMatrix, glm::vec3(0.98f));
    // testShader->uniform("uModelMatrix", modelMatrix);
    // wsClass->drawTriangle();

    testShader->disable();

    if(g_wSphere_wireframe){
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void drawWatermanPc(){
    glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);
    
    if(g_wSphere_cullBackface){
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }else{
        glDisable(GL_CULL_FACE);
    }

    pcShader->enable();
    pcShader->uniform("uGlPointSize", float(g_point_size));
    pcShader->uniform("uViewMatrix", cam->viewMatrix);
    pcShader->uniform("uProjMatrix", cam->projMatrix);

    glm::mat4 modelMatrix = glm::rotate(rotation, glm::vec3(0.0f, 1.0f, 0.0f));

    modelMatrix = glm::scale(modelMatrix, glm::vec3(1.02f));

    pcShader->uniform("uModelMatrix", modelMatrix);
    wsClass->drawPointCloud();
    pcShader->disable();
    
    glDisable(GL_POINT_SPRITE);
	glDisable(GL_PROGRAM_POINT_SIZE);
}

void updateRotation(){
    rotation += g_rotation_speed /10000.0f;
}

int main(void){
    std::cout << "*******************" << std::endl;
    std::cout << "*** Version 1.0 ***" << std::endl;
    std::cout << "*** Shaderpath" << std::endl;
    std::cout << "*******************" << std::endl;

    /*************************
	 * Setup opengl / buffers
	*************************/
    GLFWwindow* window;
    GLuint vertex_buffer;
    GLint vpos_location, vcol_location;
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit()){
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Simple example", NULL, NULL);
    if (!window){
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
        exit(EXIT_FAILURE);
	};

    glfwSwapInterval(1);

	/*************************
	 * Imgui
	*************************/
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);

    testShader = new Shader("resources/shaders/test.vert.glsl", 
                            "resources/shaders/test.frag.glsl");
	
    pcShader = new Shader("resources/shaders/pointcloud.vert.glsl",
                          "resources/shaders/pointcloud.frag.glsl");

    wsClass = new WatermanSphere(float(g_waterman_root));
    wsClass->init();
    wsClass->uploadTriangle();
    wsClass->uploadPointCloud();
    // wsClass->uploadLine();

    cam = new Camera();
	cam->setProjectionMatrix(70.0f, WIDTH / HEIGHT, 1.0f, 500.0f);
    
	/*************************
	 * Init OpenGl-Stuff
	*************************/
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

	/*************************
	 * Renderloop
	*************************/
	while (!glfwWindowShouldClose(window)){   
        /*************************
	 	* Clear
		*************************/
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        /*************************
	 	* Set camera
		*************************/
        cam->setViewMatrix(glm::vec3(g_camera_distance), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        /*************************
	 	* Draw OpenGl
		*************************/
        drawWaterman();
        drawWatermanPc();
        updateRotation();

        /*************************
	 	* Imgui overlay
		*************************/
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

        /*Imgui-Demo*/
        // ImGui::ShowDemoWindow();
        
        /*Texture preview*/
        ImGui::Begin("Waterman Polyhedron");
        hardwareSpecs();
        extensions();

        ImGui::Separator();
        ImGui::SliderFloat("Cam distance", &g_camera_distance, 0.0f, 500.0f); 
        ImGui::SliderFloat("Rotation speed", &g_rotation_speed, 0.0f, 50.0f); 
        ImGui::SliderInt("Point size", &g_point_size, 0, 25);
        ImGui::CheckboxFlags("Wireframe", &g_wSphere_wireframe, 1);
        ImGui::CheckboxFlags("Backfacecull", &g_wSphere_cullBackface, 1);
        ImGui::Separator();        
        ImGui::SliderInt("Waterman root", &g_waterman_root, 0, 1000);
        ImGui::CheckboxFlags("Polyeder", &g_wSphere_polyeder, 1);
        if (ImGui::Button("Create new")){
            wsClass = new WatermanSphere(float(g_waterman_root));
            wsClass->init();

            wsClassPolyeder = new WatermanSphere(float(g_waterman_root));
            wsClassPolyeder->init();

            if(g_wSphere_polyeder){
                wsClass->uploadLine();
                wsClassPolyeder->uploadTriangle();
            }else{
                wsClass->uploadTriangle();
            }

            wsClass->uploadPointCloud();
            g_wSphere_DrawPolyeder = g_wSphere_polyeder;
            std::cout << "Create new" << std::endl;
        }

        ImGui::End();
        ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

	/*************************
	 * Exit, cleanup
	*************************/
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}