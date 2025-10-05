// main.cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <utility>
#include <cmath>

#include "MoonMaker.h"
#include "Sphere.h"
#include "Gravity.h"
#include "roche.h"

// -------------------- Shader Sources --------------------
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 color;
void main() {
    FragColor = vec4(color, 1.0);
}
)";

// -------------------- Shader Compile --------------------
GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Shader compilation error (type " << type << "):\n" << infoLog << std::endl;
    }
    return shader;
}

bool checkProgramLinkStatus(GLuint prog) {
    GLint success;
    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(prog, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Program link error:\n" << infoLog << std::endl;
        return false;
    }
    return true;
}

// -------------------- Camera --------------------
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f, pitch = 0.0f;
float lastX = 400, lastY = 300;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// -------------------- Callbacks --------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if(firstMouse){
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
        return;
    }
    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;
    lastX = (float)xpos;
    lastY = (float)ypos;

    float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f) pitch = 89.0f;
    if(pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void processInput(GLFWwindow* window){
    float cameraSpeed = 5.0f * deltaTime;
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraPos += cameraSpeed * cameraFront;
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraPos -= cameraSpeed * cameraFront;
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp))*cameraSpeed;
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp))*cameraSpeed;
}

// -------------------- Main --------------------
int main(){
    if(!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(800,600,"Roche Limit Simulator",nullptr,nullptr);
    if(!window){glfwTerminate(); return -1;}
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
    glfwSetCursorPosCallback(window,mouse_callback);
    glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr << "Failed to init GLAD\n";
        return -1;
    }

    // Compile shaders and link program
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram,vertexShader);
    glAttachShader(shaderProgram,fragmentShader);
    glLinkProgram(shaderProgram);
    if(!checkProgramLinkStatus(shaderProgram)) return -1;
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // ---------------- Initial bodies ----------------
    // Get user input for simulation parameters
    float planetMass, planetRadius, moonMass, moonRadius;
    float moonDistance, moonVelocityY, moonVelocityZ;

    std::cout << "Enter planet mass: ";
    std::cin >> planetMass;

    std::cout << "Enter planet radius: ";
    std::cin >> planetRadius;

    std::cout << "Enter moon mass: ";
    std::cin >> moonMass;

    std::cout << "Enter moon radius: ";
    std::cin >> moonRadius;

    std::cout << "Enter moon distance from planet: ";
    std::cin >> moonDistance;

    std::cout << "Enter moon velocity (Y component): ";
    std::cin >> moonVelocityY;

    std::cout << "Enter moon velocity (Z component): ";
    std::cin >> moonVelocityZ;

    // Display orbital velocity recommendation
    float orbitalVelocity = sqrt(0.001f * planetMass / moonDistance);
    std::cout << "\nRecommended orbital velocity for stable orbit: " << orbitalVelocity << std::endl;
    std::cout << "Your velocity magnitude: " << sqrt(moonVelocityY*moonVelocityY + moonVelocityZ*moonVelocityZ) << "\n" << std::endl;

    bool passed_roche_limit = false;
    bool fragment_initialized = false;

    Sphere planetSphere(planetRadius, 36, 18);
    Sphere moonSphere(moonRadius, 36, 18);
    Sphere fragmentSphere(0.05f, 12, 12);

    Body planet(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f), planetMass);
    Body moon(glm::vec3(moonDistance,0.0f,0.0f), glm::vec3(0.0f, moonVelocityY, moonVelocityZ), moonMass);
    std::vector<Body> fragments;

    glEnable(GL_DEPTH_TEST);

    // ---------------- Render Loop ----------------
    while(!glfwWindowShouldClose(window)){
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Cap deltaTime to prevent numerical instability
        if(deltaTime > 0.016f) deltaTime = 0.016f; // Max ~60 FPS

        processInput(window);

        glClearColor(0.05f,0.05f,0.1f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos+cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f),800.0f/600.0f,0.1f,100.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"view"),1,GL_FALSE,glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"projection"),1,GL_FALSE,glm::value_ptr(projection));

        // ---------------- Update positions ----------------
        if(!passed_roche_limit)
            passed_roche_limit = update_roche_status(planet, moon, planetRadius, moonRadius);

        if(passed_roche_limit && !fragment_initialized){
            auto centers_and_masses = parallel_calculate_centres_and_mass_serial(
                {moon.position.x, moon.position.y, moon.position.z}, moonRadius, 0.05
            );
            for(auto &f : centers_and_masses){
                fragments.emplace_back(
                    glm::vec3((float)f.first[0], (float)f.first[1], (float)f.first[2]),
                    glm::vec3((float)moon.velocity[0],(float)moon.velocity[1], (float)moon.velocity[2] ),
                    f.second
                );
            }
            fragment_initialized = true;
        }

        // Gravity update
        if(fragment_initialized)
            parallelUpdateGravity(planet, fragments, deltaTime);
        else
            updateGravity(planet, moon, deltaTime);

        // ---------------- Draw planet ----------------
        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"model"),1,GL_FALSE,glm::value_ptr(model));
        glUniform3f(glGetUniformLocation(shaderProgram,"color"),0.2f,0.7f,1.0f);
        planetSphere.draw();

        // ---------------- Draw moon / fragments ----------------
        if(fragment_initialized){
            for(auto &f : fragments){
                glm::mat4 m = glm::translate(glm::mat4(1.0f), f.position);
                glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"model"),1,GL_FALSE,glm::value_ptr(m));
                glUniform3f(glGetUniformLocation(shaderProgram,"color"),1.0f,0.5f,0.0f);
                fragmentSphere.draw();
            }
        } else {
            glm::mat4 m = glm::translate(glm::mat4(1.0f), moon.position);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"model"),1,GL_FALSE,glm::value_ptr(m));
            glUniform3f(glGetUniformLocation(shaderProgram,"color"),1.0f,0.5f,0.0f);
            moonSphere.draw();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
