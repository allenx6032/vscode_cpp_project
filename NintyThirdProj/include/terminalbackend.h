#pragma once

#include "vec2.h"
#include "backend.h"

typedef struct TerminalBackend {
  Backend backend;
} TerminalBackend;

void terminal_backend_init(TerminalBackend *t, Vec2i size);
