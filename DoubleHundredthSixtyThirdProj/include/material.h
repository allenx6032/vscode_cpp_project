#ifndef material_h
#define material_h

#include "engine.h"

typedef struct {
  char name[256];
  char texture_path[256]; 
  char normal_map_path[256]; 
  char specular_map_path[256]; 
  char mask_map_path[256];
  vec3 diffuse;
  float specular;
  float reflectivity;
  int texture_subdivision;
} material;

void material_init(material* mat);

#endif
