#version 330

in vec3 N;
in vec4 mvpos;
in vec4 vsoSMCoord;

out vec4 fragColor;

uniform vec4 scolor;
uniform vec4 lcolor;
uniform vec4 lumpos;
uniform sampler2D smTex;

//La puissance de la lumiere et aussi sa portee
uniform float lightPower;
uniform float lightRange; 

void main() {
  //modification de ambient diffus et spec 
  float distanceToLight = length(mvpos.xyz - lumpos.xyz);
  float attenuation = 1.0 / (1.0 + 0.1 * distanceToLight + 0.01 * distanceToLight * distanceToLight);
  float intensity = clamp(1.0 - distanceToLight / lightRange, 0.0, 1.0); //intensite lumineuse en fonction de la distance



  vec3 n = normalize(N);
  
  vec4 ambient = 0.15f * lightPower  * lcolor * scolor; /* lacolor, sacolor */
  
  vec3 Ld = normalize(mvpos.xyz - lumpos.xyz);
  float ild = clamp(dot(n, -Ld), 0.0, 1.0);


  vec4 diffus = (ild * lcolor * scolor * intensity * lightPower * attenuation); // Calcul de la couleur diffuse avec puissance et portée

  
  vec3 R = normalize(reflect(Ld, n));
  vec3 Vue = -normalize(mvpos.xyz);
  float ils = pow(clamp(dot(R, Vue), 0, 1), 20);
  
  vec4 spec = ils * lightPower * lcolor; /* sscolor */
  
  vec3 projCoords = vsoSMCoord.xyz / vsoSMCoord.w;
  if(texture(smTex, projCoords.xy).r  <  projCoords.z) {
    diffus *= 0.0;
    spec *= 0.0;
  }
  fragColor = ambient + diffus + spec;
}
