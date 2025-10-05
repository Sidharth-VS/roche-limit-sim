#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Sphere.h"
#include "Gravity.h"

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
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success){
        char infoLog[512];
        glGetShaderInfoLog(shader,512,nullptr,infoLog);
        std::cerr << "Shader compilation error:\n" << infoLog << std::endl;
    }
    return shader;
}

// -------------------- Camera --------------------
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f, pitch = 0.0f;
float lastX = 400, lastY = 300;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// -------------------- Callbacks --------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0,0,width,height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if(firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

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
    float cameraSpeed = 2.5f * deltaTime;
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraPos += cameraSpeed * cameraFront;
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraPos -= cameraSpeed * cameraFront;
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cameraPos -= glm::normalize(glm::cross(cameraFront,cameraUp))*cameraSpeed;
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cameraPos += glm::normalize(glm::cross(cameraFront,cameraUp))*cameraSpeed;
}

// -------------------- Main --------------------
int main(){
    // GLFW init
    if(!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(800,600,"Two Spheres",nullptr,nullptr);
    if(!window){glfwTerminate(); return -1;}
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
    glfwSetCursorPosCallback(window,mouse_callback);
    glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr << "Failed to init GLAD\n";
        return -1;
    }

    // Compile shaders
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram,vertexShader);
    glAttachShader(shaderProgram,fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Get user input for simulation parameters
    float planetMass, planetRadius, moonMass, moonRadius, moonVelocityY;
    float moonDistance;

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

    // Spheres
    Sphere mainSphere(planetRadius, 36, 18);
    Sphere smallSphere(moonRadius, 36, 18);
    Body planet = Body(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), planetMass);
    Body moon = Body(glm::vec3(moonDistance, 0.0f, 0.0f), glm::vec3(0.0f, moonVelocityY, 0.0f), moonMass);

    glEnable(GL_DEPTH_TEST);

    // ---------------- Render Loop ----------------
    while(!glfwWindowShouldClose(window)){
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        updateGravity(planet, moon, deltaTime);

        processInput(window);

        glClearColor(0.1f,0.1f,0.1f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos+cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f),800.0f/600.0f,0.1f,100.0f);

        GLuint viewLoc = glGetUniformLocation(shaderProgram,"view");
        GLuint projLoc = glGetUniformLocation(shaderProgram,"projection");
        glUniformMatrix4fv(viewLoc,1,GL_FALSE,glm::value_ptr(view));
        glUniformMatrix4fv(projLoc,1,GL_FALSE,glm::value_ptr(projection));

        // ---------------- Main Sphere ----------------
        glm::mat4 model = glm::mat4(1.0f);
        GLuint modelLoc = glGetUniformLocation(shaderProgram,"model");
        glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(model));

        GLuint colorLoc = glGetUniformLocation(shaderProgram,"color");
        glUniform3f(colorLoc,0.2f,0.7f,1.0f);
        mainSphere.draw();
        
        // ---------------- Smaller Sphere (orbiting) ----------------
        glm::mat4 smallModel = glm::mat4(1.0f);
        smallModel = glm::translate(smallModel, moon.position); // offset in x
        GLuint smallModelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(smallModelLoc, 1, GL_FALSE, glm::value_ptr(smallModel));
        glUniform3f(colorLoc,1.0f,0.5f,0.0f);
        smallSphere.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
