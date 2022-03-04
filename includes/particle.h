#ifndef PARTICLE_H
#define PARTICLE_H

#include <vector>

//TO DO:
// - Substitute pointers for vectors
// - glDrawBuffer

class Particle {
private:
    //private functions
    void spawnParticle(int ID);

    //particle variables
    std::vector<glm::mat4> pPos;
    std::vector<glm::vec4> pColors;
    std::vector<float> pSizes;

    //trail variables
    std::vector<int> trailSize;
    std::vector<std::vector<glm::mat4>> trailPosition;
    std::vector<std::vector<float>> trailAlpha;
    std::vector<int> lastTrail;

    //openGL variables
    GLuint VAO, shaderID;

public:
    //public variables
    int particleAmount;

    //constructor
    Particle(int amount);

    //class functions
    void setPosition(int ID, glm::vec3 position);
    void setColor(int ID, glm::vec4 color);
    void setSize(int ID, float size);
    void setTrailSize(int ID, int size);

    void drawParticle(int ID);
    void drawAllParticles();
};

#endif