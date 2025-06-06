/*!\file animations.h
 *
 * \brief Votre espace de liberté : c'est ici que vous pouvez ajouter
 * vos fonctions de transition et d'animation avant de les faire
 * référencées dans le tableau _animations du fichier \ref window.c
 *
 * Des squelettes d'animations et de transitions sont fournis pour
 * comprendre le fonctionnement de la bibliothèque. En bonus des
 * exemples dont un fondu en GLSL.
 *
 * \author Farès BELHADJ, amsi@up8.edu
 * \date April 12, 2023
 */
#ifndef _ANIMATIONS_H

#define _ANIMATIONS_H

#ifdef __cplusplus
extern "C" {
#endif

  extern void fondu(void (* a0)(int), void (* a1)(int), Uint32 t, Uint32 et, int state);
  extern void fondui(void (* a0)(int), void (* a1)(int), Uint32 t, Uint32 et, int state);
  
  extern void rouge(int state);
  extern void vert(int state);
  extern void bleu(int state);
  extern void noir(int state);
  extern void blanc(int state);
  extern void animationsInit(void);

  extern void sceneTransition1(int state);
  extern void sceneTransition2(int state);
  extern void sceneTransition3(int state);
  extern void sceneTransition4(int state);
  extern void scene1(int state);
  extern void scene2(int state);
  extern void scene3(int state);
  extern void scene4(int state);
  extern void scene5(int state);
  extern void scene6(int state);
  extern void scene7(int state);
  extern void scene8(int state);
  extern void scene9(int state);
  extern void credit1(int state);
  extern void credit2(int state);
  extern void credit3(int state);
  extern void credit4(int state);
  extern void credit5(int state);
  extern void credit6(int state);
  extern void credit7(int state);
  extern void credit8(int state);
  

#ifdef __cplusplus
}
#endif

#endif
