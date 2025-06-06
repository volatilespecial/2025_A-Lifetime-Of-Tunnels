/*!\file window.c
 *
 * \brief Utilisation de GL4Dummies pour réaliser une démo.
 *
 * Ici est géré l'ouverture de la fenêtre ainsi que l'ordonnancement
 * des animations. Apriori les seuls éléments à modifier ici lors de
 * votre intégration sont le tableau static \ref _animations et le nom
 * du fichier audio à lire.
 *
 * \author Farès BELHADJ, amsi@up8.edu
 * \date April 12, 2023
 */
#include <stdlib.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4dh.h>
#include <GL4D/gl4duw_SDL2.h>
#include "animations.h"
#include "audioHelper.h"

/* Prototypes des fonctions statiques contenues dans ce fichier C. */
static void init(void);
static void quit(void);
static void resize(int w, int h);
static void keydown(int keycode);

/*!\brief tableau contenant les animations sous la forme de timeline,
 * ce tableau se termine toujours par l'élémént {0, NULL, NULL,
 * NULL} */
static GL4DHanime _animations[] = {
  { 2000, noir, NULL, NULL },
  { 1000, noir, scene1, fondu },
  { 4000, scene1, NULL, NULL},
  { 500, scene1, scene2, fondu},
  { 3500, scene2, NULL, NULL },
  { 500, scene2, scene3, fondu},
  { 11000, scene3, NULL, NULL },
  { 2000, scene3, scene4, fondu},
  { 9000, scene4, NULL, NULL },
  { 2000, scene4, sceneTransition1, fondu},
  { 7000, sceneTransition1, NULL, NULL },
  { 2000, sceneTransition1, scene5, fondu},
  { 8800, scene5, NULL, NULL },
  { 2000, scene5, sceneTransition2, fondu},
  { 7000, sceneTransition2, NULL, NULL },
  { 2000, sceneTransition2, scene6, fondu},
  { 9500, scene6, NULL, NULL },
  { 2000, scene6, sceneTransition3, fondu},
  { 7000, sceneTransition3, NULL, NULL },
  { 2000, sceneTransition3, scene7, fondu},
  { 7800, scene7, NULL, NULL },
  { 1000, scene7, sceneTransition4, fondu},
  { 7700, sceneTransition4, NULL, NULL },
  { 2000, sceneTransition4, scene8, fondu},
  { 25000, scene8, NULL, NULL },
  { 2000, scene8, scene9, fondu},
  { 14000, scene9, NULL, NULL },
  { 1000, scene9, credit1, fondu },
  { 3000, credit1, NULL, NULL },
  { 500, credit1, blanc, fondu },
  { 500, blanc, NULL, NULL },
  { 500, blanc, credit2, fondu },
  { 4000, credit2, NULL, NULL },
  { 500, credit2, blanc, fondu },
  { 500, blanc, NULL, NULL },
  { 500, blanc, credit3, fondu },
  { 1500, credit3, NULL, NULL },
  { 500, credit3, blanc, fondu },
  { 500, blanc, NULL, NULL },
  { 500, blanc, credit4, fondu },
  { 1500, credit4, NULL, NULL },
  { 500, credit4, blanc, fondu },
  { 500, blanc, NULL, NULL },
  { 500, blanc, credit5, fondu },
  { 1500, credit5, NULL, NULL },
  { 500, credit5, blanc, fondu },
  { 500, blanc, NULL, NULL },
  { 500, blanc, credit6, fondu },
  { 1500, credit6, NULL, NULL },
  { 500, credit6, blanc, fondu },
  { 500, blanc, NULL, NULL },
  { 500, blanc, credit7, fondu },
  { 1500, credit7, NULL, NULL },
  { 500, credit7, blanc, fondu },
  { 500, blanc, NULL, NULL },
  { 500, blanc, credit8, fondu },
  { 4000, credit8, NULL, NULL },
  { 4000, credit8, noir, fondu },
  { 1000, noir, NULL, NULL },
  {    0, NULL, NULL, NULL } /* Toujours laisser à la fin */
};

/*!\brief dimensions initiales de la fenêtre */
static GLfloat _dim[] = {1920, 1080};
//static GLfloat _dim[] = {1280, 720};

/*!\brief fonction principale : initialise la fenêtre, OpenGL, audio
 * et lance la boucle principale (infinie).
 */
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "Ateliers API8 - démo", 
			 GL4DW_POS_UNDEFINED, GL4DW_POS_UNDEFINED, 
			 _dim[0], _dim[1],
			 ///* GL4DW_RESIZABLE |  */GL4DW_SHOWN /*| GL4DW_FULLSCREEN_DESKTOP*/))
       /* GL4DW_RESIZABLE |  */GL4DW_SHOWN | GL4DW_FULLSCREEN_DESKTOP))
    return 1;
  SDL_GL_SetSwapInterval(1); /* zéro pour désynchro verticale -> 6000 fps sur machine démo */
  init();
  atexit(quit);
  gl4duwResizeFunc(resize);
  gl4duwKeyDownFunc(keydown);
  gl4duwDisplayFunc(gl4dhDraw);

  ahInitAudio("relaxing-ambient-music-nostalgic-memories-310690.mp3");
  gl4duwMainLoop();
  return 0;
}

/*!\brief Cette fonction initialise les paramètres et éléments OpenGL
 * ainsi que les divers données et fonctionnalités liées à la gestion
 * des animations.
 */
static void init(void) {
  glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
  gl4dhInit(_animations, _dim[0], _dim[1], animationsInit);
  resize(_dim[0], _dim[1]);
}

/*!\brief paramétre la vue (viewPort) OpenGL en fonction des
 * dimensions de la fenêtre.
 * \param w largeur de la fenêtre.
 * \param w hauteur de la fenêtre.
 */
static void resize(int w, int h) {
  _dim[0] = w; _dim[1] = h;
  glViewport(0, 0, _dim[0], _dim[1]);
}

/*!\brief permet de gérer les évènements clavier-down.
 * \param keycode code de la touche pressée.
 */
static void keydown(int keycode) {
  switch(keycode) {
  case SDLK_ESCAPE:
  case 'q':
    exit(0);
  default: break;
  }
}

/*!\brief appelée à la sortie du programme (atexit).
 */
static void quit(void) {
  ahClean();
  gl4duClean(GL4DU_ALL);
}
