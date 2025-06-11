#include <ptext_screen.h>
#include <ptext_utils.h>

void screenAppend(struct screenBuffer *buff, const char *s, size_t len) {
  char *new = xrealloc(buff->chars, buff->len + len);
  memcpy(&new[buff->len], s, len);
  buff->chars = new;
  buff->len += len;
}
