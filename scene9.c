/*!\file window.c
 * \brief GL4Dummies, exemple simple 3D avec GL4Dummies
 * \author Farès BELHADJ, amsi@up8.edu
 * \date jannuary 20, 2025
 */

/* inclusion des entêtes de fonctions de création et de gestion de
 * fenêtres système ouvrant un contexte favorable à GL4dummies. Cette
 * partie est dépendante de la bibliothèque SDL2 */
#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dm.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4dh.h>
#include "assimp.h"
#include "structures.h"


static void init(void);
static void draw(void);
static void quit(void);
static void viewportResize(void);

static void walls_Flash(void);
static void walls_init(int section);
static void walls_draw(void);
static void walls_quit(void);

/* on créé une variable pour stocker l'identifiant du programme GPU */
static GLuint _pId = 0;

static GLuint _wall = 0;

/* largeur et hauteur de fenêtre */
//static int _ww = 1200, _wh = 1000;

static sections_t * sections = NULL;
static int nb_sections = 30;
static float progSections = 0.0f;
static int movingWalls = 1;
static float movingSpeed = 1.0f;
static int maxMovingWalls = 12;

void scene9(int state) {
  /* INITIALISEZ VOS VARIABLES */
  /* ... */
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    init();
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    quit();
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: /* GL4DH_DRAW */
    /* JOUER L'ANIMATION */
    draw();
    return;
  }
}



/* initialise des paramètres GL et GL4D */
void init(void) {
  /* activer le test de profondeur */
  //glEnable(GL_DEPTH_TEST);

  /* on active le rendu en mode fil de fer */
  //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  /* générer un cube en GL4D */
  _wall = gl4dgGenCubef();
  /* générer un tore en GL4D */
  /* créer un programme GPU pour OpenGL (en GL4D) */
  _pId = gl4duCreateProgram("<vs>shaders/scene1.vs", "<fs>shaders/scene1.fs", NULL);
  /* créer dans GL4D une matrice qui s'appelle modview ; matrice de
     modélisation et vue qu'on retrouvera dans le vertex shader */
  gl4duGenMatrix(GL_FLOAT, "modview");
  /* créer dans GL4D une matrice qui s'appelle proj ; matrice de
     projection qu'on retrouvera dans le vertex shader */
  gl4duGenMatrix(GL_FLOAT, "proj");
  /* binder (mettre au premier plan, "en courante" ou "en active") la matrice proj */
  gl4duBindMatrix("proj");
  /* mettre la matrice identité (celle qui ne change rien) dans la matrice courante */
  // gl4duLoadIdentityf();
  // /* combiner la matrice courante avec une matrice de projection en
  //    orthogonale. Voir le support de cours pour les six paramètres :
  //    left, right, bottom, top, near, far */
  // //GLfloat ratio = _wh / (GLfloat)_ww;
  // //gl4duOrthof(-1.0f, 1.0f, -1.0f * ratio, 1.0f * ratio, 0.0f, 10.0f);
  // //gl4duFrustumf(-0.5f, 0.5f, -0.5f * ratio, 0.5f * ratio, 1.0f, 10.0f);
  //    gl4duFrustumf(-1, 1, -1, 1, 1, 1000);

  viewportResize();


  walls_init(nb_sections);
}

void draw(void) {
  viewportResize();
  /* set une couleur d'effacement random pour OpenGL */
  //glClearColor(gl4dmURand(), gl4dmURand(), gl4dmURand(), 1.0);
  GLboolean dt = glIsEnabled(GL_BLEND);
    glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  /* set la couleur d'effacement OpenGL */
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  /* effacer le buffer de couleur (image) et le buffer de profondeur d'OpenGL */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* utiliser le programme GPU "_pId" */
  glUseProgram(_pId);
  /* binder (mettre au premier plan, "en courante" ou "en active") la
     matrice modview */
  gl4duBindMatrix("modview");
  /* mettre la matrice identité (celle qui ne change rien) dans la matrice courante */
  gl4duLoadIdentityf();

   walls_draw();
   walls_Flash();

  /* n'utiliser aucun programme GPU (pas nécessaire) */
  glUseProgram(0);
  if(!dt)
   glDisable(GL_BLEND);
}

/* appelée lors du exit */
void quit(void) {
   walls_quit();
}

void walls_init(int nb_sections){
   assert(sections == NULL);
   sections = malloc(nb_sections * sizeof(*sections));
   assert(sections != NULL);

   static float angFourWalls;
   float rc = 1.0f;
   float bc = 1.0f;
   float gc = 1.0f;

   for(int i = 0; i<nb_sections; ++i){
      //transz+=0.5f;
      for(int j = 0; j<4; ++j){

         
         sections[i].walls[j].rot.angle = angFourWalls;
         sections[i].walls[j].rot.x = 0.0f;
         sections[i].walls[j].rot.y = 0.0f;
         sections[i].walls[j].rot.z = 1.0f;
         sections[i].walls[j].p.x = 3.9f;
         sections[i].walls[j].p.y = 0.0f;
         sections[i].walls[j].p.z = -40.0f;
         if(i<maxMovingWalls - 2){
            sections[i].walls[j].scale.x = 0.7f;
            sections[i].walls[j].scale.y = 4.6f;
            sections[i].walls[j].scale.z = 1.0f;
         }else{
            sections[i].walls[j].scale.x = 5.0f;
            sections[i].walls[j].scale.y = 5.0f;
            sections[i].walls[j].scale.z = 1.0f;
         }


         angFourWalls += 90.0f;
      }
      sections[i].active = 0;
      sections[i].color[0] = 0.0f;
      sections[i].color[1] = 0.0f;
      sections[i].color[2] = 0.0f;
      sections[i].color[3] = 0.0f;
      rc-=0.05f;
      bc-=0.05f;
      gc-=0.05f;
   }
}



void walls_draw(void){
   for(int i = nb_sections - 1; i >= 0; --i){
      for(int j = 0; j<4; ++j){
         //gl4duLoadIdentityf();
         gl4duPushMatrix();
         gl4duRotatef(sections[i].walls[j].rot.angle, sections[i].walls[j].rot.x, sections[i].walls[j].rot.y, sections[i].walls[j].rot.z);
         gl4duTranslatef(sections[i].walls[j].p.x, sections[i].walls[j].p.y, sections[i].walls[j].p.z);
         gl4duScalef(sections[i].walls[j].scale.x, sections[i].walls[j].scale.y, sections[i].walls[j].scale.z);
         gl4duSendMatrices();
         gl4duPopMatrix();
         glUniform4fv(glGetUniformLocation(_pId, "color"), 1, sections[i].color);
         //glUniform4f(glGetUniformLocation(_pId, "color"), sections[i].color[0], sections[i].color[1], sections[i].color[2], 1.0f);
         gl4dgDraw(_wall);
      }
   }
   progSections+=0.5f;
}

void walls_Flash(void) {

  /* récup de dt */
  static double t0 = -1.0;
  if(t0 < 0.0)
    t0 = gl4dGetElapsedTime() / 1000.0;
  double t = gl4dGetElapsedTime() / 1000.0, dt = t - t0;
  t0 = t;
  /* fin récup de dt */

   int i, j, c;
   //for(i = nb_sections - 1; i >= nb_sections - movingWalls; ++i){
   for(i = 0; i<movingWalls && movingWalls < maxMovingWalls; ++i){
      if(sections[i].color[3] != 1.0f)
         sections[i].color[3] = 1.0f;
      if(sections[i].walls[0].p.z > -8.0f && !sections[i].active){
         movingWalls++;
         movingSpeed *= 1.2f;
         sections[i].active = 1;
      }
      if(i >= maxMovingWalls - 2 && sections[i].walls[0].p.z >= -4.0f){
         continue;
      }
      for(j = 0; j<4; ++j){
         sections[i].color[3] = 1.0f;
         sections[i].walls[j].p.z += (25.0f * movingSpeed) * dt;
      }
      for(c = 0; c<3; ++c){
         sections[i].color[c] += 0.03f * movingSpeed;
      }
   }
}

void walls_quit(void) {
   if(sections) {
      free(sections);
      sections = NULL;
      nb_sections = 0;
   }
}

void viewportResize(void) {
  gl4duLoadIdentityf();
  /* combiner la matrice courante avec une matrice de projection en
     orthogonale. Voir le support de cours pour les six paramètres :
     left, right, bottom, top, near, far */
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  GLfloat ratio = vp[3] / (GLfloat)vp[2];
  //gl4duOrthof(-1.0f, 1.0f, -1.0f * ratio, 1.0f * ratio, 0.0f, 10.0f);
 // gl4duFrustumf(-0.5f, 0.5f, -0.5f * ratio, 0.5f * ratio, 0.5f, 1000.0f);
  //gl4duFrustumf(-1, 1, -1, 1, 1, 1000);
    gl4duFrustumf(-1, 1, -1 * ratio, 1 * ratio, 1, 1000);

}

