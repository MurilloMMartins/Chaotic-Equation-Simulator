#ifndef TEXT_H
#define TEXT_H

#include "shader.h"

void FreeTypeInit();
void RenderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 color);

#endif