// Shader de fragment (basic.fs)
#version 330 core

// La sortie du shader fragment est la couleur du pixel
out vec4 FragColor;

void main()
{
    // Utiliser la coordonnée de texture pour générer une couleur fractale
    vec2 uv = gl_FragCoord.xy / vec2(800, 600); // Taille de la fenêtre
    vec3 color = vec3(uv, 0.5 + 0.5 * sin(uv.x * 10.0 + uv.y * 20.0));
    
    // Output the color
    FragColor = vec4(color, 1.0);
}
