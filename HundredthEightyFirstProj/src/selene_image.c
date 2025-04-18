#include "selene_image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "selene_stb_image.h"

extern mat4x4 view;

Image * selene_create_image(const char * path) {
  Image * image = (Image*) malloc(sizeof(Image));

  int w, h, comp;
  unsigned char * img = stbi_load(path, &(image->_width), &(image->_height), &comp, STBI_rgb_alpha);

  image->_filtermin = malloc(sizeof(char) * 50);
  image->_filtermag = malloc(sizeof(char) * 50);

  image->_wraps = malloc(sizeof(char) * 50);
  image->_wrapt = malloc(sizeof(char) * 50);
  
  //image->_filtermin = "nearest";
  strcpy(image->_filtermin, "nearest");
  //image->_filtermag = "nearest";
  strcpy(image->_filtermag, "nearest");
  //image->_wraps = "clamp";
  strcpy(image->_wraps, "clamp");
  //image->_wrapt = "clamp";
  strcpy(image->_wrapt, "clamp");
  
  /*float vertices[] = {
    0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f
    };*/
  Vertex vertices[4];
  for (int i = 0; i < 4; i++)
    vertices[i].r = vertices[i].g = vertices[i].b = 255.0f/255.0f;
  vertices[0].x = 0.0f;
  vertices[0].y = 0.0f;
  vertices[1].x = 0.0f;
  vertices[1].y = 1.0f;
  vertices[2].x = 1.0f;
  vertices[2].y = 1.0f;
  vertices[3].x = 1.0f;
  vertices[3].y = 0.0f;

  vertices[0].s = 0.0f;
  vertices[0].t = 0.0f;
  vertices[1].s = 0.0f;
  vertices[1].t = 1.0f;
  vertices[2].s = 1.0f;
  vertices[2].t = 1.0f;
  vertices[3].s = 1.0f;
  vertices[3].t = 0.0f;
 
  unsigned int indices[] = {
    0, 1, 2,
    2, 3, 0
  };
  
  glGenVertexArrays(1, &(image->_vao));
  glBindVertexArray(image->_vao);

  glGenBuffers(1, &(image->_vbo));
  
  glBindBuffer(GL_ARRAY_BUFFER, image->_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  
  glGenBuffers(1, &(image->_ebo));
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, image->_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (4 * sizeof(vertices[0].s)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) (2 * sizeof(vertices[0].s)));
  glEnableVertexAttribArray(2);
  
  glBindVertexArray(0);

  // Textures
  
  glGenTextures(1, &(image->_tex));

  glBindTexture(GL_TEXTURE_2D, image->_tex);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  if (!img) {
    printf("Could not be possible to load the image: %s\n", path);
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->_width, image->_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);

  stbi_image_free(img);
  glBindTexture(GL_TEXTURE_2D, 0);

  //selene_add_text_data(name, image->_tex);
  
  return image;
}

void selene_set_image_filter(Image * image, const char * filtermin, const char * filtermag) {

  /***************** Filter Min *******************/
  if (!strcmp(filtermin, "linear")) {
    glBindTexture(GL_TEXTURE_2D, image->_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
  } else if (!strcmp(filtermin, "nearest")) {
    glBindTexture(GL_TEXTURE_2D, image->_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
  } else {
    printf("%s is not a valid filter min\n", filtermin);
  }
  /************** Filter Mag ***************/
  if (!strcmp(filtermag, "linear")) {
    glBindTexture(GL_TEXTURE_2D, image->_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
  } else if(!strcmp(filtermag, "linear")) {
    glBindTexture(GL_TEXTURE_2D, image->_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
  } else {
    printf("%s is not a valid filter mag\n", filtermag);
  }
}

void selene_set_image_wrap(Image * image, const char * wraps, const char * wrapt) {

  /************* WRAP S ***************/
  if (!strcmp(wraps, "repeat")) {
    glBindTexture(GL_TEXTURE_2D, image->_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);
  } else if (!strcmp(wraps, "mirrored_repeat")) {
    glBindTexture(GL_TEXTURE_2D, image->_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);
  } else if(!strcmp(wraps, "clamp")) {
    glBindTexture(GL_TEXTURE_2D, image->_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
  } else {
    printf("%s is not a valid Wrap S\n", wraps);
  }

  /**************** WRAP T ***************/
  if (!strcmp(wrapt, "repeat")) {
    glBindTexture(GL_TEXTURE_2D, image->_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);
  } else if (!strcmp(wrapt, "mirrored_repeat")) {
    glBindTexture(GL_TEXTURE_2D, image->_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);
  } else if(!strcmp(wrapt, "clamp")) {
    glBindTexture(GL_TEXTURE_2D, image->_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
  } else {
    printf("%s is not a valid Wrap T\n", wrapt);
  }
}

int selene_get_image_width(Image * image) {
  return image->_width;
}

int selene_get_image_height(Image * image) {
  return image->_height;
}

void selene_draw_image(Image * image, Quad * quad, int x, int y) {

  mat4x4 model;

  mat4x4_translate(model, x, y, 0.0);

  vec4 tex;
  if (quad) {

    for (int i = 0; i < 4; i++) {
      float d = i % 2 == 0 ? (float)selene_get_image_width(image) : (float) selene_get_image_height(image);
      tex[i] = (float)quad->_attr[i] / d;
    }
    
    mat4x4_scale_aniso(model, model, quad->_width, quad->_height, 0.0f);
  } else {
    tex[0] = 0.0f;
    tex[1] = 0.0f;

    tex[2] = 1.0f;
    tex[3] = 1.0f;
    mat4x4_scale_aniso(model, model, selene_get_image_width(image), selene_get_image_height(image), 0.0f);
  }
  
  selene_send_uniform(CORE->_default_shader, "model", 16, *model);
  selene_send_uniform(CORE->_default_shader, "spriteFrame" , 4, tex);
  selene_send_uniform(CORE->_default_shader, "view", 16, *view);
  
  glBindTexture(GL_TEXTURE_2D, image->_tex);
  glBindVertexArray(image->_vao);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void selene_draw_image_ex(Image * image, Quad * quad, int x, int y, float sx, float sy, float angle, float cx, float cy) {

  //selene_use_default_shader();
  mat4x4 model;
  mat4x4 aux;

  mat4x4_translate(model, x, y, 0.0);

  mat4x4_rotate_Z(model, model, toRadians(angle));
  mat4x4_translate_mul(model, -cx * sx, -cy * sy, 0.0);
  
  vec4 tex;
  if (quad) {

    for (int i = 0; i < 4; i++) {
      float d = i % 2 == 0 ? (float)selene_get_image_width(image) : (float) selene_get_image_height(image);
      tex[i] = (float)quad->_attr[i] / d;
    }
    
    mat4x4_scale_aniso(model, model, quad->_width * sx, quad->_height * sy, 0.0f);
  } else {
    tex[0] = 0.0f;
    tex[1] = 0.0f;

    tex[2] = 1.0f;
    tex[3] = 1.0f;
    mat4x4_scale_aniso(model, model, selene_get_image_width(image) * sx, selene_get_image_height(image) * sy, 0.0f);
  }
  
  selene_send_uniform(CORE->_default_shader, "model", 16, *model);
  selene_send_uniform(CORE->_default_shader, "spriteFrame" , 4, tex);
  selene_send_uniform(CORE->_default_shader, "view", 16, *view);

  glBindTexture(GL_TEXTURE_2D, image->_tex);
  glBindVertexArray(image->_vao);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void selene_destroy_image(Image* image) {
  glDeleteVertexArrays(1, &(image->_vao));
  glDeleteBuffers(1, &(image->_vbo));
  glDeleteBuffers(1, &(image->_ebo));
  glDeleteTextures(1, &(image->_tex));
  free(image->_filtermag);
  free(image->_filtermin);
  free(image->_wraps);
  free(image->_wrapt);
  free(image);
}
