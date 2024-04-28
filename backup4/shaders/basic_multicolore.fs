#version 330
in vec2 vsoTexCoord;
in vec4 vsoPos;
uniform float zoom;
out vec4 fragColor;

uniform float temps;

void main(void) {
    float rougeAlea = sin(temps * 30) ;
    float vertAlea  = cos(temps * 30) ;
    float bleuAlea  = tan(temps * 30) ;

    fragColor = vec4(rougeAlea, vertAlea, bleuAlea, 1.0);
}