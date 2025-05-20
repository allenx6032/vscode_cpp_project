#include "rubik_font_provider.c.h"

#if FP_BACKEND == FP_BACKEND_STB_TRUETYPE
# include "rubik_font_provider_stb_truetype.c.h"
#else
# error Font provider backend not specified.
#endif
