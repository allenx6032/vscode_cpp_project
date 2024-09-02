#pragma once

#include "guilite/api.h"
#include "guilite/wnd.h"
#include "guilite/resource.h"
#include "guilite/word.h"
#include "guilite/display.h"
#include "guilite/theme.h"

typedef struct c_label c_label;
struct c_label
{
    c_wnd base;
};

void c_label_init(c_label *self);
