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
static int nb_models = 2;
static int _nb_pave = 8;

static double fps = 22.0f;

static GLuint *_models = NULL;


static model_t * _office = NULL;
static model_t * _officemult = NULL;
static model_t * _car = NULL;
static model_t * _paveStruct = NULL;

//static int _ww = 800, _wh = 800;

void scene6(int state) {
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

   _models[0] = assimpGenScene("models/office/scene.gltf");
   _models[1] = assimpGenScene("models/office2/scene.gltf");

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
  SDL_Surface * src = IMG_Load("images/childhood.tif");
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
  GLfloat lum[4] = {0.0f, 3.0f, -1.3f, 1.0f};
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
  glUniform1f( glGetUniformLocation(_pId, "shininess"), 100.0f );
  glUniform4f(glGetUniformLocation(_pId, "color"), 1.0f, 1.0f, 1.0f, 1.0f);
  glUniform4f(glGetUniformLocation(_pId, "diffuse_color"), 1.0f, 1.0f, 1.0f, 1.0f);
  glUniform4f(glGetUniformLocation(_pId, "ambient_color"), 1.0f, 1.0f, 1.0f, 1.0f);
  glUniform4f(glGetUniformLocation(_pId, "specular_color"), 1.0f, 0.8f, 0.8f, 1.0f);


  /* UN SOL */
  /* binder (mettre au premier plan, "en courante" ou "en active") la
     matrice modelView */
  gl4duBindMatrix("modelViewMatrix");
  /* mettre la matrice identité (celle qui ne change rien) dans la matrice courante */
  gl4duLoadIdentityf();
  gl4duTranslatef(0.0f, -1.0, _officemult[3].p.z - 3.0f);
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

  assert(_office == NULL && _officemult == NULL && _car == NULL);
  _office = malloc(sizeof(model_t));
  _officemult = malloc(4 * sizeof(model_t));
  _car = malloc(sizeof(model_t));


  assert(_office && _officemult && _car);

  int i;


  //office
  _office->p.x = -1.1f;
  _office->p.y = -0.5f;
  //_office[0].p.z = -70.0f;
  _office->p.z = -72.0f;
  _office->r = 1.1f;
  _office->rot.angle = -60.0f;
  _office->rot.x = 0.0f;
  _office->rot.y = 1.0f;
  _office->rot.z = 0.0f;
  for(i = 0; i<3; ++i)
    _office[0].color[i] = 0.0f;

  //officemult
  _officemult[0].p.x = -1.3f;
  _officemult[0].p.y = -0.5f;
  //_office[0].p.z = -70.0f;
  _officemult[0].p.z = -63.0f;
  _officemult[0].r = 1.0f;
  _officemult[0].rot.angle = 60.0f;
  _officemult[0].rot.x = 0.0f;
  _officemult[0].rot.y = 1.0f;
  _officemult[0].rot.z = 0.0f;
  for(i = 0; i<3; ++i)
    _officemult[0].color[i] = 0.0f;

  _officemult[1].p.x = 1.3f;
  _officemult[1].p.y = -0.5f;
  //_office[0].p.z = -70.0f;
  _officemult[1].p.z = -65.0f;
  _officemult[1].r = 1.0f;
  _officemult[1].rot.angle = -60.0f;
  _officemult[1].rot.x = 0.0f;
  _officemult[1].rot.y = 1.0f;
  _officemult[1].rot.z = 0.0f;
  for(i = 0; i<3; ++i)
    _officemult[1].color[i] = 0.0f;

  _officemult[2].p.x = -1.3f;
  _officemult[2].p.y = -0.5f;
  //_office[0].p.z = -70.0f;
  _officemult[2].p.z = -67.0f;
  _officemult[2].r = 1.0f;
  _officemult[2].rot.angle = 60.0f;
  _officemult[2].rot.x = 0.0f;
  _officemult[2].rot.y = 1.0f;
  _officemult[2].rot.z = 0.0f;
  for(i = 0; i<3; ++i)
    _officemult[2].color[i] = 0.0f;

  _officemult[3].p.x = 1.3f;
  _officemult[3].p.y = -0.5f;
  //_office[0].p.z = -70.0f;
  _officemult[3].p.z = -69.0f;
  _officemult[3].r = 1.0f;
  _officemult[3].rot.angle = -60.0f;
  _officemult[3].rot.x = 0.0f;
  _officemult[3].rot.y = 1.0f;
  _officemult[3].rot.z = 0.0f;
  for(i = 0; i<3; ++i)
    _officemult[3].color[i] = 0.0f;




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

  if(_office->p.z < -63.0f){
    if(_office->color[3] < 1.0f){
      for(i = 0; i<4; ++i){
        _officemult[i].color[3] += 0.05f;
      }
      _office->color[3] +=0.05f;
    }
  }

   if(_office->color[0] < 1.0f){
     for(j = 0; j<3; ++j){
      _office->color[j] += 0.004f;
     }
    }

  _office->p.z += (fps * v) * dt;
  for(i =0; i<4; ++i){
    _officemult[i].p.z += (fps * v) * dt;
  } 



  if(_office->p.z < -60.0f)
    v = 0.8f;
  else if(_office->p.z < -57.5f)
    v = 0.775f;
  else if(_office->p.z < -55.0f)
    v = 0.75f;
  else if(_office->p.z < -52.5f)
    v = 0.725f;
  else if(_office->p.z < -50.0f)
    v = 0.70f;
  else if(_office->p.z < -47.5f)
    v = 0.675f;
  else if(_office->p.z < -45.0f)
    v = 0.65f;
  else if(_office->p.z < -42.5f)
    v = 0.625f;
  else if(_office->p.z < -40.0f)
    v = 0.60f;
  else if(_office->p.z < -37.5f)
    v = 0.575f;
  else if(_office->p.z < -35.0f)
    v = 0.55f;
  else if(_office->p.z < -32.5f)
    v = 0.5f;
  else if(_office->p.z < -30.0f)
    v = 0.4f;
  else if(_office->p.z < -27.5f)
    v = 0.35f;
  else if(_office->p.z < -25.0f)
    v = 0.3f;
  else if(_office->p.z < -22.5f)
    v = 0.275f;
  else if(_office->p.z < -20.0f)
    v = 0.25f;


  for(i = 0; i<5; ++i){
     for(j = 0; j<3; ++j){
        _officemult[i].color[j] += 0.004f;
      }
  }

}

void mobile_draw(GLuint pId, GLuint * _models, int nb_models) {

  int i;

  for(i = 3; i>=0; --i){
    gl4duPushMatrix();
    gl4duTranslatef(_officemult[i].p.x, _officemult[i].p.y, _officemult[i].p.z);
    gl4duScalef(_officemult[i].r, _officemult[i].r, _officemult[i].r);
    gl4duRotatef(_officemult[i].rot.angle, _officemult[i].rot.x, _officemult[i].rot.y, _officemult[i].rot.z);
    //gl4duRotatef(-20.0f, 0.0f, 1.0f, 0.0f);
    /* envoyer les matrice GL4D au programme GPU OpenGL (en cours) */
    glUniform4fv(glGetUniformLocation(pId, "color"), 1, _officemult[i].color);

    //glUniform4f(glGetUniformLocation(pId, "color"), _office[i].color[0], 
      //        _office[i].color[1], _office[i].color[2], _office[i].color[3]);
    assimpDrawScene(_models[1]);
    gl4duPopMatrix();
  }

  gl4duPushMatrix();
  gl4duTranslatef(_office->p.x, _office->p.y, _office->p.z);
  gl4duScalef(_office->r, _office->r, _office->r);
  gl4duRotatef(_office->rot.angle, _office->rot.x, _office->rot.y, _office->rot.z);
  //gl4duRotatef(-20.0f, 0.0f, 1.0f, 0.0f);
  /* envoyer les matrice GL4D au programme GPU OpenGL (en cours) */
  glUniform4fv(glGetUniformLocation(pId, "color"), 1, _office->color);

  //glUniform4f(glGetUniformLocation(pId, "color"), _office[i].color[0], 
    //        _office[i].color[1], _office[i].color[2], _office[i].color[3]);
  assimpDrawScene(_models[0]);
  gl4duPopMatrix();



}

void mobile_quit(void) {
  if(_office){
    free(_office);
    _office = NULL;
  }
  if(_officemult){
    free(_officemult);
    _officemult = NULL;
  }
  if(_car){
    free(_car);
    _car = NULL;
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


