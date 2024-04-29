#version 330
in vec2 vsoTexCoord;
in vec4 vsoPos;
uniform float zoom;
out vec4 fragColor;


uniform float temps;

void main(void) {

   float red = 0.5 + 0.5 * sin(temps * 30); // La composante rouge varie entre 0 et 1 avec le sinus du temps

  fragColor = vec4(red, 0.0,0.0, 1.0);
}
