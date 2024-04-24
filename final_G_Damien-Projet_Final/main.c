#include <GL4D/gl4duw_SDL2.h>
#include <stdio.h>
#include <stdbool.h>


static void init(void);
static void resize(int width, int height);
static void draw(void);
static void sortie(void);

static const GLuint SHADOW_MAP_SIDE = 1024;
static GLuint _wW = 1200, _wH = 960;
static GLuint _quadId = 0;

static GLuint _cubeId = 0;

static GLuint _torusId = 0, _terrainId = 0;
static GLuint _smTex = 0;
static GLuint _pId = 0, _smpId = 0, _fbo = 0;

/*!\brief active/désactive la pause (de rotation) */
static GLboolean _pause = GL_FALSE;

//X_camera y_camera  Zcamera
static float _z_camera = -100.0;

static float _y_camera = -108.8;

static float _x_camera = -100.0;

//Variable de rotation de camera  
static float _rota_camera = 0; //tourner 

static float _rotaX_camera = 0; //pencher 


static void keyup(int keycode);
static void keydown(int keycode);

/*!\brief flag pour savoir si la touche shift est enfoncée */
static GLboolean _shift = GL_FALSE;


static GLfloat a0 = 0.0f;


//pour le saut
static GLboolean _isJumping = GL_FALSE;
static GLfloat _jumpSpeed = 0.1f;
static GLfloat _gravity = 0.005f;





int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "Eclairage", GL4DW_POS_CENTERED, GL4DW_POS_CENTERED,
			 _wW, _wH, GL4DW_OPENGL | GL4DW_RESIZABLE | GL4DW_SHOWN)) {
    fprintf(stderr, "Erreur lors de la création de la fenêtre\n");
    return 1;
  }
  init();
  atexit(sortie);
  gl4duwResizeFunc(resize);
  gl4duwDisplayFunc(draw);
  gl4duwKeyUpFunc(keyup);
  gl4duwKeyDownFunc(keydown);
  gl4duwMainLoop();
  return 0;
}

void init(void) {
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  _quadId = gl4dgGenQuadf();

  _cubeId = gl4dgGenCubef();


  _torusId = gl4dgGenTorusf(20, 10, 0.3);
  GLfloat * hm = gl4dmTriangleEdge(513, 513, 0.4f);
  _terrainId = gl4dgGenConef(20, GL_TRUE); //gl4dgGenGrid2dFromHeightMapf(513, 513, hm);
  free(hm);
  _pId = gl4duCreateProgram("<vs>shaders/lights.vs", "<fs>shaders/lights.fs", NULL);
  _smpId  = gl4duCreateProgram("<vs>shaders/shadowMap.vs", "<fs>shaders/shadowMap.fs", NULL);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  gl4duGenMatrix(GL_FLOAT, "proj");
  gl4duGenMatrix(GL_FLOAT, "mod");
  gl4duGenMatrix(GL_FLOAT, "view");
  
  gl4duGenMatrix(GL_FLOAT, "lightView");
  gl4duGenMatrix(GL_FLOAT, "lightProj");
  
  /* Création et paramétrage de la Texture de shadow map */
  glGenTextures(1, &_smTex);
  glBindTexture(GL_TEXTURE_2D, _smTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_SIDE, SHADOW_MAP_SIDE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  /* Création du Framebuffer Object */
  glGenFramebuffers(1, &_fbo);
  resize(_wW, _wH);
}

static void resize(int width, int height) {
  GLfloat ratio;
  _wW = width;
  _wH = height;
  glViewport(0, 0, _wW, _wH);
  ratio = _wW / ((GLfloat)_wH);

  //Puissance et porte lumiere 
  gl4duBindMatrix("proj");
  gl4duLoadIdentityf();
  gl4duFrustumf(-ratio, ratio, -1.0f, 1.0f, 2.0f, 500.0f);
  gl4duBindMatrix("lightProj");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.1f, 0.1f, -0.1f, 0.1f, 0.1f, 100.0f);
}




typedef struct {
    float x;
    float y;
    float z;
    float width;
    float height;
    float depth;
     char color[20] // Variable de type chaîne de caractères pour la couleur
} Cube;



// Définition des differents cubes
Cube initFirstCube() {
    Cube cube;
    cube.x = 32.0f;         
    cube.y = 0.0f;       
    cube.z = 0.0f;       
    cube.width = 4.0f;    
    cube.height = 20.0f;   
    cube.depth = 4.0f;    
    return cube;
}



Cube initSecondCube() {
  Cube cube;
  cube.x = -32.0f; cube.y = 0.0f;  cube.z = 0.0f;       
  cube.width = 4.0f; cube.height = 20.0f;  cube.depth = 4.0f;    
  return cube;
}

Cube init3Cube() {
  Cube cube;
  cube.x = 1.0f; cube.y = 0.0f;  cube.z = 0.0f;       
  cube.width = 4.0f; cube.height = 20.0f;  cube.depth = 4.0f;  
  return cube;
}

//cube de la premiere salle  mur gauche (bleu)
Cube init4Cube() {
  Cube cube;
  cube.x = -85.0f; cube.y = -100.0f;  cube.z = -100.0f;       
  cube.width = 2.0f; cube.height = 30.0f; cube.depth = 70.0f;   
  return cube;
}

//  mur droite (violet)
Cube init5Cube() {
  Cube cube;
  cube.x = -115.0f; cube.y = -100.0f;  cube.z = -105.0f;       
  cube.width = 2.0f; cube.height = 30.0f; cube.depth = 25.0f;    
  return cube;
}

//  mur sol (orange) 
Cube init6Cube() {
  Cube cube;
  cube.x = -100.0f; cube.y = -96.0f;  cube.z = -100.0f;       
  cube.width = 16.0f; cube.height = 2.0f;   cube.depth = 50.0f;    
  return cube;
}

//mur du fond  (rouge )
Cube init7Cube() {
  Cube cube;
  cube.x = -100.0f; cube.y = -100.0f;  cube.z = -129.0f;       
  cube.width = 18.0f; cube.height = 30.0f;  cube.depth = 2.0f;    
  return cube;
}

//mur du fond  (vert)
Cube init8Cube() {
  Cube cube;
  cube.x = -140.0f; cube.y = -100.0f;  cube.z = -50.0f;       
  cube.width = 57.0f; cube.height = 30.0f; cube.depth = 2.0f;    
  return cube;
}

//  mur sol (jaune) 
Cube init9Cube() {
  Cube cube;
  cube.x = -151.0f; cube.y = -96.0f;  cube.z = -70.0f;       
  cube.width = 35.0f; cube.height = 2.0f; cube.depth = 20.0f;   
  return cube;
}

//cube saut 1 tuto (rouge)
Cube init10Cube() {
  Cube cube;
  cube.x = -150.0f; cube.y = -100.0f;  cube.z = -70.0f;       
  cube.width = 2.0f; cube.height = 3.0f; cube.depth = 19.0f;   
  return cube;
}

//cube saut 2 tuto (violet) 
Cube init11Cube() {
  Cube cube;
  cube.x = -175.0f; cube.y = -103.0f;  cube.z = -70.0f;       
  cube.width = 2.0f; cube.height = 7.0f; cube.depth = 19.0f;   
  return cube;
}


//cube sol salle1 orange (on annule la collision haute)
Cube init12Cube() {
  Cube cube;
  cube.x = -187.0f; cube.y = -83.0f;  cube.z = -70.0f;       
  cube.width = 10.1f; cube.height = 27.0f; cube.depth = 130.0f;   
  return cube;
}


//sol (jaune) anti gravite vertical 
Cube init13Cube() {
  Cube cube;
  cube.x = -185.0f; cube.y = -109.0f;  cube.z = -65.0f;       
  cube.width = 10.0f; cube.height = 1.5f; cube.depth = 15.0f;      
  return cube;
}

//mur (bleu) vertical 
Cube init14Cube() {
  Cube cube;
  cube.x = -157.0f; cube.y = -100.0f;  cube.z = -82.0f;       
  cube.width = 40.0f; cube.height = 30.0f; cube.depth = 2.0f;      
  return cube;
}

//mur salle gauche (rouge)   
Cube init15Cube() {
  Cube cube;
  cube.x = -273.0f; cube.y = -80.0f;  cube.z = 30.0f;       
  cube.width = 77.0f; cube.height = 30.0f;  cube.depth = 2.0f;    
  return cube;
}

//mur salle droite (bleu)  
Cube init16Cube() {
  Cube cube;
  cube.x = -273.0f; cube.y = -80.0f;  cube.z = -160.0f;       
  cube.width = 77.0f; cube.height = 30.0f; cube.depth = 2.0f;    
  return cube;
}

//Plateforme 1
Cube init17Cube() {
  Cube cube;
  cube.x = -243.0f; cube.y = -55.0f;  cube.z = -100.0f;       
  cube.width = 10.0f; cube.height = 2.0f;  cube.depth = 10.0f;    
  return cube;
}

//mPlateforme 2
Cube init18Cube() {
  Cube cube;
  cube.x = -263.0f; cube.y = -60.0f;  cube.z = -60.0f;       
  cube.width = 6.0f; cube.height = 2.0f; cube.depth = 8.0f;    
  return cube;
}

//mPlateforme 3
Cube init19Cube() {
  Cube cube;
  cube.x = -223.0f; cube.y = -60.0f;  cube.z = -40.0f;       
  cube.width = 6.0f;  cube.height = 2.0f; cube.depth = 6.0f;    
  return cube;
}

//mPlateforme 4
Cube init20Cube() {
  Cube cube;
  cube.x = -270.0f; cube.y = -64.0f;  cube.z = -70.0f;       
  cube.width = 2.0f; cube.height = 2.0f; cube.depth = 4.0f;    
  return cube;
}

//mPlateforme 5
Cube init21Cube() {
  Cube cube;
  cube.x = -285.0f; cube.y = -72.0f;  cube.z = -40.0f;       
  cube.width = 2.0f; cube.height = 2.0f; cube.depth = 4.0f;    
  return cube;
}

//mPlateforme 6
Cube init22Cube() {
  Cube cube;
  cube.x = -285.0f; cube.y = -72.0f;  cube.z = -20.0f;       
  cube.width = 1.0f; cube.height = 2.0f;  cube.depth = 2.0f;    
  return cube;
}

//mPlateforme 7
Cube init23Cube() {
  Cube cube;
  cube.x = -305.0f; cube.y = -72.0f;  cube.z = -0.0f;       
  cube.width = 3.0f; cube.height = 1.0f;  cube.depth = 1.0f;    
  return cube;
}

//mPlateforme 8
Cube init24Cube() {
  Cube cube;
  cube.x = -325.0f; cube.y = -77.0f;  cube.z = 15.0f;       
  cube.width = 7.0f; cube.height = 1.5f; cube.depth = 1.0f;    
  return cube;
}

//mPlateforme 9
Cube init25Cube() {
  Cube cube;
  cube.x = -355.0f; cube.y = -65.0f;  cube.z = 25.0f;       
  cube.width = 10.0f; cube.height = 16.0f; cube.depth = 5.0f;    
  return cube;
}

//cube mur du fond salle orange  
Cube init26Cube() {
  Cube cube;
  cube.x = -357.0f; cube.y = -80.0f;  cube.z = -70.0f;       
  cube.width = 10.1f; cube.height = 30.0f;  cube.depth = 90.0f;   
  return cube;
}



  //LAVE
  //cube mur du fond salle orange  
  Cube init27Cube() {
    Cube cube;
    cube.x = -260.0f;  cube.y = -35.0f;  cube.z = -40.0f;        
    cube.width = 130.1f; cube.height = 4.0f;  cube.depth = 130.0f;   
    return cube;
  }



/*CREATION DE LISTE */
#define MAX_CUBES 27 // Nombre maximal de cubes
Cube cubes[MAX_CUBES]; // Tableau de cubes
int numCubes = 0; 


/* REGLE DE CREATION 

X  = vers la droite
-X = vers la gauche 
Y = vers le bas 
-Y = vers le haut 

MAIS POUR AFFICHER LES CUBE IL FAUT mettre un MOINS (-) mais le calcul des colision change pas 

*/



void scene(GLboolean sm, GLfloat dt) {
  static GLfloat x = 0.0f;
  static GLfloat bleu[] = {0.0f, 0.0f, 1.0f, 1.0f};        // Bleu
  static GLfloat orange[] = {1.0f, 0.5f, 0.0f, 1.0f};     // Orange
  static GLfloat gris_clair[] = {0.9f, 0.9f, 0.9f, 1.0f}; // Gris clair à 80%
  static GLfloat gris_moyen[] = {0.7f, 0.7f, 0.7f, 1.0f}; // Gris clair à 80%
  static GLfloat gris_fonce[] = {0.5f, 0.5f, 0.5f, 1.0f}; // Gris clair à 80%
  static GLfloat violet[] = {0.5f, 0.0f, 0.5f, 1.0f};     // Violet
  static GLfloat rouge[] = {1.0f, 0.0f, 0.0f, 1.0f};      // Rouge
  static GLfloat jaune[] = {1.0f, 1.0f, 0.0f, 1.0f};      // Jaune
  static GLfloat blanc[] = {1.0f, 1.0f, 1.0f, 1.0f};      // Blanc
  static GLfloat vert[] = {0.0f, 1.0f, 0.0f, 1.0f};       // Vert

  GLfloat lumpos0[] = { 0.0f, 3.5f, 1.5f, 1.0f }, lumpos[4], * mat;
  lumpos0[0] = 4.0f * sin(x);

  gl4duBindMatrix("view");

  /* gl4duLoadIdentityf(); */
  /* gl4duLookAtf(lumpos0[0], lumpos0[1], lumpos0[2], 0, 0, -5.0f, 0, 1, 0); */
  
  mat = (GLfloat *)gl4duGetMatrixData();
  MMAT4XVEC4(lumpos, mat, lumpos0);
  
  if(sm) {
    glCullFace(GL_FRONT);
    glUseProgram(_smpId);

    //La lumiere avec la rotation et le deplacement 
    gl4duBindMatrix("lightView");
    gl4duLoadIdentityf();
    //gl4duLookAtf(lumpos0[0], lumpos0[1], lumpos0[2], 0, 0, -5.0f, 0, 1, 0);
    gl4duRotatef(_rotaX_camera, 1.0f, 0.0f, 0.0f);
    gl4duRotatef(_rota_camera, 0.0f, 1.0f, 0.0f);
    
    gl4duTranslatef(_x_camera, _y_camera, _z_camera);


    //la puissance de la lumière qu'on regle sur inferieur a 1 pour un coté immesif
    float lightPower = 1.0f; 
    glUniform1f(glGetUniformLocation(_pId, "lightPower"), lightPower);

    //la distance maximale de la lumière 
    float lightRange = 100.0f; 
    glUniform1f(glGetUniformLocation(_smpId, "lightRange"), lightRange);

    /*printf("lightPower: %f\n", lightPower);
    printf("lightRange: %f\n", lightRange); */

  } else {
    glCullFace(GL_BACK);
    glUseProgram(_pId);
    x += dt;

    //la puissance de la lumière qu'on regle sur inferieur a 1 pour un coté immesif
    float lightPower = 1.0f; 
    glUniform1f(glGetUniformLocation(_pId, "lightPower"), lightPower);

    //la distance maximale de la lumière
    float lightRange = 100.0f;
    glUniform1f(glGetUniformLocation(_pId, "lightRange"), lightRange);
    
    /*printf("lightPower: %f\n", lightPower);
    printf("lightRange: %f\n", lightRange); */


  }
  glEnable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _smTex);
  glUniform1i(glGetUniformLocation(_pId, "smTex"), 0);

  gl4duBindMatrix("mod");

  gl4duLoadIdentityf();
  gl4duTranslatef(0.0f, 0.0f, -4.0f);
  
  //gl4duScalef(7.0f, 1.0f, 7.0f);


  /*TEST*/
  gl4duScalef(7.0f, 5.0f, 0.0f);
  gl4duRotatef(-90.0f, 0.0f, 0.0f, 0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, rouge);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, vert);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_quadId);

  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, jaune);


    /*TEST*/
  //CUBE 1
  cubes[0] = initFirstCube();
  gl4duLoadIdentityf();

  gl4duTranslatef(cubes[0].x, cubes[0].y , cubes[0].z);
    // X(largeur) Y(hauteur)  Z(devant/ derriere ) 
  gl4duScalef(cubes[0].width,  cubes[0].height, cubes[0].depth);

  //gl4duRotatef(-90.0f, 1.0f, 0.0f, -4.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, -0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, jaune);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_cubeId);


  //CUBE 2
  cubes[1] = initSecondCube();
  gl4duLoadIdentityf();

  gl4duTranslatef(cubes[1].x, cubes[1].y , cubes[1].z);
    // X(largeur) Y(hauteur)  Z(devant/ derriere ) 
  gl4duScalef(cubes[1].width,  cubes[1].height, cubes[1].depth);

  //gl4duRotatef(-90.0f, 1.0f, 0.0f, -4.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, -0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_cubeId);



  //CUBE3
  cubes[2] = init3Cube();
  gl4duLoadIdentityf();

  gl4duTranslatef(cubes[2].x, cubes[2].y , cubes[2].z);
    // X(largeur) Y(hauteur)  Z(devant/ derriere ) 
  gl4duScalef(cubes[2].width,  cubes[2].height, cubes[2].depth);

  //gl4duRotatef(-90.0f, 1.0f, 0.0f, -4.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, -0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, bleu);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_cubeId);


  //Devut SALLE
  //CUBE4 Bleu
  cubes[3] = init4Cube();
  gl4duLoadIdentityf();

  gl4duTranslatef(-cubes[3].x, -cubes[3].y , -cubes[3].z);
    // X(largeur) Y(hauteur)  Z(devant/ derriere ) 
  gl4duScalef(cubes[3].width,  cubes[3].height, cubes[3].depth);

  //gl4duRotatef(-90.0f, 1.0f, 0.0f, -4.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, -0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, gris_clair);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_cubeId);


  //CUBE 5
  cubes[4] = init5Cube();
  gl4duLoadIdentityf();

  gl4duTranslatef(-cubes[4].x, -cubes[4].y , -cubes[4].z);
    // X(largeur) Y(hauteur)  Z(devant/ derriere ) 
  gl4duScalef(cubes[4].width,  cubes[4].height, cubes[4].depth);

  //gl4duRotatef(-90.0f, 1.0f, 0.0f, -4.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, -0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, gris_clair);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_cubeId);


  //CUBE 6 orange
  cubes[5] = init6Cube();
  gl4duLoadIdentityf();

  gl4duTranslatef(-cubes[5].x, -cubes[5].y , -cubes[5].z);
    // X(largeur) Y(hauteur)  Z(devant/ derriere ) 
  gl4duScalef(cubes[5].width,  cubes[5].height, cubes[5].depth);

  //gl4duRotatef(-90.0f, 1.0f, 0.0f, -4.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, -0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, gris_moyen);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_cubeId);


  //CUBE 7 rouge
  cubes[6] = init7Cube();
  gl4duLoadIdentityf();

  gl4duTranslatef(-cubes[6].x, -cubes[6].y , -cubes[6].z);
    // X(largeur) Y(hauteur)  Z(devant/ derriere ) 
  gl4duScalef(cubes[6].width,  cubes[6].height, cubes[6].depth);

  //gl4duRotatef(-90.0f, 1.0f, 0.0f, -4.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, -0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, rouge);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_cubeId);


  //CUBE 8 mur vert 
  cubes[7] = init8Cube();
  gl4duLoadIdentityf();

  gl4duTranslatef(-cubes[7].x, -cubes[7].y , -cubes[7].z);
    // X(largeur) Y(hauteur)  Z(devant/ derriere ) 
  gl4duScalef(cubes[7].width,  cubes[7].height, cubes[7].depth);

  //gl4duRotatef(-90.0f, 1.0f, 0.0f, -4.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, -0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_cubeId);


  //CUBE 9 sol jaune 
  cubes[8] = init9Cube();
  gl4duLoadIdentityf();

  gl4duTranslatef(-cubes[8].x, -cubes[8].y , -cubes[8].z);
    // X(largeur) Y(hauteur)  Z(devant/ derriere ) 
  gl4duScalef(cubes[8].width,  cubes[8].height, cubes[8].depth);

  //gl4duRotatef(-90.0f, 1.0f, 0.0f, -4.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, -0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, gris_moyen);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_cubeId);


 //CUBE 10 saut1 rouge  
  cubes[9] = init10Cube();
  gl4duLoadIdentityf();

  gl4duTranslatef(-cubes[9].x, -cubes[9].y , -cubes[9].z);
    // X(largeur) Y(hauteur)  Z(devant/ derriere ) 
  gl4duScalef(cubes[9].width,  cubes[9].height, cubes[9].depth);

  //gl4duRotatef(-90.0f, 1.0f, 0.0f, -4.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, -0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, rouge);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_cubeId);


  //CUBE 11 saut1 violet  
  cubes[10] = init11Cube();
  gl4duLoadIdentityf();

  gl4duTranslatef(-cubes[10].x, -cubes[10].y , -cubes[10].z);
    // X(largeur) Y(hauteur)  Z(devant/ derriere ) 
  gl4duScalef(cubes[10].width,  cubes[10].height, cubes[10].depth);

  //gl4duRotatef(-90.0f, 1.0f, 0.0f, -4.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, -0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, violet);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_cubeId);


  //CUBE 12 sol salle1 orange  
  cubes[11] = init12Cube();
  gl4duLoadIdentityf();

  gl4duTranslatef(-cubes[11].x, -cubes[11].y , -cubes[11].z);
    // X(largeur) Y(hauteur)  Z(devant/ derriere ) 
  gl4duScalef(cubes[11].width,  cubes[11].height, cubes[11].depth);

  //gl4duRotatef(-90.0f, 1.0f, 0.0f, -4.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, -0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, gris_moyen);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_cubeId);


  //CUBE 13  sol antigravite jaune
  cubes[12] = init13Cube();
  gl4duLoadIdentityf();

  gl4duTranslatef(-cubes[12].x, -cubes[12].y , -cubes[12].z);
    // X(largeur) Y(hauteur)  Z(devant/ derriere ) 
  gl4duScalef(cubes[12].width,  cubes[12].height, cubes[12].depth);

  //gl4duRotatef(-90.0f, 1.0f, 0.0f, -4.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, -0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, gris_moyen);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_cubeId);


  //CUBE 14 mure bleu 
  cubes[13] = init14Cube();
  gl4duLoadIdentityf();

  gl4duTranslatef(-cubes[13].x, -cubes[13].y , -cubes[13].z);
    // X(largeur) Y(hauteur)  Z(devant/ derriere ) 
  gl4duScalef(cubes[13].width,  cubes[13].height, cubes[13].depth);

  //gl4duRotatef(-90.0f, 1.0f, 0.0f, -4.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, -0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, bleu);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_cubeId);


  //CUBE 15 mure rouge gauche salle 1 
  cubes[14] = init15Cube();
  gl4duLoadIdentityf();

  gl4duTranslatef(-cubes[14].x, -cubes[14].y , -cubes[14].z);
    // X(largeur) Y(hauteur)  Z(devant/ derriere ) 
  gl4duScalef(cubes[14].width,  cubes[14].height, cubes[14].depth);

  //gl4duRotatef(-90.0f, 1.0f, 0.0f, -4.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, -0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, gris_clair);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_cubeId);


  //CUBE 16 mure rouge gauche salle 1 
  cubes[15] = init16Cube();
  gl4duLoadIdentityf();

  gl4duTranslatef(-cubes[15].x, -cubes[15].y , -cubes[15].z);
    // X(largeur) Y(hauteur)  Z(devant/ derriere ) 
  gl4duScalef(cubes[15].width,  cubes[15].height, cubes[15].depth);

  //gl4duRotatef(-90.0f, 1.0f, 0.0f, -4.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, -0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, gris_clair);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_cubeId);

 
  //CUBE 17 plateforme
  cubes[16] = init17Cube();
  gl4duLoadIdentityf();

  gl4duTranslatef(-cubes[16].x, -cubes[16].y , -cubes[16].z);
    // X(largeur) Y(hauteur)  Z(devant/ derriere ) 
  gl4duScalef(cubes[16].width,  cubes[16].height, cubes[16].depth);

  //gl4duRotatef(-90.0f, 1.0f, 0.0f, -4.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, -0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_cubeId);


  //CUBE 18 plateforme
  cubes[17] = init18Cube();
  gl4duLoadIdentityf();

  gl4duTranslatef(-cubes[17].x, -cubes[17].y , -cubes[17].z);
    // X(largeur) Y(hauteur)  Z(devant/ derriere ) 
  gl4duScalef(cubes[17].width,  cubes[17].height, cubes[17].depth);

  //gl4duRotatef(-90.0f, 1.0f, 0.0f, -4.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, -0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_cubeId);


  //CUBE 19 plateforme
  cubes[18] = init19Cube();
  gl4duLoadIdentityf();

  gl4duTranslatef(-cubes[18].x, -cubes[18].y , -cubes[18].z);
    // X(largeur) Y(hauteur)  Z(devant/ derriere ) 
  gl4duScalef(cubes[18].width,  cubes[18].height, cubes[18].depth);

  //gl4duRotatef(-90.0f, 1.0f, 0.0f, -4.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, -0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_cubeId);


  //CUBE 20 plateforme
  cubes[19] = init20Cube();
  gl4duLoadIdentityf();

  gl4duTranslatef(-cubes[19].x, -cubes[19].y , -cubes[19].z);
    // X(largeur) Y(hauteur)  Z(devant/ derriere ) 
  gl4duScalef(cubes[19].width,  cubes[19].height, cubes[19].depth);

  //gl4duRotatef(-90.0f, 1.0f, 0.0f, -4.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, -0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_cubeId);


  //CUBE 21 plateforme
  cubes[20] = init21Cube();
  gl4duLoadIdentityf();

  gl4duTranslatef(-cubes[20].x, -cubes[20].y , -cubes[20].z);
    // X(largeur) Y(hauteur)  Z(devant/ derriere ) 
  gl4duScalef(cubes[20].width,  cubes[20].height, cubes[20].depth);

  //gl4duRotatef(-90.0f, 1.0f, 0.0f, -4.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, -0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_cubeId);


  //CUBE 22 plateforme
  cubes[21] = init22Cube();
  gl4duLoadIdentityf();

  gl4duTranslatef(-cubes[21].x, -cubes[21].y , -cubes[21].z);
    // X(largeur) Y(hauteur)  Z(devant/ derriere ) 
  gl4duScalef(cubes[21].width,  cubes[21].height, cubes[21].depth);

  //gl4duRotatef(-90.0f, 1.0f, 0.0f, -4.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, -0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_cubeId);


  //CUBE 23 plateforme
  cubes[22] = init23Cube();
  gl4duLoadIdentityf();

  gl4duTranslatef(-cubes[22].x, -cubes[22].y , -cubes[22].z);
    // X(largeur) Y(hauteur)  Z(devant/ derriere ) 
  gl4duScalef(cubes[22].width,  cubes[22].height, cubes[22].depth);

  //gl4duRotatef(-90.0f, 1.0f, 0.0f, -4.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, -0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_cubeId);


  //CUBE 24 plateforme
  cubes[23] = init24Cube();
  gl4duLoadIdentityf();

  gl4duTranslatef(-cubes[23].x, -cubes[23].y , -cubes[23].z);
    // X(largeur) Y(hauteur)  Z(devant/ derriere ) 
  gl4duScalef(cubes[23].width,  cubes[23].height, cubes[23].depth);

  //gl4duRotatef(-90.0f, 1.0f, 0.0f, -4.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, -0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_cubeId);


  //CUBE 25 plateforme
  cubes[24] = init25Cube();
  gl4duLoadIdentityf();

  gl4duTranslatef(-cubes[24].x, -cubes[24].y , -cubes[24].z);
    // X(largeur) Y(hauteur)  Z(devant/ derriere ) 
  gl4duScalef(cubes[24].width,  cubes[24].height, cubes[24].depth);

  //gl4duRotatef(-90.0f, 1.0f, 0.0f, -4.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, -0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, jaune);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_cubeId);


  //CUBE 26 plateforme
  cubes[25] = init26Cube();
  gl4duLoadIdentityf();

  gl4duTranslatef(-cubes[25].x, -cubes[25].y , -cubes[25].z);
    // X(largeur) Y(hauteur)  Z(devant/ derriere ) 
  gl4duScalef(cubes[25].width,  cubes[25].height, cubes[25].depth);

  //gl4duRotatef(-90.0f, 1.0f, 0.0f, -4.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, -0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, bleu);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_cubeId);




  //CUBE LAVE plateforme
  cubes[26] = init27Cube();
  gl4duLoadIdentityf();

  gl4duTranslatef(-cubes[26].x, -cubes[26].y , -cubes[26].z);
    // X(largeur) Y(hauteur)  Z(devant/ derriere ) 
  gl4duScalef(cubes[26].width,  cubes[26].height, cubes[26].depth);

  //gl4duRotatef(-90.0f, 1.0f, 0.0f, -4.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, -0.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "scolor"), 1, orange);
  glUniform4fv(glGetUniformLocation(_pId, "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lumpos);
  gl4dgDraw(_cubeId);




  glUseProgram(0);
}




void keyup(int keycode) {
  if(keycode == GL4DK_LSHIFT || keycode == GL4DK_RSHIFT)
    _shift = 0;
}



void keydown(int keycode) {
  switch(keycode) {
    case GL4DK_UP:
      // Calcul des nouvelles positions de la camera
      float forwardX = sin(_rota_camera * (M_PI / 180.0));
      float forwardZ = -cos(_rota_camera * (M_PI / 180.0));
      float newX = _x_camera - forwardX * 1.0f;
      float newZ = _z_camera - forwardZ * 1.0f;

        
      bool collisionDetected = false;
      printf("Position actuelle de la caméra : (%f, %f, %f)\n", _x_camera, _y_camera, _z_camera);

      /*
        // Calcul des limites du cube avec une marge de 0.3
        float cube_left = cube.x - cube.width - cube.width /2;
        float cube_right = cube.x + cube.width + cube.width /2;
        float cube_front = cube.z + cube.depth + cube.depth/2;
        float cube_back = cube.z - cube.depth - cube.depth/2;
        float cube_top = cube.y + cube.height + cube.height/2;
        float cube_bottom = cube.y - cube.height - cube.height/2;

        // Affichage des valeurs pour débogage
        printf("Cube %d - cube_left : %f\n", i + 1, cube_left);
        printf("Cube %d - cube_right : %f\n", i + 1, cube_right);
        printf("Cube %d - cube_front : %f\n", i + 1, cube_front);
        printf("Cube %d - cube_back : %f\n", i + 1, cube_back);
        printf("Cube %d - cube_top : %f\n", i + 1, cube_top);
        printf("Cube %d - cube_bottom : %f\n", i + 1, cube_bottom);
        
        */


      if (newX >= cubes[0].x - cubes[0].width * 1.1 && newX <= cubes[0].x + cubes[0].width * 1.1 &&
          newZ <= cubes[0].z + cubes[0].depth * 1.1 && newZ >= cubes[0].z - cubes[0].depth * 1.1 &&
          _y_camera >= cubes[0].y - cubes[0].height / 2 && _y_camera <= cubes[0].y + cubes[0].height / 2) {
          // Collision détectée avec le premier cube
          //printf("Collision détectée avec le premier cube.\n");
          collisionDetected = true;
      }

      if (newX >= cubes[1].x - cubes[1].width * 1.1 && newX <= cubes[1].x + cubes[1].width * 1.1 &&
          newZ <= cubes[1].z + cubes[1].depth * 1.1 && newZ >= cubes[1].z - cubes[1].depth * 1.1 &&
          _y_camera >= cubes[1].y - cubes[1].height / 2 && _y_camera <= cubes[1].y + cubes[1].height / 2) {
          // Collision détectée avec le deuxième cube
          //printf("Collision détectée avec le deuxième cube.\n");
          collisionDetected = true;
      }

      if (newX >= cubes[2].x - cubes[2].width * 1.1 && newX <= cubes[2].x + cubes[2].width * 1.1 &&
          newZ <= cubes[2].z + cubes[2].depth * 1.1 && newZ >= cubes[2].z - cubes[2].depth * 1.1 &&
          _y_camera >= cubes[2].y - cubes[2].height / 2 && _y_camera <= cubes[2].y + cubes[2].height / 2) {
          // Collision détectée avec le troisième cube
          //printf("Collision détectée avec le troisième cube.\n");
          collisionDetected = true;
      }

      if (newX >= cubes[3].x - cubes[3].width * 1.1 && newX <= cubes[3].x + cubes[3].width * 1.1 &&
          newZ <= cubes[3].z + cubes[3].depth * 1.1 && newZ >= cubes[3].z - cubes[3].depth * 1.1 &&
          _y_camera >= cubes[3].y - cubes[3].height / 2 && _y_camera <= cubes[3].y + cubes[3].height / 2) {
          // Collision détectée avec le quatrième cube
          //printf("Collision détectée avec le quatrième cube.\n");
          collisionDetected = true;
      }

      if (newX >= cubes[4].x - cubes[4].width * 1.1 && newX <= cubes[4].x + cubes[4].width * 1.1 &&
          newZ <= cubes[4].z + cubes[4].depth * 1.1 && newZ >= cubes[4].z - cubes[4].depth *1.1 &&
          _y_camera >= cubes[4].y - cubes[4].height / 2 && _y_camera <= cubes[4].y + cubes[4].height / 2) {
          // Collision détectée avec le cinquième cube
          //printf("Collision détectée avec le cinquième cube.\n");
          collisionDetected = true;
      }

      if (newX >= cubes[5].x - cubes[5].width * 1.1 && newX <= cubes[5].x + cubes[5].width * 1.1 &&
          newZ <= cubes[5].z + cubes[5].depth * 1.1 && newZ >= cubes[5].z - cubes[5].depth *1.1 &&
          _y_camera >= cubes[5].y - cubes[5].height / 2 && _y_camera <= cubes[5].y + cubes[5].height / 2) {
          // Collision détectée avec le sixième cube
          // printf("Collision détectée avec le sixième cube.\n");
          collisionDetected = true;
      }

      if (newX >= cubes[6].x - cubes[6].width * 1.1 && newX <= cubes[6].x + cubes[6].width * 1.1 &&
          newZ <= cubes[6].z + cubes[6].depth * 1.1 && newZ >= cubes[6].z - cubes[6].depth *1.1 &&
          _y_camera >= cubes[6].y - cubes[6].height / 2 && _y_camera <= cubes[6].y + cubes[6].height / 2) {
          // Collision détectée avec le 7 cube
          // printf("Collision détectée avec le 7 cube.\n");
          collisionDetected = true;
      }

      if (newX >= cubes[7].x - cubes[7].width * 1.1 && newX <= cubes[7].x + cubes[7].width * 1.1 &&
          newZ <= cubes[7].z + cubes[7].depth * 1.1 && newZ >= cubes[7].z - cubes[7].depth *1.1 &&
          _y_camera >= cubes[7].y - cubes[7].height / 2 && _y_camera <= cubes[7].y + cubes[7].height / 2) {
          // Collision détectée avec le 8 cube
          // printf("Collision détectée avec le 8 cube.\n");
          collisionDetected = true;
      }

      if (newX >= cubes[8].x - cubes[8].width * 1.1 && newX <= cubes[8].x + cubes[8].width * 1.1 &&
          newZ <= cubes[8].z + cubes[8].depth * 1.1 && newZ >= cubes[8].z - cubes[8].depth *1.1 &&
          _y_camera >= cubes[8].y - cubes[8].height / 2 && _y_camera <= cubes[8].y + cubes[8].height / 2) {
          // Collision détectée avec le 9 cube
          // printf("Collision détectée avec le 9 cube.\n");
          collisionDetected = true;
      }

      if (newX >= cubes[9].x - cubes[9].width * 1.1 && newX <= cubes[9].x + cubes[9].width * 1.1 &&
          newZ <= cubes[9].z + cubes[9].depth * 1.1 && newZ >= cubes[9].z - cubes[9].depth *1.1 &&
          _y_camera >= cubes[9].y - cubes[9].height / 2 && _y_camera <= cubes[9].y + cubes[9].height / 2) {
          // Collision détectée avec le 10 cube
          // printf("Collision détectée avec le 10 cube.\n");
          collisionDetected = true;
      }

      if (newX >= cubes[10].x - cubes[10].width * 1.1 && newX <= cubes[10].x + cubes[10].width * 1.1 &&
          newZ <= cubes[10].z + cubes[10].depth * 1.1 && newZ >= cubes[10].z - cubes[10].depth *1.1 &&
          _y_camera >= cubes[10].y - cubes[10].height / 2 && _y_camera <= cubes[10].y + cubes[10].height / 2) {
          // Collision détectée avec le 11 cube
          //printf("Collision détectée avec le 11 cube.\n");
          collisionDetected = true;
      }

      if (newX >= cubes[11].x - cubes[11].width * 1.1 && newX <= cubes[11].x + cubes[11].width * 1.1 &&
          newZ <= cubes[11].z + cubes[11].depth * 1.1 && newZ >= cubes[11].z - cubes[11].depth *1.1 &&
          _y_camera >= cubes[11].y - cubes[11].height / 2 && _y_camera <= cubes[11].y + cubes[11].height / 2) {
          // Collision détectée avec le 12 cube
          // printf("Collision détectée avec le 12 cube.\n");
          collisionDetected = true;
      }

      if (newX >= cubes[12].x - cubes[12].width * 1.1 && newX <= cubes[12].x + cubes[12].width * 1.1 &&
          newZ <= cubes[12].z + cubes[12].depth * 1.1 && newZ >= cubes[12].z - cubes[12].depth *1.1 &&
          _y_camera >= cubes[12].y - cubes[12].height / 2 && _y_camera <= cubes[12].y + cubes[12].height / 2) {
          // Collision détectée avec le 13 cube
          // printf("Collision détectée avec 13 cube.\n");
          collisionDetected = true;
      }

      if (newX >= cubes[13].x - cubes[13].width * 1.1 && newX <= cubes[13].x + cubes[13].width * 1.1 &&
          newZ <= cubes[13].z + cubes[13].depth * 1.1 && newZ >= cubes[13].z - cubes[13].depth *1.1 &&
          _y_camera >= cubes[13].y - cubes[13].height / 2 && _y_camera <= cubes[13].y + cubes[13].height / 2) {
          // Collision détectée avec le 14 cube
          // printf("Collision détectée avec le 14 cube.\n");
          collisionDetected = true;
      }

      if (newX >= cubes[14].x - cubes[14].width * 1.1 && newX <= cubes[14].x + cubes[14].width * 1.1 &&
          newZ <= cubes[14].z + cubes[14].depth * 1.1 && newZ >= cubes[14].z - cubes[14].depth *1.1 &&
          _y_camera >= cubes[14].y - cubes[14].height / 2 && _y_camera <= cubes[14].y + cubes[14].height / 2) {
          // Collision détectée avec le 15 cube
          // printf("Collision détectée avec le 15 cube.\n");
          collisionDetected = true;
      }

      if (newX >= cubes[15].x - cubes[15].width * 1.1 && newX <= cubes[15].x + cubes[15].width * 1.1 &&
          newZ <= cubes[15].z + cubes[15].depth * 1.1 && newZ >= cubes[15].z - cubes[15].depth *1.1 &&
          _y_camera >= cubes[15].y - cubes[15].height / 2 && _y_camera <= cubes[15].y + cubes[15].height / 2) {
          // Collision détectée avec le 16 cube
          // printf("Collision détectée avec le 16 cube.\n");
          collisionDetected = true;
      }

      if (newX >= cubes[16].x - cubes[16].width * 1.1 && newX <= cubes[16].x + cubes[16].width * 1.1 &&
          newZ <= cubes[16].z + cubes[16].depth * 1.1 && newZ >= cubes[16].z - cubes[16].depth *1.1 &&
          _y_camera >= cubes[16].y - cubes[16].height / 2 && _y_camera <= cubes[16].y + cubes[16].height / 2) {
          // Collision détectée avec le 17 cube
          // printf("Collision détectée avec le 17 cube.\n");
          collisionDetected = true;
      }

      if (newX >= cubes[17].x - cubes[17].width * 1.1 && newX <= cubes[17].x + cubes[17].width * 1.1 &&
          newZ <= cubes[17].z + cubes[17].depth * 1.1 && newZ >= cubes[17].z - cubes[17].depth *1.1 &&
          _y_camera >= cubes[17].y - cubes[17].height / 2 && _y_camera <= cubes[17].y + cubes[17].height / 2) {
          // Collision détectée avec le 18 cube
          // printf("Collision détectée avec le 18 cube.\n");
          collisionDetected = true;
      }

      if (newX >= cubes[18].x - cubes[18].width * 1.1 && newX <= cubes[18].x + cubes[18].width * 1.1 &&
          newZ <= cubes[18].z + cubes[18].depth * 1.1 && newZ >= cubes[18].z - cubes[18].depth *1.1 &&
          _y_camera >= cubes[18].y - cubes[18].height / 2 && _y_camera <= cubes[18].y + cubes[18].height / 2) {
          // Collision détectée avec le 19 cube
          // printf("Collision détectée avec le 19 cube.\n");
          collisionDetected = true;
      }

      if (newX >= cubes[19].x - cubes[19].width * 1.1 && newX <= cubes[19].x + cubes[19].width * 1.1 &&
          newZ <= cubes[19].z + cubes[19].depth * 1.1 && newZ >= cubes[19].z - cubes[19].depth *1.1 &&
          _y_camera >= cubes[19].y - cubes[19].height / 2 && _y_camera <= cubes[19].y + cubes[19].height / 2) {
          // Collision détectée avec le 20 cube
          // printf("Collision détectée avec le 20 cube.\n");
          collisionDetected = true;
      }

      if (newX >= cubes[20].x - cubes[20].width * 1.1 && newX <= cubes[20].x + cubes[20].width * 1.1 &&
          newZ <= cubes[20].z + cubes[20].depth * 1.1 && newZ >= cubes[20].z - cubes[20].depth *1.1 &&
          _y_camera >= cubes[20].y - cubes[20].height / 2 && _y_camera <= cubes[20].y + cubes[20].height / 2) {
          // Collision détectée avec le 21 cube
          // printf("Collision détectée avec le 21 cube.\n");
          collisionDetected = true;
      }

      if (newX >= cubes[21].x - cubes[21].width * 1.1 && newX <= cubes[21].x + cubes[21].width * 1.1 &&
        newZ <= cubes[21].z + cubes[21].depth * 1.1 && newZ >= cubes[21].z - cubes[21].depth *1.1 &&
        _y_camera >= cubes[21].y - cubes[21].height / 2 && _y_camera <= cubes[21].y + cubes[21].height / 2) {
        // Collision détectée avec le 22 cube
        // printf("Collision détectée avec le 22 cube.\n");
        collisionDetected = true;
      }

      if (newX >= cubes[22].x - cubes[22].width * 1.1 && newX <= cubes[22].x + cubes[22].width * 1.1 &&
          newZ <= cubes[22].z + cubes[22].depth * 1.1 && newZ >= cubes[22].z - cubes[22].depth *1.1 &&
          _y_camera >= cubes[22].y - cubes[22].height / 2 && _y_camera <= cubes[22].y + cubes[22].height / 2) {
          // Collision détectée avec le 23 cube
          // printf("Collision détectée avec le 23 cube.\n");
          collisionDetected = true;
      }

      if (newX >= cubes[23].x - cubes[23].width * 1.1 && newX <= cubes[23].x + cubes[23].width * 1.1 &&
          newZ <= cubes[23].z + cubes[23].depth * 1.1 && newZ >= cubes[23].z - cubes[23].depth *1.1 &&
          _y_camera >= cubes[23].y - cubes[23].height / 2 && _y_camera <= cubes[23].y + cubes[23].height / 2) {
          // Collision détectée avec le 24 cube
          // printf("Collision détectée avec le sixième cube.\n");
          collisionDetected = true;
      }

      if (newX >= cubes[24].x - cubes[24].width * 1.1 && newX <= cubes[24].x + cubes[24].width * 1.1 &&
          newZ <= cubes[24].z + cubes[24].depth * 1.1 && newZ >= cubes[24].z - cubes[24].depth *1.1 &&
          _y_camera >= cubes[24].y - cubes[24].height / 2 && _y_camera <= cubes[24].y + cubes[24].height / 2) {
          // Collision détectée avec le 25 cube
          // printf("Collision détectée avec le 25 cube.\n");
          collisionDetected = true;
      }
        if (newX >= cubes[25].x - cubes[25].width * 1.1 && newX <= cubes[25].x + cubes[25].width * 1.1 &&
          newZ <= cubes[25].z + cubes[25].depth * 1.1 && newZ >= cubes[25].z - cubes[25].depth *1.1 &&
          _y_camera >= cubes[25].y - cubes[25].height / 2 && _y_camera <= cubes[25].y + cubes[25].height / 2) {
          // Collision détectée avec le 26 cube
          // printf("Collision détectée avec le 26 cube.\n");
          collisionDetected = true;
      }

      if (newX >= cubes[26].x - cubes[26].width * 1.1 && newX <= cubes[26].x + cubes[26].width * 1.1 &&
          newZ <= cubes[26].z + cubes[26].depth * 1.1 && newZ >= cubes[26].z - cubes[26].depth *1.1 &&
          _y_camera >= cubes[26].y - cubes[26].height / 2 && _y_camera <= cubes[26].y + cubes[26].height / 2) {
          // Collision détectée avec le 26 cube
          // printf("Collision détectée avec le 26 cube.\n");
          collisionDetected = true;
      }



      if (!collisionDetected ) {
          // Mise à jour de la position de la caméra si aucune collision n'est détectée
          _x_camera = newX;
          _z_camera = newZ;
      }
    
    break;

    case GL4DK_DOWN:
      // Calcul de la direction vers l'arrière (inversion des composantes)
      float backwardX = -sin(_rota_camera * (M_PI / 180.0));
      float backwardZ = cos(_rota_camera * (M_PI / 180.0));
      _x_camera -= backwardX * 1.1f; 
      _z_camera -= backwardZ * 1.1f;
      break;

    case GL4DK_LEFT:
      _rota_camera -= 2.1f;
      break;
    case GL4DK_RIGHT:
      _rota_camera += 2.1f;
      break;

    //pour le saut 
    case ' ':
      if (!_isJumping) {
          _isJumping = GL_TRUE;
          //_jumpSpeed = 0.5f; // PUISSANCE saut
          _jumpSpeed = 0.3f;
      }
      break;


     case 'z' :
      _rotaX_camera -= 2.1f;
      break;

    case 's' :
    _rotaX_camera += 2.1f;
    break;

    break;
    default:
      break;
  }
}



void draw(void) {


  GLenum rendering = GL_COLOR_ATTACHMENT0;
  static double t0 = 0.0;
  double t = gl4dGetElapsedTime() / 1000.0, dt = t - t0;
  t0 = t;

  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  /* désactiver le rendu de couleur et ne laisser que le depth, dans _smTex */
  glDrawBuffer(GL_NONE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,    GL_TEXTURE_2D, 0, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _smTex, 0);
  /* viewport de la shadow map et dessin de la scène du point de vue de la lumière */
  glViewport(0, 0, SHADOW_MAP_SIDE, SHADOW_MAP_SIDE);
  glClear(GL_DEPTH_BUFFER_BIT);
  scene(GL_TRUE, dt);

  /* remettre GL_COLOR_ATTACHMENT0, viewport et fbo écran */
  glDrawBuffers(1, &rendering);
  glViewport(0, 0, _wW, _wH);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  


  //printf("\n  Y de la camera :  %f",_y_camera);

  // LE SAUT 
  if (_isJumping) {
    // Mise à jour  dela position verticale en fonction de la vitesse de saut
    _y_camera -= _jumpSpeed;
    _jumpSpeed -= _gravity; // Application de la gravité


    /* TEST !!!!
    // Si le joueur touche le sol, arrêt du saut
    if (_y_camera >= 1.8f) {
        _y_camera = 1.8f; // pour pas que le joueur ne descend pas en dessous du sol
        _isJumping = GL_FALSE;
        _jumpSpeed = 0.0f;
        _gravity = 0.005f; // Réactiver la gravité
    }

    */

    // Si le joueur touche le sol/la lave, arrêt du saut
    if (_y_camera >= -45.8f) {

        _x_camera = -185.0f; 
        _y_camera = -112.0f;
        _z_camera = -65.0f;  

        _isJumping = GL_FALSE;
        _jumpSpeed = 0.0f;
        _gravity = 0.005f; // Réactiver la gravité
    }



    //collision HAUT DES CUBES les variable utilisé
    bool onTopOfCube = false;
    for (int i = 0; i < MAX_CUBES; i++) {
        Cube cube = cubes[i];
        float  cube_bottom  = cube.y + cube.height + cube.height; 
        float  cube_top = cube.y - cube.height -  cube.height; 
        float cube_left = cube.x - cube.width - cube.width /2;
        float cube_right = cube.x + cube.width + cube.width /2;
        float cube_front = cube.z + cube.depth + cube.depth;
        float cube_back = cube.z - cube.depth - cube.depth;


    float top_interval = cube_top - (cube_top - cube_bottom) * 0.3; // 70% de la hauteur du haut du cube
    float bottom_interval = cube_top - (cube_top - cube_bottom) * 0.05; // 95% de la hauteur du haut du cube


    float top_interval_haute =cube_top + cube_top * (1.0/10.0) ;
    
    // Vérification de si la caméra est sur le dessus du cube
        
    if (i!=11 && i!=27) { 
      if (_x_camera >= cube_left && _x_camera <= cube_right &&
          _y_camera <= top_interval && _y_camera >= cube_top  &&
          _z_camera >= cube_back && _z_camera <= cube_front) {

          _y_camera = cube_top ; // Placer la caméra juste au-dessus du cube
          onTopOfCube = true;
          _isJumping = GL_FALSE;
          printf("Collision détectée avec la face supérieure du cube %d.\n", i + 1);
          // reactivation de la gravité après la collision
          _gravity = 0.00f;
          break;
      }
    }
    }

    // Si la caméra n'est pas sur le dessus d'un cube, reactivation de  la gravité
    if (!onTopOfCube) {
        _gravity = 0.005f;
    }
} 
else {
    // Réactivation de  la gravité si la caméra n'est pas en train de sauter
    //_gravity = 0.005f;
    _isJumping = GL_TRUE;
}


  /* vue 1 */
  gl4duBindMatrix("view");
  gl4duLoadIdentityf();
  //gl4duLookAtf(0, 1.8f, 6.0f, 0, 0, 0, 0.0f, 1.0f, 0);
  //gl4duLookAtf(_x_camera, _y_camera, _z_camera, _x_camera*2, 0, 0, 0.0f, 1.0f, 0);
  
  gl4duRotatef(_rotaX_camera, 1.0f, 0.0f, 0.0f);
  gl4duRotatef(_rota_camera, 0.0f, 1.0f, 0.0f);
  gl4duTranslatef(_x_camera, _y_camera, _z_camera);
  /* printf("Z Perso  %f",_z_camera);
  printf("Y Perso  %f",_y_camera);
  printf("X Perso  %f",_x_camera); */

  scene(GL_FALSE, dt);


/*  printf("Collision évitée. Position de la caméra : (%f, %f)\n", _x_camera, _z_camera);
    printf("Position du cube 1 : (%f, %f, %f)\n", x_cube1, y_cube1, z_cube1);
    printf("Échelle du cube 1 : (%f, %f, %f)\n", scalex_cube1, scaley_cube1, scalez_cube1);
*/
  //test pause
  if(!_pause)
    a0 += 360.0f * dt / (24.0 /* * 60.0 */);
}

void sortie(void) {
  if(_fbo) {
    glDeleteTextures(1, &_smTex);
    glDeleteFramebuffers(1, &_fbo);
    _fbo = 0;
  }
  gl4duClean(GL4DU_ALL);
}
