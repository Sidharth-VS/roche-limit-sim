#ifndef SPHERE_H
#define SPHERE_H

#include <glad/glad.h>
#include <vector>
#include <cmath>

class Sphere {
public:
    Sphere(float radius = 1.0f, unsigned int sectorCount = 36, unsigned int stackCount = 18) {
        buildSphere(radius, sectorCount, stackCount);
    }

    ~Sphere() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void draw() {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    }

private:
    GLuint VAO{}, VBO{}, EBO{};
    unsigned int indexCount{};

    void buildSphere(float radius, unsigned int sectorCount, unsigned int stackCount) {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
        const float PI = 3.14159265359f;

        for (unsigned int i = 0; i <= stackCount; ++i) {
            float stackAngle = PI / 2 - i * (PI / stackCount);
            float xy = radius * cosf(stackAngle);
            float z = radius * sinf(stackAngle);

            for (unsigned int j = 0; j <= sectorCount; ++j) {
                float sectorAngle = j * (2 * PI / sectorCount);
                float x = xy * cosf(sectorAngle);
                float y = xy * sinf(sectorAngle);
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);
            }
        }

        for (unsigned int i = 0; i < stackCount; ++i) {
            unsigned int k1 = i * (sectorCount + 1);
            unsigned int k2 = k1 + sectorCount + 1;
            for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
                if (i != 0) {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }
                if (i != (stackCount - 1)) {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }
            }
        }

        indexCount = indices.size();

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }
};

#endif
