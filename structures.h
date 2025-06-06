/*!\file mobile.h
 *
 * \brief 
 * \author Farès BELHADJ, amsi@up8.edu
 * \date March 27, 2025
 */

#ifndef _MOBILE_H

#define _MOBILE_H

#include <GL4D/gl4du.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct vect3D_t vect3D_t;
  typedef struct vectRotation3D_t vectRotation3D_t;
  typedef struct model_t model_t;
  typedef struct wall_t wall_t;
  typedef struct sections_t sections_t; 

  struct vect3D_t {
    GLfloat x, y, z;
  };

  struct vectRotation3D_t{
    GLfloat angle, x, y, z;
  };

  struct model_t {
    vect3D_t p, v;
    vectRotation3D_t rot;
    GLfloat r; 
    GLfloat color[4];
  };

  struct wall_t {
    vect3D_t p;
    vectRotation3D_t rot;
    vect3D_t scale;
    
  };

  struct sections_t {
   wall_t walls[4];
   GLfloat color[4];
   GLboolean setTexture;
   int active;
  };

  
#ifdef __cplusplus
}
#endif

#endif

