#version 330
in  vec2 vsoTexCoord;
in  vec4 vsoPos;
uniform float zoom;
out vec4 fragColor;

/*
 * 2D, 3D and 4D Perlin noise, classic and simplex, in a GLSL fragment shader.

 * Author: Stefan Gustavson ITN-LiTH (stegu@itn.liu.se) 2004-12-05
 * Simplex indexing functions by Bill Licea-Kane, ATI (bill@ati.com)
 */


//#extension GL_ARB_explicit_attrib_location : enable

uniform sampler2D permTexture;
uniform sampler2D gradTexture;

#define ONE 0.00390625
#define ONEHALF 0.001953125

float snoise(vec2 P) {


#define F2 0.366025403784

#define G2 0.211324865405

  // Skew the (x,y) space to determine which cell of 2 simplices we're in
 	float s = (P.x + P.y) * F2;   // Hairy factor for 2D skewing
  vec2 Pi = floor(P + s);
  float t = (Pi.x + Pi.y) * G2; // Hairy factor for unskewing
  vec2 P0 = Pi - t; // Unskew the cell origin back to (x,y) space
  Pi = Pi * ONE + ONEHALF; // Integer part, scaled and offset for texture lookup

  vec2 Pf0 = P - P0;  // The x,y distances from the cell origin


  vec2 o1;
  if(Pf0.x > Pf0.y) o1 = vec2(1.0, 0.0);  // +x, +y traversal order
  else o1 = vec2(0.0, 1.0);               // +y, +x traversal order


  vec2 grad0 = texture(permTexture, Pi).rg * 4.0 - 1.0;
  float t0 = 0.5 - dot(Pf0, Pf0);
  float n0;
  if (t0 < 0.0) n0 = 0.0;
  else {
    t0 *= t0;
    n0 = t0 * t0 * dot(grad0, Pf0);
  }

  vec2 Pf1 = Pf0 - o1 + G2;
  vec2 grad1 = texture(permTexture, Pi + o1*ONE).rg * 4.0 - 1.0;
  float t1 = 0.5 - dot(Pf1, Pf1);
  float n1;
  if (t1 < 0.0) n1 = 0.0;
  else {
    t1 *= t1;
    n1 = t1 * t1 * dot(grad1, Pf1);
  }

  vec2 Pf2 = Pf0 - vec2(1.0-2.0*G2);
  vec2 grad2 = texture(permTexture, Pi + vec2(ONE, ONE)).rg * 4.0 - 1.0;
  float t2 = 0.5 - dot(Pf2, Pf2);
  float n2;
  if(t2 < 0.0) n2 = 0.0;
  else {
    t2 *= t2;
    n2 = t2 * t2 * dot(grad2, Pf2);
  }

  return 70.0 * (n0 + n1 + n2);
}




uniform float temps;

void main() {
    //perlin fonction temps 
    float valeur_bruit = snoise(vsoTexCoord * 10.0 + vec2(1.0, temps * 8));

    //on ajoute du bruit 
    valeur_bruit = (valeur_bruit + 0.7) * 0.7;

    // dpelacement lave
    float  deplacement = sin(temps * 2.0) * 0.2 + 0.8;

    // couleur lave
    vec3 color = vec3(1.0, 0.3, 0.0) * valeur_bruit ;

    // Dessiner la lave
    gl_FragColor = vec4(color, 1.0);
}