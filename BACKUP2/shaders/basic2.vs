// Shader de vertex (basic.vs)
#version 330 core

// Les attributs de vertex
layout(location = 0) in vec3 aPos;

// La coordonnée de texture
out vec2 TexCoord;

void main()
{
    // Position du vertex
    gl_Position = vec4(aPos, 1.0);
    
    // Les coordonnées de texture correspondent simplement à la position du vertex
    TexCoord = aPos.xy;
}
