/**
 * @file particle.cpp
 * @author Murillo Moraes Martins
 * @brief This is a "generalized" 2D particle simulator, I tried to make the functions for general purposes, but some are kinda specific.
 * @version 0.1
 * @date 2022-03-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "particle.h"
#include "shader.h"

Particle::Particle(int amount){
    //resizing particle variables
    particleAmount = amount;
    pPos.resize(amount);
    pColors.resize(amount);
    pSizes.resize(amount);
    //resizing trail variables
    trailSize.resize(amount);
    trailPosition.resize(amount);
    trailAlpha.resize(amount);
    lastTrail.resize(amount);

    //initializing all variables
    for(int i = 0; i < amount; i++){
        pPos[i] = glm::mat4(1.0f);
        pColors[i] = glm::vec4(1.0f);
        pSizes[i] = 1.0f;
        trailSize[i] = 0;
        lastTrail[i] = 0;
    }

    //creating shader and getting it's ID
    Shader particleShader("./shaders/particle.vs", "./shaders/particle.fs");
    shaderID = particleShader.ID;

    //initializing point
    float point[] = {0.0f, 0.0f, 0.0f};

    //initializing and binding VAO and VBO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW);
    //enabling position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    //enabling blend function
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Particle::setPosition(int ID, glm::vec3 position){
    if(trailSize[ID] != 0){
        //positioning a trail in the particle's last position
        trailPosition[ID][lastTrail[ID]] = glm::mat4(1.0f); 
        trailPosition[ID][lastTrail[ID]] = glm::translate(trailPosition[ID][lastTrail[ID]], position);

        //changing trail color
        for(int i = 0; i < trailSize[ID]; i++){
            int index = lastTrail[ID] - i;
            if(index < 0) index = trailSize[ID] - (i-lastTrail[ID]);
            trailAlpha[ID][index] = 1.0f - (i/(float)trailSize[ID]);
        }
        lastTrail[ID] = (lastTrail[ID] + 1) % trailSize[ID];
    }
    //updating particle position
    pPos[ID] = glm::mat4(1.0f);
    pPos[ID] = glm::translate(pPos[ID], position);
}

void Particle::setColor(int ID, glm::vec4 color){
    pColors[ID] = color;
}

void Particle::setSize(int ID, float size){
    pSizes[ID] = size;
}

void Particle::setTrailSize(int ID, int size){
    //setting trail size
    trailSize[ID] = size;
    //resizing variables
    trailPosition[ID].resize(size);
    trailAlpha[ID].resize(size);

    for(int i = 0; i < size; i++){
        //initializing position
        trailPosition[ID][i] = glm::mat4(1.0f);
        glm::vec3 currPos = glm::vec3(pPos[ID][3].x, pPos[ID][3].y, pPos[ID][3].z);
        trailPosition[ID][i] = glm::translate(trailPosition[ID][i], currPos);

        //initializing color
        trailAlpha[ID][i] = 0.0f;
    }
}

void shaderSetMat4(const std::string &name, const glm::mat4 &mat, GLuint ID){
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void shaderSetVec4(const std::string &name, const glm::vec4 &value, GLuint ID){ 
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
}

void Particle::spawnParticle(int ID){
    //setting particle properties
    glPointSize(pSizes[ID]);

    for(int i = 0; i < trailSize[ID]; i++){
        shaderSetMat4("transform", trailPosition[ID][i], shaderID);
        shaderSetVec4("pColor", glm::vec4(pColors[ID].x, pColors[ID].y, pColors[ID].z, trailAlpha[ID][i]), shaderID);
        //drawing point
        glDrawArrays(GL_POINTS, 0, 1);
    }

    shaderSetMat4("transform", pPos[ID], shaderID);
    shaderSetVec4("pColor", pColors[ID], shaderID);
    //drawing point
    glDrawArrays(GL_POINTS, 0, 1);
    
}

void Particle::drawParticle(int ID){
    //using shader and binding VAO
    glUseProgram(shaderID);
    glBindVertexArray(VAO);

    spawnParticle(ID);
    
    //reseting vertex array
    glBindVertexArray(0);
}

void Particle::drawAllParticles(){
    //using shader and binding VAO
    glUseProgram(shaderID);
    glBindVertexArray(VAO);

    for(int i = 0; i < particleAmount; i++)
        spawnParticle(i);

    //reseting vertex array
    glBindVertexArray(0);
}