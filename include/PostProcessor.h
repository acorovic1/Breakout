#pragma once

#define POST_PROCESSOR_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Texture.h"
#include "SpriteRenderer.h"
#include "Shader.h"

class PostProcessor
{
public:
    
    Shader PostProcessingShader;
    Texture2D Texture;
    unsigned int Width, Height;
    
    bool Confuse, Chaos, Shake;
    
    PostProcessor(Shader shader, unsigned int width, unsigned int height);
    void BeginRender(); // prepares the postprocessor's framebuffer operations before rendering the game
    void EndRender(); // should be called after rendering the game, so it stores all the rendered data into a texture object
    void Render(float time);
private:
    
    unsigned int MSFBO, FBO; // MSFBO = Multisampled FBO
    unsigned int RBO; // RBO is used for multisampled color buffer
    unsigned int VAO;
   
    void initRenderData();
};