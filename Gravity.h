#ifndef GRAVITY_H
#define GRAVITY_H
#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <omp.h>
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
const float G = 0.1f;

void updateGravity(Body& planet, Body& moon, float dTime){
    glm::vec3 dir = planet.position - moon.position;
    float distance = glm::length(dir);
    glm::vec3 dirNorm = glm::normalize(dir);

    float force = G*planet.mass*moon.mass/(distance*distance);

    glm::vec3 acc = (force/moon.mass)*dirNorm;

    moon.velocity += acc*dTime;
    moon.position += moon.velocity*dTime;

}
void parallelUpdateGravity(Body& planet, vector<Body>& fragments, float dTime){
    #pragma omp parallel for
    for(auto& fragment : fragments){
        glm::vec3 dir = planet.position - fragment.position;
        float distance = glm::length(dir);
        glm::vec3 dirNorm = glm::normalize(dir);

        float force = G*planet.mass*fragment.mass/(distance*distance);

        glm::vec3 acc = (force/fragment.mass)*dirNorm;

        fragment.velocity += acc*dTime;
        fragment.position += fragment.velocity*dTime;
    }

}
void serialUpdateGravity(Body& planet, vector<Body>& fragments, float dTime){

    for(auto& fragment : fragments){
        glm::vec3 dir = planet.position - fragment.position;
        float distance = glm::length(dir);
        glm::vec3 dirNorm = glm::normalize(dir);

        float force = G*planet.mass*fragment.mass/(distance*distance);

        glm::vec3 acc = (force/fragment.mass)*dirNorm;

        fragment.velocity += acc*dTime;
        fragment.position += fragment.velocity*dTime;
    }

}



// void updateGravity(vector<Body>& bodies, float dTime){
//     const float G = 0.001f;
//     vector<glm::vec3> totalForces(bodies.size(), glm::vec3(0.0f));

//     // For each target body j, sum forces due to ALL other i
//     for (size_t j = 0; j < bodies.size(); ++j) {
//         for (size_t i = 0; i < bodies.size(); ++i) {
//             if (i == j) continue;
//             glm::vec3 dir = bodies[i].position - bodies[j].position;
//             float dist = glm::length(dir);
//             if (dist > 1e-5f) {
//                 glm::vec3 dirNorm = glm::normalize(dir);
//                 float forceMag = G * bodies[j].mass * bodies[i].mass / (dist * dist);
//                 totalForces[j] += forceMag * dirNorm;
//             }
//         }
//     }
//     // Then update each body's velocity and position ONCE per step
//     for (size_t j = 0; j < bodies.size(); ++j) {
//         glm::vec3 acc = totalForces[j] / bodies[j].mass;
//         bodies[j].velocity += acc * dTime;
//         bodies[j].position += bodies[j].velocity * dTime;

//     }

// }

// void updateGravity(vector<Body>& bodies, float dTime){
//     const float G = 0.0001f;
//     vector<glm::vec3> totalForces(bodies.size(), glm::vec3(0.0f));

//     //update if atleast one of the bodies is teh planet
//     for (size_t j = 0; j < bodies.size(); ++j) {
//         for (size_t i = 0; i < bodies.size(); ++i) {
//             if (i == bodies.size()-1 || j == bodies.size()-1){
//                 glm::vec3 dir = bodies[i].position - bodies[j].position;
//                 float dist = glm::length(dir);
//                 if (dist > 1e-5f) {
//                     glm::vec3 dirNorm = glm::normalize(dir);
//                     float forceMag = G * bodies[j].mass * bodies[i].mass / (dist * dist);
//                     totalForces[j] += forceMag * dirNorm;
//                 }
//             }
//         }
//     }
//     for (size_t j = 0; j < bodies.size(); ++j) {
//         glm::vec3 acc = totalForces[j] / bodies[j].mass;
//         bodies[j].velocity += acc * dTime;
//         bodies[j].position += bodies[j].velocity * dTime;

//     }

// }

#endif