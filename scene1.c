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
#include <unistd.h>
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

sections_t * sections = NULL;
static int nb_sections = 30;
static float progSections = 0.0f;
static int currentWallReveal = 0;
static double fps = 22.0f;


void scene1(int state) {
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
  
  /* désactiver la synchronisation verticale */

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
//   gl4duLoadIdentityf();
//   //float ratio = _wh / (float)_ww;
//   /* combiner la matrice courante avec une matrice de projection en
//      perspective. Voir le support de cours pour les six paramètres :
//      left, right, bottom, top, near, far */
//    gl4duFrustumf(-1, 1, -1, 1, 1, 1000);
   viewportResize();
  /* si vous souhitez une projection orthogonale */
   //gl4duOrthof(-2, 2, -2 * ratio, 2 * ratio, 1, 1000);

  walls_init(nb_sections);
}

void draw(void) {
   viewportResize();
  GLboolean dt = glIsEnabled(GL_BLEND);
  /* set une couleur d'effacement random pour OpenGL */
  //glClearColor(gl4dmURand(), gl4dmURand(), gl4dmURand(), 1.0);

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
  /* nettoyer (libérer) tout objet créé avec GL4D */
}

void walls_init(int nb_sections){
   assert(sections == NULL);
   sections = malloc(nb_sections * sizeof(*sections));
   assert(sections != NULL);

   static float angFourWalls;
   float transz = -1.5f;
   float rc = 1.0f;
   float bc = 1.0f;
   float gc = 1.0f;

   for(int i = 0; i<nb_sections; ++i){
      //transz+=0.5f;
      transz-=1.5f;
      for(int j = 0; j<4; ++j){

         
         sections[i].walls[j].rot.angle = angFourWalls;
         sections[i].walls[j].rot.x = 0.0f;
         sections[i].walls[j].rot.y = 0.0f;
         sections[i].walls[j].rot.z = 1.0f;
         sections[i].walls[j].p.x = 3.9f;
         sections[i].walls[j].p.y = 0.0f;
         sections[i].walls[j].p.z = transz;
         sections[i].walls[j].scale.x = 0.7f;
         sections[i].walls[j].scale.y = 4.6f;
         sections[i].walls[j].scale.z = 1.0f;

         angFourWalls += 90.0f;
      }
      sections[i].color[0] = 1.0f;
      sections[i].color[1] = 1.0f;
      sections[i].color[2] = 1.0f;
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
         //glUniform4f(glGetUniformLocation(_pId, "color"), sections[i].color[0], sections[i].color[1], sections[i].color[2], alpha);
         gl4dgDraw(_wall);
      }
   }
   progSections+=0.5f;
}

void walls_Flash(void) {
   static int reverseFlash = 0;
   static int flashDone = 0;

   /* bloc de ralentissement */
   static double lastReveal = -1.0;
   const double freq = 1.0 / fps;
   double t = gl4dGetElapsedTime() / 1000.0, dt = t - lastReveal;
   if(dt >= freq)
     lastReveal = t;
   else
     return;
   /* fin bloc de ralentissement */
   
   if(!flashDone){
      if(!reverseFlash){
         if(currentWallReveal - 1 >= 0 && currentWallReveal != nb_sections){
            sections[currentWallReveal - 1].color[3] = 0.0f;
         }
         if(currentWallReveal < nb_sections){
            sections[currentWallReveal].color[3] = 1.0f;
            currentWallReveal++;
            if(currentWallReveal >= nb_sections){
               reverseFlash = 1;
               currentWallReveal = nb_sections - 1;
            }
         }
      }else{
         if(currentWallReveal + 1 < nb_sections){
            sections[currentWallReveal + 1].color[3] = 0.0f;
         }
         if(currentWallReveal >= 0){
            sections[currentWallReveal].color[3] = 1.0f;
            currentWallReveal--;
         }else{
            flashDone = 1;
         }
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
    gl4duFrustumf(-1, 1, -1 * ratio, 1 * ratio, 1, 1000);
  //gl4duFrustumf(-0.5f, 0.5f, -0.5f * ratio, 0.5f * ratio, 0.5f, 1000.0f);
  //gl4duFrustumf(-1, 1, -1, 1, 1, 1000);
}

