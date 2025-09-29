#ifndef GRAVITY_H
#define GRAVITY_H
#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;

class Body{
    public:

    glm::vec3 position;
    glm::vec3 velocity;
    float mass;
    

    Body(glm::vec3 pos, glm::vec3 vel, float m):position(pos), velocity(vel), mass(m){}
    
};

void updateGravity(Body& planet, Body& moon, float dTime){
    const float G = 0.0001f;
    glm::vec3 dir = planet.position - moon.position;
    float distance = glm::length(dir);
    glm::vec3 dirNorm = glm::normalize(dir);

    float force = G*planet.mass*moon.mass/(distance*distance);

    glm::vec3 acc = (force/moon.mass)*dirNorm;

    moon.velocity += acc*dTime;
    moon.position += moon.velocity*dTime;
}



#endif