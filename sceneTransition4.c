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
#include <SDL_image.h>
#include <unistd.h>
#include "structures.h"


static void init(void);
static void draw(void);
static void quit(void);
static void viewportResize(void);

/* créer un tableau de GLuint pour stocker des identifiants de texture */
static GLuint _tex[1] = {0};


static void walls_Move(void);
static void walls_init(int section);
static void walls_draw(void);
static void walls_quit(void);

/* on créé une variable pour stocker l'identifiant du programme GPU */
static GLuint _pId = 0;

static GLuint _wall = 0;

/* largeur et hauteur de fenêtre */
//static int _ww = 1200, _wh = 1000;

static sections_t * sections = NULL;
static int nb_sections = 37;
static double fps = 19.0f;


void sceneTransition4(int state) {
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
  _pId = gl4duCreateProgram("<vs>shaders/scene2.vs", "<fs>shaders/scene2.fs", NULL);
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

  /* générer les identifiants de texture et les stocker dans _tex */
  glGenTextures(1, _tex);
  /* on active la texture _tex[0] */
  glBindTexture(GL_TEXTURE_2D, _tex[0]);
  /* paramétrage de la texture */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  /* fin paramétrage */
  /* charger l'image depuis un fichier */
  SDL_Surface * src = IMG_Load("images/image3.tif");
  assert(src);
  fprintf(stderr, "%d %d\n", src->format->BitsPerPixel, src->format->BytesPerPixel);
  /* transfert de l'image depuis CPU vers GPU */
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, src->w, src->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, src->pixels);
  SDL_FreeSurface(src);


  /* on désactive la texture courante */
  //glBindTexture(GL_TEXTURE_2D, 0);


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


   /* on active l'étage de texture 0 (par défaut) */
   glActiveTexture(GL_TEXTURE0);
    /* on active la texture _tex[0] */
  glBindTexture(GL_TEXTURE_2D, _tex[0]);

  /* binder (mettre au premier plan, "en courante" ou "en active") la
     matrice modview */
  gl4duBindMatrix("modview");

  /* set le uniform tex_scale (dans le vertex shader) à 1 */
  glUniform1f( glGetUniformLocation(_pId, "tex_scale"), 1.0f );

   const GLfloat Lp[] = { 0.0f, 0.0f, -6.0f, 1.0f };
   glUniform4fv(glGetUniformLocation(_pId, "Lp"), 1, Lp);

  /* mettre la matrice identité (celle qui ne change rien) dans la matrice courante */
  gl4duLoadIdentityf();

 
   walls_draw();
   walls_Move();

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
   float transz = -30.0f;
   float rc = 1.0f;
   float bc = 1.0f;
   float gc = 1.0f;

   for(int i = 0; i<nb_sections; ++i){
      //transz+=0.5f;
      transz-=1.5f;
      angFourWalls = i* 4.0f;
      for(int j = 0; j<4; ++j){
         
         sections[i].walls[j].rot.angle = angFourWalls;
         sections[i].walls[j].rot.x = 0.0f;
         sections[i].walls[j].rot.y = 0.0f;
         sections[i].walls[j].rot.z = 1.0f;
         sections[i].walls[j].p.x = 3.9f;
         sections[i].walls[j].p.y = 0.0f;
         sections[i].walls[j].p.z = transz;
         sections[i].walls[j].scale.x = 0.7f;
         sections[i].walls[j].scale.y = 4.5f;
         sections[i].walls[j].scale.z = 1.0f;

         angFourWalls += 90.0f;
      }
      sections[i].color[0] = rc;
      sections[i].color[1] = gc;
      sections[i].color[2] = bc;
      sections[i].color[3] = 1.0f;
      rc-=0.05f;
      bc-=0.05f;
      gc-=0.05f;
   }
}



void walls_draw(void){
   for(int i = nb_sections - 1; i >= 0; --i){
      if (sections[i].walls[0].p.z < 0.0f){
      for(int j = 0; j<4; ++j){
         //gl4duLoadIdentityf();
         /* set le uniform couleur (dans le frag shader) à blanc */
         //glUniform4f( glGetUniformLocation(_pId, "color"), 1.0f, 1.0f, 1.0f, 1.0f );

         gl4duPushMatrix();
         gl4duRotatef(sections[i].walls[j].rot.angle, sections[i].walls[j].rot.x, sections[i].walls[j].rot.y, sections[i].walls[j].rot.z);
         gl4duTranslatef(sections[i].walls[j].p.x, sections[i].walls[j].p.y, sections[i].walls[j].p.z);
         gl4duScalef(sections[i].walls[j].scale.x, sections[i].walls[j].scale.y, sections[i].walls[j].scale.z);
         /* set le uniform tex_scale (dans le vertex shader) à la taille du scale */
         glUniform1f( glGetUniformLocation(_pId, "tex_scale"), 1.0f );
         gl4duSendMatrices();
         gl4duPopMatrix();
         //glUniform4fv(glGetUniformLocation(_pId, "color"), 1, sections[i].color);
         glUniform4f(glGetUniformLocation(_pId, "color"), 1.0f, 1.0f, 1.0f, 1.0f);
         
         glBindTexture(GL_TEXTURE_2D, _tex[0]);
         //glUniform4f(glGetUniformLocation(_pId, "color"), sections[i].color[0], sections[i].color[1], sections[i].color[2], alpha);
         gl4dgDraw(_wall);
           /* on désactive la texture */
         glBindTexture(GL_TEXTURE_2D, 0);
      }
      }
   }
   
}

void walls_Move(void) {
    /* récup de dt */
  static double t0 = -1.0;
  if(t0 < 0.0)
    t0 = gl4dGetElapsedTime() / 1000.0;
  double t = gl4dGetElapsedTime() / 1000.0, dt = t - t0;
  t0 = t;
  /* fin récup de dt */
  
   static float v = 0.005f;
   for(int i = 0; i<nb_sections; ++i){
      for(int j = 0; j<4; ++j){
	sections[i].walls[j].p.z += (fps * v) * dt; 
      }
      if(v < 0.5f)
            v+=0.005f;
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
  //gl4duFrustumf(-0.5f, 0.5f, -0.5f * ratio, 0.5f * ratio, 0.5f, 1000.0f);
  //gl4duFrustumf(-1, 1, -1, 1, 1, 1000);
    gl4duFrustumf(-1, 1, -1 * ratio, 1 * ratio, 1, 1000);

}

