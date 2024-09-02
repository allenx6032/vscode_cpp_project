#include "vec2.h"
#include "backend.h"

typedef struct JpegBackend {
  Backend backend;
  char *jpegFilename;
} JpegBackend;

extern int jpeg_backend_init(JpegBackend *this, Vec2i size,
                             const char *filename);
