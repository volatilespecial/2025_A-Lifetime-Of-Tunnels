/*!\file window.c
 * \brief rendu de texte avec bibliothèque SDL2_ttf
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date June 04 2018 */
#include <stdio.h>
#include <assert.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dh.h>
#include <SDL_ttf.h>
/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(void);
//static void resize(int w, int h);
static void draw(void);
static void quit(void);
static void initText(GLuint * ptId, const char * text);
static void viewportResize(void);

/*!\brief dimensions de la fenêtre */
//static int _wW = 800, _wH = 600;
/*!\brief identifiant du programme GLSL */
static GLuint _pId = 0;
/*!\brief identifiant de la géométrie d'un plan */
static GLuint _quad = 0;
/*!\brief identifiant de la texture contenant le texte */
static GLuint _textTexId = 0;

void credit7(int state) {
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

/*!\brief initialise les paramètres OpenGL et les données */
static void init(void) {
  _pId = gl4duCreateProgram("<vs>shaders/credits.vs", "<fs>shaders/credits.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  //resize(_wW, _wH);
  gl4duBindMatrix("projectionMatrix");
  viewportResize();
  // gl4duLoadIdentityf();
  // gl4duFrustumf(-1, 1, -1, 1, 1, 1000);
  gl4duBindMatrix("modelViewMatrix");
  _quad = gl4dgGenQuadf();
  initText(&_textTexId,
  "Tiles 007 : This work is based on \"Tiles 007\" by ambientCG licensed under CC-CC0.\n\n"
  "Grass 008 : This work is based on \"Grass 008\" by ambientCG licensed under CC-CC0.\n\n"
  "Wood Floor 064 : This work is based on \"Wood Floor 064\" by ambientCG licensed under CC-CC0.\n\n"
  "Ground 080 : This work is based on \"Ground 080\" by ambientCG licensed under CC-CC0.\n\n"
    );
}
/*!\brief Cette fonction paramétre la vue (viewport) OpenGL en
 * fonction des dimensions de la fenêtre.*/
// static void resize(int w, int h) {
//   _wW  = w; _wH = h;
//   glViewport(0, 0, _wW, _wH);
//   gl4duBindMatrix("projectionMatrix");
//   gl4duLoadIdentityf();
//   gl4duFrustumf(-1, 1, -_wH / (GLfloat)_wW, _wH / (GLfloat)_wW, 1, 1000);
//   gl4duBindMatrix("modelViewMatrix");
// }
/*!\brief dessine dans le contexte OpenGL actif. */
static void draw(void) {
  viewportResize();
  GLboolean gb = glIsEnabled(GL_BLEND);
  glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glUseProgram(_pId);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _textTexId);
  glUniform1i(glGetUniformLocation(_pId, "inv"), 1);
  glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  gl4duScalef(0.9, 1.2, 1);
  //gl4duTranslatef(0, d * cos(inclinaison * M_PI / 180.0f), -2 + d * sin(inclinaison * M_PI / 180.0f));
  gl4duTranslatef(0.0,0.0,-2.5);
  //gl4duRotatef(inclinaison, 1, 0, 0);
  gl4duSendMatrices();
  gl4dgDraw(_quad);
  glUseProgram(0);
  if(!gb)
    glDisable(GL_BLEND);
}
/*!\brief appelée au moment de sortir du programme (atexit), libère les éléments utilisés */
static void quit(void) {
  if(_textTexId) {
    glDeleteTextures(1, &_textTexId);
    _textTexId = 0;
  }
}
/*!\brief création d'une texture avec du texte. 
 *
 * \param ptId pointeur vers la variable stockant l'identifiant de
 * texture OpenGL. Si l'identifiant existe déjà (!= 0) l'utiliser
 * sinon (== 0) en générer un nouveau.
 *
 * \param text pointeur vers la chaine de caractères contenant le
 * texte.
 */
static void initText(GLuint * ptId, const char * text) {
  static int firstTime = 1;
  SDL_Color c = {1, 1, 1, 255};
  SDL_Surface * d, * s;
  TTF_Font * font = NULL;
  if(firstTime) {
    /* initialisation de la bibliothèque SDL2 ttf */
    if(TTF_Init() == -1) {
      fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
      exit(2);
    }
    firstTime = 0;
  }
  if(*ptId == 0) {
    /* initialisation de la texture côté OpenGL */
    glGenTextures(1, ptId);
    glBindTexture(GL_TEXTURE_2D, *ptId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }
  /* chargement de la font */
  if( !(font = TTF_OpenFont("SourceSerif4.ttf", 160)) ) {
    fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());
    return;
  }
  /* création d'une surface SDL avec le texte */
  d = TTF_RenderUTF8_Blended_Wrapped(font, text, c, 3000);
  if(d == NULL) {
    TTF_CloseFont(font);
    fprintf(stderr, "Erreur lors du TTF_RenderText\n");
    return;
  }
  /* copie de la surface SDL vers une seconde aux spécifications qui correspondent au format OpenGL */
  s = SDL_CreateRGBSurface(0, d->w, d->h, 32, R_MASK, G_MASK, B_MASK, A_MASK);
  assert(s);
  SDL_BlitSurface(d, NULL, s, NULL);
  SDL_FreeSurface(d);
  /* transfert vers la texture OpenGL */
  glBindTexture(GL_TEXTURE_2D, *ptId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
  fprintf(stderr, "Dimensions de la texture : %d %d\n", s->w, s->h);
  SDL_FreeSurface(s);
  TTF_CloseFont(font);
  glBindTexture(GL_TEXTURE_2D, 0);
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