/*!\file window.c
 *
 * \brief Bruit de Perlin appliqué en GPU
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date March 3 2017
 */
#include <GL4D/gl4du.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4df.h>
#include <GL4D/gl4duw_SDL2.h>

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void         init(void);
static void         keydown(int keycode);
static void         draw(void);
static void         quit(void);
/* fonctions externes dans noise.c */
extern void initNoiseTextures(void);
extern void useNoiseTextures(GLuint pid, int shift);
extern void unuseNoiseTextures(int shift);
extern void freeNoiseTextures(void);


/*!\brief identifiant de la géométrie */
static GLuint _geom = 0;
static GLuint _cubeId = 0;

static GLuint _coneId = 0;

static GLuint _torusId = 0, _terrainId = 0;
static GLuint _smTex = 0;
static GLuint _pId = 0, _smpId = 0, _fbo = 0;

static GLuint _pId_transition, _pId_lave ,_pId_TV, _pId_blanc 
=0;
 
/*!\brief identifiant des GLSL program */
static GLuint _pId_white = 0;
static GLuint _pId2 = 0;

/*!\brief arrete l'animation */
static GLuint _pause = 0;
/*!\brief coefficient de zoom */
static GLfloat _zoom = 3.0;
static GLint _blur = 0;
/*!\brief temps */
static GLfloat _temps = 0.1;



//X_camera y_camera  Zcamera ZONE TEST 
 
/*
static float _z_camera =  -999.0;

static float _y_camera = -999.0;

static float _x_camera =  -999.0;

*/


//X_camera y_camera  Zcamera LOADING


static float _z_camera =  -2100.0;

static float _y_camera = -2000.0;

static float _x_camera =  -2000.0;





//X_camera y_camera  Zcamera LABYRINTHE
/*

static float _z_camera = -0.0;

static float _y_camera = -0.8;

static float _x_camera = -1.0;
*/

//Variable de rotation de camera  
static float _rota_camera = 0; //tourner 

static float _rotaX_camera = 0; //pencher 




static GLfloat a0 = 0.0f;



static void updateCameraPosition(GLfloat dt);




/*!\brief La fonction principale créé la fenêtre d'affichage,
 * initialise GL et les données, affecte les fonctions d'événements et
 * lance la boucle principale d'affichage.
 */
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "DL4D-Earth", 0, 0, 
			 1280, 1024, SDL_WINDOW_SHOWN))
    return 1;
  init();
  atexit(quit);
  gl4duwKeyDownFunc(keydown);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}

/*!\brief initialise les paramètres OpenGL */
static void init(void) {
  initNoiseTextures();
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0f, 0.0f, 0.1f, 0.0f);
  glViewport(0, 0, 1280, 1024);
  gl4duGenMatrix(GL_FLOAT, "modelMatrix");
  
  gl4duGenMatrix(GL_FLOAT, "viewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();

  gl4duFrustumf(-0.5, 0.5, -0.5, 0.5, 1.0, 1000.0);
  gl4duBindMatrix("modelViewMatrix");
  _pId_white  = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic_blanc.fs", NULL);

  _pId2  = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic3.fs", NULL);
  
  _pId_transition = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basicTransition.fs", NULL);

  _pId_lave = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basicLAVE.fs", NULL);

  _pId_TV = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic_TV.fs", NULL);

  _pId_blanc = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic_blanc.fs", NULL);

 
  _geom = gl4dgGenTorusf(100, 20, 0.3);
  _cubeId =gl4dgGenCubef();
  //_coneId = gl4dgGenConef(20,20);
}


void keydown(int keycode) {
  switch(keycode) {
    case GL4DK_UP:
      // Calcul des nouvelles positions de la camera
      float forwardX = sin(_rota_camera * (M_PI / 180.0));
      float forwardZ = -cos(_rota_camera * (M_PI / 180.0));
      float newX = _x_camera - forwardX * 1.0f;
      float newZ = _z_camera - forwardZ * 1.0f;

        
      printf("Position actuelle de la caméra : (%f, %f, %f)\n", _x_camera, _y_camera, _z_camera);

      _x_camera = newX;
        _z_camera = newZ;

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















typedef struct {
    float x;
    float y;
    float z;
    float width;
    float height;
    float depth;
    GLuint pid;// Variable de type chaîne de caractères pour la couleur
} Cube;



// Définition des differents cubes
Cube initFirstCube() {
    Cube cube;
    cube.x = 0.0f;         
    cube.y = 0.0f;       
    cube.z = -3.0f;       
    cube.width = 3.0f;    
    cube.height = 1.0f;   
    cube.depth = 1.0f;   
      cube.pid= _pId2;
    return cube;
}




/*CREATION DE LISTE */
#define MAX_CUBES 1 // Nombre maximal de cubes
Cube cubes[MAX_CUBES]; // Tableau de cubes
int numCubes = 0; 





/* REGLE DE CREATION 

X  = vers la droite
-X = vers la gauche 
Y = vers le bas 
-Y = vers le haut 

MAIS POUR AFFICHER LES CUBE IL FAUT mettre un MOINS (-) mais le calcul des colision change pas 

*/




  static double t0 = 0.0;
/*!\brief Cette fonction dessine dans le contexte OpenGL actif.*/
static void draw(void) {
  static GLfloat a0 = 0.0;
  static Uint32 t0 = 0;
  GLfloat dt = 0.0;
  Uint32 t;
  dt = ((t = SDL_GetTicks()) - t0) / 1000.0;
  t0 = t;

  a0 += dt * 2; // VITESSE_DEPLACEMENT_CAMERA est la vitesse de déplacement de la caméra
  updateCameraPosition(dt);


  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  gl4duBindMatrix("viewMatrix");
  
  gl4duLoadIdentityf();
//l4duLookAtf(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f); 
  
  //deplacer cam
  gl4duRotatef(_rotaX_camera, 1.0f, 0.0f, 0.0f);
  gl4duRotatef(_rota_camera, 0.0f, 1.0f, 0.0f);
  gl4duTranslatef(_x_camera, _y_camera, _z_camera);

  gl4duBindMatrix("modelMatrix");
  gl4duLoadIdentityf();


  glUseProgram(_pId_white);
  gl4duPushMatrix();

  gl4duTranslatef(0, 0, -3);
  gl4duRotatef(0.2 * a0, 0, 1, 0);
  gl4duRotatef(a0, 1, 0, 0);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId_white, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_geom);
  unuseNoiseTextures(0);
  gl4duPopMatrix();




  gl4duLoadIdentityf();
  gl4duTranslatef(999, 999, 990);
  glUseProgram(_pId2);
  gl4duPushMatrix();
  glUniform1f(glGetUniformLocation(_pId2, "zoom"), 0.1);
  glUniform1f(glGetUniformLocation(_pId2, "temps"), _temps * 15);
  /* envoi de toutes les matrices stockées par GL4D */


  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);

  unuseNoiseTextures(0);
  gl4duPopMatrix();





static float decalage = 100.0f;

//          + decalage



//cube4 de la premiere salle  mur gauche (bleu)
  gl4duLoadIdentityf();
  gl4duTranslatef(-85.0f + decalage, -100.0f + decalage, -100.0f + decalage);
  gl4duScalef(2.0f, 30.0f, 70.0f);   
  glUseProgram(_pId_lave);
  gl4duPushMatrix();
  glUniform1f(glGetUniformLocation(_pId_lave, "zoom"), _zoom);
  glUniform1f(glGetUniformLocation(_pId_lave, "temps"), _temps);
  /* envoi de toutes les matrices stockées par GL4D */


  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);

  unuseNoiseTextures(0);
  gl4duPopMatrix();






//cube5  mur droite (violet)
  gl4duLoadIdentityf();

  glUseProgram(_pId_lave);
  gl4duPushMatrix();

  gl4duTranslatef(-115.0f+ decalage,   -100.0f + decalage, -105.0f + decalage);
  gl4duScalef(2.0f, 30.0f, 25.0f);


  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId_lave, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();




//cube6 orange
  gl4duLoadIdentityf();

  glUseProgram(_pId_lave);
  gl4duPushMatrix();

  gl4duTranslatef(-100.0f + decalage,   -96.0f + decalage, -100.0f + decalage);
  gl4duScalef(16.0f, 2.0f, 50.0f);


  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId_lave, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();




//cube7 MUR du fond  (rouge )nge
  gl4duLoadIdentityf();

  glUseProgram(_pId_lave);
  gl4duPushMatrix();

  gl4duTranslatef(-100.0f + decalage,   -100.0f + decalage, -129.0f + decalage);
  gl4duScalef(18.0f, 30.0f, 2.0f);


  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId_lave, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();



//cube8 MUR du fond  (vert )nge
  gl4duLoadIdentityf();

  glUseProgram(_pId_lave);
  gl4duPushMatrix();

  gl4duTranslatef(-140.0f + decalage,   -100.0f + decalage, -50.0f + decalage);
  gl4duScalef(57.0f, 30.0f, 2.0f);


  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId_lave, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();



//cube9 MUR sol jaune
  gl4duLoadIdentityf();

  glUseProgram(_pId_lave);
  gl4duPushMatrix();

  gl4duTranslatef(-151.0f + decalage,   -96.0f + decalage, -70.0f + decalage);
  gl4duScalef(35.0f, 2.0f, 20.0f);


  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId_lave, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();



//cube10 saut 1 tuto (rouge)
  gl4duLoadIdentityf();

  glUseProgram(_pId_lave);
  gl4duPushMatrix();

  gl4duTranslatef(-150.0f + decalage,   -100.0f + decalage, -70.0f + decalage);
  gl4duScalef(2.0f, 3.0f, 19.0f);


  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId_lave, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();



//cube11 saut 1 tuto (rouge)
  gl4duLoadIdentityf();

  glUseProgram(_pId_lave);
  gl4duPushMatrix();

  gl4duTranslatef(-175.0f + decalage,   -103.0f + decalage, -70.0f + decalage);
  gl4duScalef(2.0f, 7.0f, 19.0f);


  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId_lave, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();



//cube12  sol salle1 orange (on annule la collision haute)
  gl4duLoadIdentityf();

  glUseProgram(_pId_lave);
  gl4duPushMatrix();

  gl4duTranslatef(-187.0f + decalage,   -83.0f + decalage, -70.0f + decalage);
  gl4duScalef(10.1f, 27.0f, 130.0f);


  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId_lave, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();





//cube13 sol (jaune) anti gravite vertical 
  gl4duLoadIdentityf();

  glUseProgram(_pId_lave);
  gl4duPushMatrix();

  gl4duTranslatef(-185.0f + decalage,   -109.0f + decalage, -65.0f + decalage);
  gl4duScalef(10.0f, 1.5f, 15.0f);


  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId_lave, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();



//cube14 mur (bleu) vertical 
  gl4duLoadIdentityf();

  glUseProgram(_pId_lave);
  gl4duPushMatrix();

  gl4duTranslatef(-157.0f + decalage,   -100.0f + decalage, -82.0f + decalage);
  gl4duScalef(40.0f, 30.0f, 2.0f);


  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId_lave, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();





//cube15 mur salle gauche (rouge)  
  gl4duLoadIdentityf();

  glUseProgram(_pId_lave);
  gl4duPushMatrix();

  gl4duTranslatef(-273.0f + decalage,   -84.0f + decalage, -30.0f + decalage);
  gl4duScalef(77.0f, 32.0f, 2.0f);


  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId_lave, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();



//cube16 mur salle DROITE (rouge)  
  gl4duLoadIdentityf();

  glUseProgram(_pId_lave);
  gl4duPushMatrix();

  gl4duTranslatef(-273.0f + decalage,   -84.0f + decalage, -160.0f + decalage);
  gl4duScalef(77.0f, 32.0f, 2.0f);


  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId_lave, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();



//cube17 Plateforme 1
  gl4duLoadIdentityf();

  glUseProgram(_pId_lave);
  gl4duPushMatrix();

  gl4duTranslatef(-243.0f + decalage,   -55.0f + decalage, -100.0f + decalage);
  gl4duScalef(10.0f, 2.0f, 10.0f);


  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId_lave, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();

//cube18 Plateforme 2
  gl4duLoadIdentityf();

  glUseProgram(_pId_lave);
  gl4duPushMatrix();

  gl4duTranslatef(-263.0f + decalage,   -60.0f + decalage, -60.0f + decalage);
  gl4duScalef(6.0f, 2.0f, 8.0f);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId_lave, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();




//cube19 Plateforme 3
  gl4duLoadIdentityf();

  glUseProgram(_pId_blanc);
  gl4duPushMatrix();

  gl4duTranslatef(-223.0f + decalage,   -60.0f + decalage, -40.0f + decalage);
  gl4duScalef(6.0f, 2.0f, 6.0f);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId_blanc, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();



//cube20 Plateforme 4
  gl4duLoadIdentityf();

  glUseProgram(_pId_lave);
  gl4duPushMatrix();

  gl4duTranslatef(-270.0f + decalage,   -64.0f + decalage, -70.0f + decalage);
  gl4duScalef(2.0f, 2.0f, 4.0f);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId_lave, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();



//cube21 Plateforme 5
  gl4duLoadIdentityf();

  glUseProgram(_pId_lave);
  gl4duPushMatrix();

  gl4duTranslatef(-285.0f + decalage,   -72.0f + decalage, -40.0f + decalage);
  gl4duScalef(2.0f, 2.0f, 4.0f);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId_lave, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();



//cube22 Plateforme 6
  gl4duLoadIdentityf();

  glUseProgram(_pId_lave);
  gl4duPushMatrix();

  gl4duTranslatef(-285.0f + decalage,   -72.0f + decalage, -20.0f + decalage);
  gl4duScalef(1.0f, 2.0f, 2.0f);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId_lave, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();





//cube23 Plateforme 7
  gl4duLoadIdentityf();

  glUseProgram(_pId_lave);
  gl4duPushMatrix();

  gl4duTranslatef(-305.0f + decalage,   -72.0f + decalage, -0.0f + decalage);
  gl4duScalef(3.0f, 1.0f, 1.0f);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId_lave, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();




//cube24 Plateforme 8
  gl4duLoadIdentityf();

  glUseProgram(_pId_lave);
  gl4duPushMatrix();

  gl4duTranslatef(-325.0f + decalage,   -77.0f + decalage, -15.0f + decalage);
  gl4duScalef(7.0f, 1.5f, 1.0f);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId_lave, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();




//cube25 Plateforme 9
  gl4duLoadIdentityf();

  glUseProgram(_pId_lave);
  gl4duPushMatrix();

  gl4duTranslatef(-355.0f + decalage,   -65.0f + decalage, -25.0f + decalage);
  gl4duScalef(10.0f, 16.0f, 5.0f);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId_lave, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();



//cube26 Plateforme 10
  gl4duLoadIdentityf();

  glUseProgram(_pId_lave);
  gl4duPushMatrix();

  gl4duTranslatef(-357.0f + decalage,   -80.0f + decalage, -70.0f + decalage);
  gl4duScalef(10.1f, 30.0f, 90.0f);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId_lave, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();




//cube27 Plateforme 11 CUBE DE LAVE 
  gl4duLoadIdentityf();

  glUseProgram(_pId_lave);
  gl4duPushMatrix();

  gl4duTranslatef(-260.0f + decalage,   -35.0f + decalage, -40.0f + decalage);
  gl4duScalef(130.1f, 4.0f, 130.0f);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId_lave, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();









/* SEQUENCE LOADING  !!!*/
//bas L
  gl4duLoadIdentityf();

  glUseProgram(_pId2);
  gl4duPushMatrix();

  glUniform1f(glGetUniformLocation(_pId2, "zoom"), 0.1);
  glUniform1f(glGetUniformLocation(_pId2, "temps"), _temps * 15);

  gl4duTranslatef(2006.0f,   2000.0f, 1994.0f);
  gl4duScalef(7.1f, 2.0f, 2.0f);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId2, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();

// Barre L
  gl4duLoadIdentityf();

  glUseProgram(_pId2);
  gl4duPushMatrix();

  glUniform1f(glGetUniformLocation(_pId2, "zoom"), 0.1);
  glUniform1f(glGetUniformLocation(_pId2, "temps"), _temps * 15);

  gl4duTranslatef(2001.0f,   2008.0f, 1994.0f);
  gl4duScalef(2.0f, 10.0f, 2.0f);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId2, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();




// Lettre O en gauche 

  gl4duLoadIdentityf();

  glUseProgram(_pId2);
  gl4duPushMatrix();

  glUniform1f(glGetUniformLocation(_pId2, "zoom"), 0.1);
  glUniform1f(glGetUniformLocation(_pId2, "temps"), _temps * 15);

  gl4duTranslatef(2021.0f,   2008.0f, 1994.0f);
  gl4duScalef(2.0f, 10.0f, 2.0f);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId2, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();



//bas O
  gl4duLoadIdentityf();

  glUseProgram(_pId2);
  gl4duPushMatrix();

  glUniform1f(glGetUniformLocation(_pId2, "zoom"), 0.1);
  glUniform1f(glGetUniformLocation(_pId2, "temps"), _temps * 15);

  gl4duTranslatef(2026.0f,   2000.0f, 1994.0f);
  gl4duScalef(7.1f, 2.0f, 2.0f);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId2, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();



  // Lettre O en droite 

  gl4duLoadIdentityf();

  glUseProgram(_pId2);
  gl4duPushMatrix();

  glUniform1f(glGetUniformLocation(_pId2, "zoom"), 0.1);
  glUniform1f(glGetUniformLocation(_pId2, "temps"), _temps * 15);

  gl4duTranslatef(2031.0f,   2008.0f, 1994.0f);
  gl4duScalef(2.0f, 10.0f, 2.0f);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId2, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();


//bas O
  gl4duLoadIdentityf();

  glUseProgram(_pId2);
  gl4duPushMatrix();

  glUniform1f(glGetUniformLocation(_pId2, "zoom"), 0.1);
  glUniform1f(glGetUniformLocation(_pId2, "temps"), _temps * 15);

  gl4duTranslatef(2026.0f,   2017.0f, 1994.0f);
  gl4duScalef(7.1f, 2.0f, 2.0f);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId2, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();







/*LETTRE A*/
// Lettre A en gauche 

  gl4duLoadIdentityf();

  glUseProgram(_pId2);
  gl4duPushMatrix();


  gl4duTranslatef(2041.0f,   2008.0f, 1994.0f);
  gl4duScalef(2.0f, 10.0f, 2.0f);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId2, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();



//bas A
  gl4duLoadIdentityf();

  glUseProgram(_pId2);
  gl4duPushMatrix();
  
  glUniform1f(glGetUniformLocation(_pId2, "zoom"), 0.1);
  glUniform1f(glGetUniformLocation(_pId2, "temps"), _temps * 15);

  gl4duTranslatef(2046.0f,   2005.0f, 1994.0f);
  gl4duScalef(7.1f, 2.0f, 2.0f);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId2, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();



  // Lettre A en droite 

  gl4duLoadIdentityf();

  glUseProgram(_pId2);
  gl4duPushMatrix();

  glUniform1f(glGetUniformLocation(_pId2, "zoom"), 0.1);
  glUniform1f(glGetUniformLocation(_pId2, "temps"), _temps * 15);

  gl4duTranslatef(2051.0f,   2008.0f, 1994.0f);
  gl4duScalef(2.0f, 10.0f, 2.0f);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId2, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();





//bas A
  gl4duLoadIdentityf();

  glUseProgram(_pId2);
  gl4duPushMatrix();


  glUniform1f(glGetUniformLocation(_pId2, "zoom"), 0.1);
  glUniform1f(glGetUniformLocation(_pId2, "temps"), _temps * 15);

  gl4duTranslatef(2046.0f,   2017.0f, 1994.0f);
  gl4duScalef(7.1f, 2.0f, 2.0f);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId2, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();





/* LETTRE D*/
// 

// Lettre D en gauche 

  gl4duLoadIdentityf();

  glUseProgram(_pId2);
  gl4duPushMatrix();

  glUniform1f(glGetUniformLocation(_pId2, "zoom"), 0.1);
  glUniform1f(glGetUniformLocation(_pId2, "temps"), _temps * 15);

  gl4duTranslatef(2061.0f,   2008.0f, 1994.0f);
  gl4duScalef(2.0f, 10.0f, 2.0f);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId2, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();



//bas diagonal du haut 
  gl4duLoadIdentityf();

  glUseProgram(_pId2);
  gl4duPushMatrix();

  glUniform1f(glGetUniformLocation(_pId2, "zoom"), 0.1);
  glUniform1f(glGetUniformLocation(_pId2, "temps"), _temps * 15);

  gl4duTranslatef(2066.0f,   2012.0f, 1994.0f);

  gl4duRotatef(-45.0, 0.0, 0.0, 1.0);
  gl4duScalef(7.1f, 2.0f, 2.0f);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId2, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();




//bas diagonal du bas 
  gl4duLoadIdentityf();

  glUseProgram(_pId2);
  gl4duPushMatrix();

  glUniform1f(glGetUniformLocation(_pId2, "zoom"), 0.1);
  glUniform1f(glGetUniformLocation(_pId2, "temps"), _temps * 15);

  gl4duTranslatef(2066.0f,   2004.0f, 1994.0f);

  gl4duRotatef(45.0, 0.0, 0.0, 1.0);
  gl4duScalef(7.1f, 2.0f, 2.0f);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId2, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();




  // Lettre i  

  gl4duLoadIdentityf();

  glUseProgram(_pId2);
  gl4duPushMatrix();

  glUniform1f(glGetUniformLocation(_pId2, "zoom"), 0.1);
  glUniform1f(glGetUniformLocation(_pId2, "temps"), _temps * 15);

  gl4duTranslatef(2077.0f,   2008.0f, 1994.0f);
  gl4duScalef(2.0f, 10.0f, 2.0f);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId2, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();




/*LETTRE N*/
// Lettre N en gauche 

  gl4duLoadIdentityf();

  glUseProgram(_pId2);
  gl4duPushMatrix();

  glUniform1f(glGetUniformLocation(_pId2, "zoom"), 0.1);
  glUniform1f(glGetUniformLocation(_pId2, "temps"), _temps * 15);

  gl4duTranslatef(2087.0f,   2008.0f, 1994.0f);
  gl4duScalef(2.0f, 10.0f, 2.0f);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId2, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();


//bas N barre oblique  
  gl4duLoadIdentityf();

  glUseProgram(_pId2);
  gl4duPushMatrix();

  glUniform1f(glGetUniformLocation(_pId2, "zoom"), 0.1);
  glUniform1f(glGetUniformLocation(_pId2, "temps"), _temps * 15);

  gl4duTranslatef(2093.0f,   2010.0f, 1994.0f);

  gl4duRotatef(-45.0, 0.0, 0.0, 1.0);
  gl4duScalef(9.1f, 2.5f, 2.0f);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId2, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();




// Lettre N en droite  
  gl4duLoadIdentityf();

  glUseProgram(_pId2);
  gl4duPushMatrix();

  glUniform1f(glGetUniformLocation(_pId2, "zoom"), 0.1);
  glUniform1f(glGetUniformLocation(_pId2, "temps"), _temps * 15);

  gl4duTranslatef(2100.0f,   2008.0f, 1994.0f);
  gl4duScalef(2.0f, 10.0f, 2.0f);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId2, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();



// Lettre APPOSTROPHE   
  gl4duLoadIdentityf();

  glUseProgram(_pId2);
  gl4duPushMatrix();

  glUniform1f(glGetUniformLocation(_pId2, "zoom"), 0.1);
  glUniform1f(glGetUniformLocation(_pId2, "temps"), _temps * 2);

  gl4duTranslatef(2110.0f,   2008.0f, 1994.0f);

  gl4duRotatef(-20.0, 0.0, 0.0, 1.0);
  gl4duScalef(2.0f, 5.0f, 2.0f);

  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId2, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();




  if(!_pause)
    _temps += dt / 50.0;
  a0 += 360.0 * dt / (24.0 /* * 60.0 */);

  gl4dfBlur (0, 0, _blur, 1, 0, GL_FALSE);
  /* gl4dfSobelSetResultMode(GL4DF_SOBEL_RESULT_RGB); */
  gl4dfSobelSetMixMode(GL4DF_SOBEL_MIX_MULT);
  gl4dfSobel (0, 0, GL_FALSE);
}





/* POUR LAVE MEILLEUR COMME CA 

//cube5  mur droite (violet)
  gl4duLoadIdentityf();

  glUseProgram(_pId_lave);
  gl4duPushMatrix();

  gl4duTranslatef(-115.0f+ decalage,   -100.0f + decalage, -105.0f + decalage);
  gl4duScalef(2.0f, 30.0f, 25.0f);


  useNoiseTextures(_pId_lave, 0);
  gl4duSendMatrices();
  gl4dgDraw(_cubeId);
  unuseNoiseTextures(0);
  gl4duPopMatrix();



*/

void updateCameraPosition(GLfloat dt) {
    // Logique pour mettre à jour la position de la caméra automatiquement
    // Utilisez les variables globales _x_camera, _y_camera, _z_camera, _rota_camera, _rotaX_camera
    // en fonction du temps écoulé (dt)
    // Exemple :
 

 /*
    
    if (t0 >= 0.5 && t0 < 15.5) {
    printf("TEEEEEEMPS");
      //X_camera y_camera  Zcamera
      _z_camera =  -2250.0;

      _y_camera = -2010.0;

      _x_camera =  -2000.0;

    }


    */




     if (t0 >= 25.5 && t0 < 27) {
      printf("TEEEEEEMPS");
       _y_camera -= 1.3f;

    }

/*
    if (t0 >= 16.0 && t0 < 18.0) {
      printf("TEEEEEEMPS");
       _rotaX_camera += 10.0f * dt; 
       _z_camera += 20.0f * dt;
    }
    if (t0 >= 16.0 && t0 < 18.0) {
      printf("TEEEEEEMPS");
       _rotaX_camera += 10.0f * dt; 
       _z_camera += 20.0f * dt;
       _rota_camera -= 50.0f * dt;
    }
    if (t0 >= 18.0 && t0 < 20.0) {
      printf("TEEEEEEMPS");
       _rota_camera -= 50.0f * dt;
       _x_camera -= 10.0f * dt; 
    }

    if (t0 >= 20.0 && t0 < 20.5) {
      printf("TEEEEEEMPS");
       _rota_camera += 50.0f * dt;
       _x_camera -= 10.0f * dt; 
    }
    if (t0 >= 20.5 && t0 < 26) {
      printf("TEEEEEEMPS");
       _x_camera -= 10.0f * dt; 
    }
    if (t0 >= 26 && t0 < 28.0) {
      printf("TEEEEEEMPS");
       _y_camera -= 10.0f;
      
       _x_camera -= 10.0f * dt; 
    }
    if (t0 >= 30 && t0 < 33.5) {
      printf("TEEEEEEMPS");
       _y_camera -= 140.0f * dt; 
      
       _x_camera -= 20.0f * dt; 
    }


    if (t0 >= 34 && t0 < 36) {
      printf("TEEEEEEMPS");
       _x_camera -= 20.0f * dt; 
       _rotaX_camera += 10.0f;
    }
*/







    // Mettez à jour les autres variables de caméra si nécessaire

    // Mettez à jour le temps initial
    t0 += dt;
}





/*!\brief appelée au moment de sortir du programme (atexit), libère les éléments utilisés */
static void quit(void) {
  freeNoiseTextures();
  gl4duClean(GL4DU_ALL);
}
