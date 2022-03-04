/**
 * @file main.cpp
 * @author Murillo Moraes Martins
 * @brief this is a chaotic equation particle simulator. This project was inspired by the simulator 
 * made by CodeParade show in this video (https://www.youtube.com/watch?v=fDSIRXmnVvk&ab_channel=CodeParade).
 * @version 0.1
 * @date 2022-03-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "./includes/shader.h"
#include "./includes/particle.h"    
#include "./includes/text.h"

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>

float TimeScale = 0.000001f;
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;
bool isPaused = false;
float t = 0.0f;

double param[18];
std::string terms[9] = {"xx", "xy", "xt", "yt", "yy", "tt", "x", "y", "t"};

std::string xString;
std::string yString;

GLFWwindow *InitializeOpenGL();
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void randomParameters();
void createEquationText();
void saveEquation();
void loadEquation();
void createNewRandom();
void newEquation();
void printHelp();

int main(int argc, char **argv){
    //initializing window and OpenGL
    GLFWwindow *window = InitializeOpenGL();
    if(window == NULL){
        std::cout << "Failed to Initialize OpenGL" << std::endl;
        return -1;
    }
    
    //compile and setup the text shader
    Shader textShader("./shaders/text.vs", "./shaders/text.fs");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    textShader.use();
    textShader.setMat4("projection", projection);
    //initializing freetype
    FreeTypeInit();

    //creating particles and setting parameters
    srand(time(NULL));
    int partAmount = 1000;
    Particle particles(partAmount);
    for(int i = 0; i < partAmount; i++){
        particles.setSize(i, 1.0f);
        float red = ((float)rand())/((float)RAND_MAX);
        float green = ((float)rand())/((float)RAND_MAX);
        float blue = ((float)rand())/((float)RAND_MAX);
        particles.setColor(i, glm::vec4(red, green, blue, 1.0f));
        particles.setTrailSize(i, 100);
    }
    randomParameters();
    createEquationText();

    printHelp();
    
    //render loop
    while(!glfwWindowShouldClose(window)){
        //processing inputs
        processInput(window);

        //setting background color and clearing buffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //updating time or rendering pause text
        if(!isPaused) t += TimeScale;
        else RenderText(textShader, "Paused", SCR_WIDTH - 95.0f, SCR_HEIGHT - 25.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));

        //creating time/scale text
        std::string timeString = "time: ";
        timeString.append(std::to_string(t));
        std::string scaleString = "scale: ";
        scaleString.append(std::to_string(TimeScale));

        float x = t;
        float y = t;
        //here comes the chaos
        for(int i = 0; i < partAmount; i++){
            float xx = x * x;
            float xt = x * t;
            float xy = x * y;
            float yt = y * t;
            float yy = y * y;
            float tt = t * t;

            float nx, ny;
            nx = (param[0]*xx) + (param[1]*xy) + (param[2]*xt) + (param[3]*yt) + (param[4]*yy) + (param[5]*tt) + (param[6]*x) + (param[7]*y) + (param[8]*t);
            ny = (param[9]*xx) + (param[10]*xy) + (param[11]*xt) + (param[12]*yt) + (param[13]*yy) + (param[14]*tt) + (param[15]*x) + (param[16]*y) + (param[17]*t);

            particles.setPosition(i, glm::vec3(nx*0.5f, ny*0.5f, 0.0f));
            x = nx;
            y = ny;
        }
        particles.drawAllParticles();

        //writing text
        RenderText(textShader, timeString, 10.0f, 10.0f, 0.3f, glm::vec3(1.0f, 1.0f, 1.0f));
        RenderText(textShader, scaleString, 10.0f, 30.0f, 0.3f, glm::vec3(1.0f, 1.0f, 1.0f));
        RenderText(textShader, xString, 10.0f, SCR_HEIGHT - 20.0f, 0.3f, glm::vec3(1.0f, 1.0f, 1.0f));
        RenderText(textShader, yString, 10.0f, SCR_HEIGHT - 40.0f, 0.3f, glm::vec3(1.0f, 1.0f, 1.0f));
        
        //updating window
        glfwSwapBuffers(window);
        //polling events
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    system("clear");
    return(0);
}

/**
 * @brief Initializes GLFW and GLAD. Also creates a OpenGL window and returns it.
 * 
 * @return GLFWwindow* 
 */
GLFWwindow *InitializeOpenGL(){
    //initializing GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //creating window
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Particle Simulator", NULL, NULL);
    if(window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //initializing GLAD
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return NULL;
    }

    return window;
}

/**
 * @brief resizes screen when screen is manually resized by user
 * 
 * @param window 
 * @param width 
 * @param height 
 */
void framebuffer_size_callback(GLFWwindow *window, int width, int height){
    glViewport(0, 0, width, height);
}

/**
 * @brief processing user input
 * 
 * @param window current window
 */
void processInput(GLFWwindow *window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //time scale inputs
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        TimeScale += 0.000001f;
    if(TimeScale > 0.000002f && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        TimeScale -= 0.000001f;
    if(TimeScale > 0.0001 && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        TimeScale -= 0.0001;
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        TimeScale += 0.0001;

    //pausing inputs
    if(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        isPaused = true;
    if(glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        isPaused = false;

    //reset input
    if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        t = 0.0f;

    //saving/loading inputs
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        saveEquation();
    if(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        loadEquation();

    //new equation input
    if(glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        newEquation();

    //help input
    if(glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
        printHelp();
}

/**
 * @brief creates random equation parameters
 * 
 */
void randomParameters(){
    srand(time(NULL));
    for(int i = 0; i < 18; i++){
        int choice = rand() % 3;
        switch(choice){
            case 0: param[i] =  1.0f; break;
            case 1: param[i] =  0.0f; break;
            case 2: param[i] = -1.0f; break;
        }
    }
}

/**
 * @brief creates the text that displays the equations on the screen
 * 
 */
void createEquationText(){
    //creating string for the x position
    xString = "x' = ";
    for(int i = 0; i < 9; i++){
        if(param[i] != 0.0f){
            if(param[i] >= 0.0f && i != 0) xString.append(" + ");
            if(param[i] < 0.0f && i != 0) xString.append(" - ");
            if(param[i] < 0.0f && i == 0) xString.append("- ");
            xString.append(terms[i]);
        }
    }

    //creating string for the y position
    yString = "y' = ";
    for(int i = 9; i < 18; i++){
        if(param[i] != 0.0f){
            if(param[i] > 0.0f && i != 0) yString.append(" + ");
            if(param[i] < 0.0f && i != 0) yString.append(" - ");
            if(param[i] < 0.0f && i == 0) yString.append("- ");
            yString.append(terms[i - 9]);
        }
    }
}

/**
 * @brief saves the coeficients of a equation to a file with a name selected by the user
 * 
 */
void saveEquation(){
    system("clear");

    //getting filename
    std::cout << "Input your filename: " << std::endl;
    std::string filename;
    std::cin >> filename;
    //creating filepath
    std::string filepath = "./equations/";
    filepath.append(filename);

    //checking if file already exists
    std::ifstream exist;
    exist.open(filepath);
    if(exist.is_open()){
        std::cout << "This file already exists, do you wish to override it? (Y/N)" << std::endl;
        char answer;
        std::cin >> answer;
        exist.close();

        //checking if user wants to override the existing file
        if(answer == 'N'){
            system("clear");
            return;
        }
    }

    //saving data to the file
    std::ofstream myfile;
    myfile.open(filepath);
    for(int i = 0; i < 18; i++){
        myfile << param[i] << std::endl;
    }
    myfile.close();
    system("clear");
    std::cout << "File " << filename << " Saved" << std::endl;
}

/**
 * @brief loading equation coeficients from a file
 * 
 */
void loadEquation(){
    system("clear");
    
    //getting filename
    std::cout << "Input your filename: " << std::endl;
    std::string filename;
    std::cin >> filename;
    //creating filepath
    std::string filepath = "./equations/";
    filepath.append(filename);

    //checking if file exists
    std::ifstream myfile;
    myfile.open(filepath);
    if(!myfile.is_open()){
        std::cout << "Invalid filename" << std::endl;
        return;
    }

    //loading params
    for(int i = 0; i < 18; i++){
        std::string test;
        myfile >> test;
        param[i] = std::stoi(test);
    }
    myfile.close();

    //reseting particles
    createEquationText();
    t = 0.0f;

    system("clear");
    std::cout << "File " << filename << " Loaded" << std::endl;
}

/**
 * @brief resets the time and creates new equations
 * 
 */
void newEquation(){
    randomParameters();
    createEquationText();
    t = 0.0f;
}

/**
 * @brief prints helpful information in the terminal
 * 
 */
void printHelp(){
    system("clear");
    std::cout << "////////////////////////// Chaotic Equation Simulator //////////////////////////" << std::endl;
    std::cout << "////////////////////////////////// Commands ////////////////////////////////////" << std::endl << std::endl;
    std::cout << "Time Scale Commands:" << std::endl;
    std::cout << "The bigger the time scale, the faster the particles move." << std::endl;
    std::cout << "\t↑ - increase the time scale by a little." << std::endl;
    std::cout << "\t↓ - decrease the time scale by a little." << std::endl;
    std::cout << "\t→ - increase the time scale by a lot." << std::endl;
    std::cout << "\t← - decrease the time scale by a lot." << std::endl << std::endl;
    std::cout << "Simulation Commands:" << std::endl;
    std::cout << "\tP - pauses the simulation." << std::endl;
    std::cout << "\tO - unpauses the simulation." << std::endl;
    std::cout << "\tR - resets the time." << std::endl;
    std::cout << "\tN - resets time and creates new equations (overrinding the current one)." << std::endl << std::endl;
    std::cout << "Saving/Loading Commands:" << std::endl;
    std::cout << "These operations are done in the terminal." << std::endl;
    std::cout << "The equation files are saved/loaded from the \"equations\" folder." << std::endl;
    std::cout << "\tS - saves the current equation." << std::endl;
    std::cout << "\tL - loads a equation and overrides the current one." << std::endl << std::endl;
    std::cout << "\tH - prints this information in the terminal." << std::endl;
    std::cout << "To close the program you can either press the ESC key or close the window." << std::endl;
    std::cout << "That's it, have fun :)." << std::endl;
}