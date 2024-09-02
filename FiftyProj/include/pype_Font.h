#pragma once

#include "pype_common.h"

using namespace pkpy;

struct PXFont {
    PY_CLASS(PXFont, pype, Font)

    PX_FontModule* ptr;
    PXFont(PX_FontModule* ptr) : ptr(ptr) {}

    static void _register(VM* vm, PyObject* mod, PyObject* type){
        vm->bind_static_method<1>(type, "__new__", [](VM* vm, ArgsView args){
            PX_FontModule* ptr = CAST(PX_FontModule*, args[0]);
            return VAR_T(PXFont, ptr);
        }); 
    }
};