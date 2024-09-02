#pragma once

#include <queue>
#include <sstream>

#include <filesystem>
#include <fstream>

#include "PX_Application.h"
#include "PainterEngine.h"
#include "PX_World.h"

#include "pype_pocketpy.h"
#include "pype_generated.h"

using namespace pkpy;

inline PX_Application App;
inline PX_World World;

inline VM* vm = nullptr;                    // global python vm
inline PyObject* g_mod = nullptr;           // global PainterEngine module
inline PyObject* g_root = nullptr;          // global root gameObject
inline Type g_tp_node;                      // global Node type cache


inline void PXError(Str msg){
    vm->_error("PXError", msg);
}