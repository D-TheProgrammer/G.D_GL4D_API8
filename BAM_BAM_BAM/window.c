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
/*!\brief identifiant des GLSL program */
static GLuint _pId = 0;

static GLuint _pId2 = 0;
static GLuint _texId = 0;

/*!\brief arrete l'animation */
static GLuint _pause = 0;
/*!\brief coefficient de zoom */
static GLfloat _zoom = 3.0;
static GLint _blur = 0;
/*!\brief temps */
static GLfloat _temps = 0.1;

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

  GLuint p[] = {0, -1, -1, (255 << 24) | 255 };
  initNoiseTextures();
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0f, 0.0f, 0.1f, 0.0f);
  glViewport(0, 0, 1280, 1024);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");


   gl4duGenMatrix(GL_FLOAT, "model");
  gl4duGenMatrix(GL_FLOAT, "view");

  gl4duGenMatrix(GL_FLOAT, "proj");


  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5, 0.5, 1.0, 1000.0);
  gl4duBindMatrix("modelViewMatrix");
  _pId  = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  _pId2  = gl4duCreateProgram("<vs>shaders/hello.vs", "<fs>shaders/hello.fs", NULL);
  
  _geom = gl4dgGenTorusf(100, 20, 0.3);


  glGenTextures(1, &_texId);
  assert(_texId);
  glBindTexture(GL_TEXTURE_2D, _texId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, p);
  glBindTexture(GL_TEXTURE_2D, 0);


}

static void keydown(int keycode) {
  switch(keycode) {
  case SDLK_DOWN:
    _zoom -= 0.1;
    break;
  case SDLK_UP:
    _zoom += 0.1;
    break;
  case SDLK_p:
    _blur += 1;
    break;
  case SDLK_m:
    _blur -= 1;
    if(_blur < 0)
      _blur = 0;
    break;
  case ' ':
    _pause = !_pause;
    break;
  case SDLK_ESCAPE:
  case 'q':
    exit(0);
  default:
    break;
  }
}

/*!\brief Cette fonction dessine dans le contexte OpenGL actif.*/
static void draw(void) {
  static const GLfloat rouge[] = {0.6f, 0.0f, 0.0f, 1.0f}, bleu[] = {0.0f, 0.0f, 0.6f, 1.0f};
  static GLfloat a = 0.0f;
  GLfloat lumpos[] = {-4.0f, 4.0f, 0.0f, 1.0f};
  lumpos[1] = 2.0f + 1.9f * sin(a);




  static GLfloat a0 = 0.0;
  static Uint32 t0 = 0;
  GLfloat dt = 0.0;
  Uint32 t;
  dt = ((t = SDL_GetTicks()) - t0) / 1000.0;
  t0 = t;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  gl4duTranslatef(0, 0, -7);
  glUseProgram(_pId2);
  gl4duPushMatrix();
  gl4duRotatef(0.2 * a0, 0, 1, 0);
  gl4duRotatef(a0, 1, 0, 0);
  glUniform4fv(glGetUniformLocation(_pId2, "couleur") , 1, rouge);
  /* envoi de toutes les matrices stockées par GL4D */
  useNoiseTextures(_pId2, 0);
  gl4duSendMatrices();
  /* dessiner une première fois la sphère avec le _pId */
  gl4dgDraw(_geom);
  unuseNoiseTextures(0);
  gl4duPopMatrix();


  gl4duBindMatrix("model");
  glUseProgram(_pId2);
  gl4duLoadIdentityf();

  gl4duTranslatef(0, 0, -3);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  gl4duScalef(5.0f, 5.0f, 5.0f);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId2, "couleur") , 1, rouge);
//truc qui fait le truc de depart 
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _texId);
  glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
  glUniform1i(glGetUniformLocation(_pId, "use_tex"), 1);

  gl4dgDraw(_geom);


  if(!_pause)
    _temps += dt / 50.0;
  a0 += 360.0 * dt / (24.0 /* * 60.0 */);

  gl4dfBlur (0, 0, _blur, 1, 0, GL_FALSE);
  /* gl4dfSobelSetResultMode(GL4DF_SOBEL_RESULT_RGB); */
  gl4dfSobelSetMixMode(GL4DF_SOBEL_MIX_MULT);
  gl4dfSobel (0, 0, GL_FALSE);
}

/*!\brief appelée au moment de sortir du programme (atexit), libère les éléments utilisés */
static void quit(void) {
  freeNoiseTextures();
  gl4duClean(GL4DU_ALL);
}
