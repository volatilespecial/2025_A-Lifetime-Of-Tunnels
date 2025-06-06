/*!\file window.c
 * \brief GL4Dummies, exemple de physique
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
#include "assimp.h"
#include "structures.h"

static void init(void);
static void draw(void);
static void quit(void);
static void viewportResize(void);

static void mobile_init(int n);
static void mobile_simu();
static void mobile_draw(GLuint pId, GLuint * _models, int nb_models);
static void mobile_quit(void);

static GLuint _tex[1] = {0};

/* on créé une variable pour stocker l'identifiant du programme GPU */
static GLuint _pId = 0;
static GLuint _plan = 0;
static GLuint _pave = 0;

/* on créé une variable pour stocker l'identifiant de la géométrie d'un modèle chargé */
static int nb_models = 4;
static int _nb_pave = 7;
static double fps = 22.0f;

static model_t * models = NULL;
static model_t * palmtrees = NULL;
static model_t * _paveStruct = NULL;

static GLuint *_models = NULL;

//static int _ww = 800, _wh = 800;

void scene7(int state) {
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
  /*glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);*/
  /* désactiver le blending */
  //glEnable(GL_BLEND);
  /* commentée pour l'instant */
  /* charger un modèle depuis un fichier */

   _models = malloc(nb_models * sizeof(*_models));
   assert(_models);

   _models[0] = assimpGenScene("models/palmtree/scene.gltf");
   _models[1] = assimpGenScene("models/chair/scene.gltf");
   _models[2] = assimpGenScene("models/ball/scene.gltf");
   _models[3] = assimpGenScene("models/umbrella/scene.gltf");

  /* créer un programme GPU pour OpenGL (en GL4D) */
  _pId = gl4duCreateProgram("<vs>shaders/models.vs", "<fs>shaders/models.fs", NULL);

  _plan = gl4dgGenQuadf();

  _pave = gl4dgGenQuadf();
  
  /* créer dans GL4D une matrice qui s'appelle mod ; matrice de
     modélisation/vue qu'on retrouvera dans le vertex shader */
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  /* créer dans GL4D une matrice qui s'appelle proj ; matrice de
     projection qu'on retrouvera dans le vertex shader */
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  /* binder (mettre au premier plan, "en courante" ou "en active") la matrice proj */
  gl4duBindMatrix("projectionMatrix");
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

  glGenTextures(1, _tex);
  glBindTexture(GL_TEXTURE_2D, _tex[0]);

  /* paramétrage de la texture */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  /* fin paramétrage */
  // GLuint pixels[] = { 0, -1, -1, 0 };
  // /* transfert de l'image depuis CPU vers GPU */
  // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    /* on active la texture _tex[1] */
  glBindTexture(GL_TEXTURE_2D, _tex[0]);
  /* paramétrage de la texture */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  /* fin paramétrage */
  /* charger l'image depuis un fichier */
  SDL_Surface * src = IMG_Load("images/sand.tif");
  assert(src);
  fprintf(stderr, "%d %d\n", src->format->BitsPerPixel, src->format->BytesPerPixel);
  /* transfert de l'image depuis CPU vers GPU */
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, src->w, src->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, src->pixels);
  SDL_FreeSurface(src);


  /* on désactive la texture courante */
  glBindTexture(GL_TEXTURE_2D, 0);


  assert(_paveStruct == NULL);
  _paveStruct = malloc(_nb_pave * sizeof(model_t));
  assert(_paveStruct);
  float zaxis = 0.0f;
  for(int i = 0; i< _nb_pave; ++i){
    _paveStruct[i].p.x = 0.0f;
    _paveStruct[i].p.y = -50.0f;
    _paveStruct[i].p.z = zaxis;
    _paveStruct[i].rot.angle = -90.0f;
    _paveStruct[i].rot.x = 1.0f;
    _paveStruct[i].rot.y = 0.0f;
    _paveStruct[i].rot.z = 0.0f;
    _paveStruct[i].color[0] = 0.0f;
    _paveStruct[i].color[1] = 0.0f;
    _paveStruct[i].color[2] = 0.0f;
    _paveStruct[i].color[3] = 0.0f;
    zaxis += 1.3f;
  }

  mobile_init(nb_models);
}

void draw(void) {
  viewportResize();
  GLfloat lum[4] = {0.0f, 6.0f, -2.0f, 1.0f};
  GLboolean dt = glIsEnabled(GL_CULL_FACE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  /* set la couleur d'effacement OpenGL */
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glEnable(GL_CULL_FACE);
  /* effacer le buffer de couleur (image) et le buffer de profondeur d'OpenGL */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* utiliser le programme GPU "_pId" */
  glUseProgram(_pId);
 
/* on active l'étage de texture 0 (par défaut) */
  glActiveTexture(GL_TEXTURE0);
  /* on active la texture _tex[0] */
  glBindTexture(GL_TEXTURE_2D, _tex[0]);
  /* on utilise l'étage 0 (texture unit 0) */
  glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
  glUniform1i( glGetUniformLocation(_pId, "hasTexture"), 1 );
  /* set le uniform tex_scale (dans le vertex shader) à 1 */
  glUniform1f( glGetUniformLocation(_pId, "tex_scale"), 1.0f );
  /* set le uniform tex_translate (dans le vertex shader) à zéro translation */
  glUniform2f( glGetUniformLocation(_pId, "tex_translate"), 0.0f, 0.0f );
  glUniform1f( glGetUniformLocation(_pId, "shininess"), 50.0f );
  glUniform4f(glGetUniformLocation(_pId, "color"), 1.0f, 1.0f, 1.0f, 1.0f);
  glUniform4f(glGetUniformLocation(_pId, "diffuse_color"), 0.8f, 0.8f, 0.8f, 1.0f);
  glUniform4f(glGetUniformLocation(_pId, "ambient_color"), 1.0f, 1.0f, 1.0f, 1.0f);
  glUniform4f(glGetUniformLocation(_pId, "specular_color"), 1.0f, 1.0f, 1.0f, 1.0f);


  /* UN SOL */
  /* binder (mettre au premier plan, "en courante" ou "en active") la
     matrice modelView */
  gl4duBindMatrix("modelViewMatrix");
  /* mettre la matrice identité (celle qui ne change rien) dans la matrice courante */
  gl4duLoadIdentityf();
  gl4duTranslatef(0.0f, -1.0, palmtrees[0].p.z - 3.0f);
  gl4duRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  /* scale le sol */
  gl4duScalef(20.0f, 20.0f, 20.0f);
  /* envoyer les matrice GL4D au programme GPU OpenGL (en cours) */
  gl4duSendMatrices();
  
  /* set le uniform couleur (dans le frag shader) à blanc */
  glUniform4f( glGetUniformLocation(_pId, "color"), 1.0f, 1.0f, 1.0f, 1.0f );
  /* mettre la matrice identité (celle qui ne change rien) dans la matrice courante */
  /* set le uniform tex_scale (dans le vertex shader) à la taille du scale */
  glUniform1f( glGetUniformLocation(_pId, "tex_scale"), 20.0f );
  // /* set le uniform tex_translate (dans le vertex shader) à ??? */
  // glUniform2f( glGetUniformLocation(_pId, "tex_translate"), _cam.x / 2.0f, -_cam.z / 2.0f );
  /* on active la texture _tex[1] */
  glBindTexture(GL_TEXTURE_2D, _tex[0]);
  /* demander le dessin d'un objet GL4D */
  gl4dgDraw(_plan);
  //glUniform1i( glGetUniformLocation(_pId, "hasTexture"), 0);
  
 

  glUniform4fv(glGetUniformLocation(_pId, "lumpos"), 1, lum);

  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();


  for(int i = 0; i< _nb_pave; ++i){
    gl4duPushMatrix();
    gl4duRotatef(_paveStruct[i].rot.angle, _paveStruct[i].rot.x, _paveStruct[i].rot.y, _paveStruct[i].rot.z);
    gl4duTranslatef(0.0f, _paveStruct[i].p.z, _paveStruct[i].p.y);
    //gl4duScalef(sections[i].walls[j].scale.x, sections[i].walls[j].scale.y, sections[i].walls[j].scale.z);
    gl4duScalef(1.2f, 1.2f, 1.2f);

    /* set le uniform tex_scale (dans le vertex shader) à la taille du scale */
         glUniform1f( glGetUniformLocation(_pId, "tex_scale"), 1.0f );

    gl4duSendMatrices();
    gl4duPopMatrix();
    //glUniform4fv(glGetUniformLocation(_pId, "color"), 1, sections[i].color);
    glUniform4f(glGetUniformLocation(_pId, "color"), 1.0f, 1.0f, 1.0f, 1.0f);

    glBindTexture(GL_TEXTURE_2D, _tex[0]);
    gl4dgDraw(_pave);

      /* on désactive la texture */
         glBindTexture(GL_TEXTURE_2D, 0);
    
  }

  glUniform1i( glGetUniformLocation(_pId, "hasTexture"), 0);


  /* binder (mettre au premier plan, "en courante" ou "en active") la
     matrice modelView */
  gl4duBindMatrix("modelViewMatrix");
  /* mettre la matrice identité (celle qui ne change rien) dans la matrice courante */
  gl4duLoadIdentityf();
  //gl4duLookAtf(0.0f, 1.0f, 2.4f, 0.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f);


  /* récup de dt */
  static double t0 = -1.0;
  if(t0 < 0.0)
    t0 = gl4dGetElapsedTime() / 1000.0;
  double t = gl4dGetElapsedTime() / 1000.0, dtime = t - t0;
  t0 = t;
  /* fin récup de dt */

    float revealSpeed = 0.1f;

  for(int i = 0; i< _nb_pave; ++i){
    if(_paveStruct[i].p.z < 3.0f){
      _paveStruct[i].p.y = -1.0f;
    }
    _paveStruct[i].p.z -= (fps * revealSpeed) *dtime;
    revealSpeed -= 0.0015f;
  }

  mobile_draw(_pId, _models, nb_models);
  mobile_simu();

  /* n'utiliser aucun programme GPU (pas nécessaire) */
  glUseProgram(0);

  if(!dt)
   glDisable(GL_CULL_FACE);
}

/* appelée lors du exit */
void quit(void) {
  mobile_quit();
  if(_paveStruct){
    free(_paveStruct);
    _paveStruct = NULL;
  }
}


void mobile_init(int nb_models) {

  assert(models == NULL && palmtrees == NULL);

  models = malloc(4 * sizeof(model_t));
  palmtrees = malloc(5 * sizeof(model_t));


  assert(models && palmtrees);

  int i;

  //chair
  models[0].p.x = -0.8f;
  models[0].p.y = -0.5f;
  models[0].p.z = -67.0f;
  models[0].r = 1.0f;
  models[0].rot.angle = 30.0f;
  models[0].rot.x = 0.0f;
  models[0].rot.y = 1.0f;
  models[0].rot.z = 0.0f;
  for(i = 0; i<4; ++i)
   models[0].color[i] = 0.0f;

  //ball
  models[1].p.x = -1.3f;
  models[1].p.y = -0.5f;
  models[1].p.z = -67.7f;
  models[1].r = 0.3f;
  models[1].rot.angle = 30.0f;
  models[1].rot.x = 0.0f;
  models[1].rot.y = 1.0f;
  models[1].rot.z = 0.0f;
  for(i = 0; i<4; ++i)
   models[1].color[i] = 0.0f;

  //umbrella
  models[2].p.x = -1.0f;
  models[2].p.y = -0.5f;
  models[2].p.z = -66.2f;
  models[2].r = 1.3f;
  models[2].rot.angle = 30.0f;
  models[2].rot.x = 0.0f;
  models[2].rot.y = 1.0f;
  models[2].rot.z = 0.0f;
  for(i = 0; i<4; ++i)
   models[2].color[i] = 0.0f;

  //second chair
  models[3].p.x = 1.0f;
  models[3].p.y = -0.5f;
  models[3].p.z = -68.0f;
  models[3].r = 1.0f;
  models[3].rot.angle = 120.0f;
  models[3].rot.x = 0.0f;
  models[3].rot.y = 1.0f;
  models[3].rot.z = 0.0f;
  for(i = 0; i<4; ++i)
   models[3].color[i] = 0.0f;

  //palmtrees
  palmtrees[0].p.x = -1.0f;
  palmtrees[0].p.y = -0.5f;
  palmtrees[0].p.z = -61.3f;
  palmtrees[0].r = 2.0f;
  palmtrees[0].rot.angle = 20.0f;
  palmtrees[0].rot.x = 0.0f;
  palmtrees[0].rot.y = 1.0f;
  palmtrees[0].rot.z = 0.0f;
  for(i = 0; i<4; ++i)
   palmtrees[0].color[i] = 0.0f;

  palmtrees[1].p.x = 1.0f;
  palmtrees[1].p.y = -0.5f;
  palmtrees[1].p.z = -62.5f;
  palmtrees[1].r = 2.0f;
  palmtrees[1].rot.angle = 25.0f;
  palmtrees[1].rot.x = 0.0f;
  palmtrees[1].rot.y = 1.0f;
  palmtrees[1].rot.z = 0.0f;
  for(i = 0; i<4; ++i)
   palmtrees[1].color[i] = 0.0f;

  palmtrees[2].p.x = -1.5f;
  palmtrees[2].p.y = -0.5f;
  palmtrees[2].p.z = -63.6f;
  palmtrees[2].r = 2.0f;
  palmtrees[2].rot.angle = 30.0f;
  palmtrees[2].rot.x = 0.0f;
  palmtrees[2].rot.y = 1.0f;
  palmtrees[2].rot.z = 0.0f;
  for(i = 0; i<4; ++i)
   palmtrees[2].color[i] = 0.0f;

  palmtrees[3].p.x = 1.3f;
  palmtrees[3].p.y = -0.5f;
  palmtrees[3].p.z = -64.5f;
  palmtrees[3].r = 2.0f;
  palmtrees[3].rot.angle = 46.0f;
  palmtrees[3].rot.x = 0.0f;
  palmtrees[3].rot.y = 1.0f;
  palmtrees[3].rot.z = 0.0f;
  for(i = 0; i<4; ++i)
   palmtrees[3].color[i] = 0.0f;

  palmtrees[4].p.x = -1.4f;
  palmtrees[4].p.y = -0.5f;
  palmtrees[4].p.z = -65.5f;
  palmtrees[4].r = 2.0f;
  palmtrees[4].rot.angle = 56.0f;
  palmtrees[4].rot.x = 0.0f;
  palmtrees[4].rot.y = 1.0f;
  palmtrees[4].rot.z = 0.0f;
  for(i = 0; i<4; ++i)
   palmtrees[4].color[i] = 0.0f;


}

void mobile_simu() {

  /* récup de dt */
  static double t0 = -1.0;
  if(t0 < 0.0)
    t0 = gl4dGetElapsedTime() / 1000.0;
  double t = gl4dGetElapsedTime() / 1000.0, dt = t - t0;
  t0 = t;
  /* fin récup de dt */
  static float v = 0.5f;
  int i, j;

  if(models[0].p.z < -63.0f){
    if(models[0].color[3] < 1.0f){
      for(i = 0; i< 4; ++i){
        models[i].color[3] += 0.05f;
      }
      for(i = 0; i<5; i++){
        palmtrees[i].color[3] += 0.05f;
      }
    }
  }

  for(i = 0; i<4; ++i){
    models[i].p.z += (fps * v) * dt;
  }
  for(i = 0; i<5; ++i){
    palmtrees[i].p.z += (fps * v) * dt;
  }

    if(models[0].p.z < -60.0f)
    v = 0.8f;
  else if(models[0].p.z < -57.5f)
    v = 0.775f;
  else if(models[0].p.z < -55.0f)
    v = 0.75f;
  else if(models[0].p.z < -52.5f)
    v = 0.725f;
  else if(models[0].p.z < -50.0f)
    v = 0.70f;
  else if(models[0].p.z < -47.5f)
    v = 0.675f;
  else if(models[0].p.z < -45.0f)
    v = 0.65f;
  else if(models[0].p.z < -42.5f)
    v = 0.625f;
  else if(models[0].p.z < -40.0f)
    v = 0.60f;
  else if(models[0].p.z < -37.5f)
    v = 0.575f;
  else if(models[0].p.z < -35.0f)
    v = 0.55f;
  else if(models[0].p.z < -32.5f)
    v = 0.5f;
  else if(models[0].p.z < -30.0f)
    v = 0.4f;
  else if(models[0].p.z < -27.5f)
    v = 0.35f;
  else if(models[0].p.z < -25.0f)
    v = 0.3f;
  else if(models[0].p.z < -22.5f)
    v = 0.275f;
  else if(models[0].p.z < -20.0f)
    v = 0.25f;



  if(models[0].color[0] < 1.0f){
    for(i = 0; i<4; ++i){
      for(j = 0; j<3; ++j){
        models[i].color[j] += 0.004f;
      }
    }
    for(i = 0; i<5; ++i){
      for(j = 0; j<3; ++j){
        palmtrees[i].color[j] += 0.004f;
      }
    }
  }

}

void mobile_draw(GLuint pId, GLuint * _models, int nb_models) {

  int i;

  for(i = 0; i<5; ++i){
    gl4duPushMatrix();
    gl4duTranslatef(palmtrees[i].p.x, palmtrees[i].p.y, palmtrees[i].p.z);
    gl4duScalef(palmtrees[i].r, palmtrees[i].r, palmtrees[i].r);
    gl4duRotatef(palmtrees[i].rot.angle, palmtrees[i].rot.x, palmtrees[i].rot.y, palmtrees[i].rot.z);
    //gl4duRotatef(-20.0f, 0.0f, 1.0f, 0.0f);
    /* envoyer les matrice GL4D au programme GPU OpenGL (en cours) */
    glUniform4fv(glGetUniformLocation(pId, "color"), 1, palmtrees[i].color);

    //glUniform4f(glGetUniformLocation(pId, "color"), _office[i].color[0], 
      //        _office[i].color[1], _office[i].color[2], _office[i].color[3]);
    assimpDrawScene(_models[0]);
    gl4duPopMatrix();
  }

  for(i = 0; i<3; ++i){
    gl4duPushMatrix();
    gl4duTranslatef(models[i].p.x, models[i].p.y, models[i].p.z);
    gl4duScalef(models[i].r, models[i].r, models[i].r);
    gl4duRotatef(models[i].rot.angle, models[i].rot.x, models[i].rot.y, models[i].rot.z);
    //gl4duRotatef(-20.0f, 0.0f, 1.0f, 0.0f);
    /* envoyer les matrice GL4D au programme GPU OpenGL (en cours) */
    glUniform4fv(glGetUniformLocation(pId, "color"), 1, models[i].color);

    //glUniform4f(glGetUniformLocation(pId, "color"), _office[i].color[0], 
      //        _office[i].color[1], _office[i].color[2], _office[i].color[3]);
    assimpDrawScene(_models[i+1]);
    gl4duPopMatrix();
  }
  gl4duPushMatrix();
  gl4duTranslatef(models[3].p.x, models[3].p.y, models[3].p.z);
  gl4duScalef(models[3].r, models[3].r, models[3].r);
  gl4duRotatef(models[3].rot.angle, models[3].rot.x, models[3].rot.y, models[3].rot.z);
  //gl4duRotatef(-20.0f, 0.0f, 1.0f, 0.0f);
  /* envoyer les matrice GL4D au programme GPU OpenGL (en cours) */
  glUniform4fv(glGetUniformLocation(pId, "color"), 1, models[3].color);

  //glUniform4f(glGetUniformLocation(pId, "color"), _office[i].color[0], 
    //        _office[i].color[1], _office[i].color[2], _office[i].color[3]);
  assimpDrawScene(_models[1]);
  gl4duPopMatrix();

}

void mobile_quit(void) {
  if(models){
    free(models);
    models = NULL;
  }
  if(palmtrees){
    free(palmtrees);
    palmtrees = NULL;
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

