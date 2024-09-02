#pragma once

#include "pype_common.h"
#include "pype_Node.h"
#include "pype_Color.h"

inline void _register_progressbar_type(VM* vm, PyObject* mod){
    PyObject* type = vm->new_type_object(mod, "ProgressBar", g_tp_node);
    vm->bind_method<0>(type, "_px_obj_init", [](VM* vm, ArgsView args){
        PX_Object* obj = PX_Object_ProcessBarCreate(
            &App.runtime.mp_game,
            get_px_obj(g_root), // PX_Object *Parent,
            0,
            0,
            120,
            40
        );
        inject_py_object(obj, args[0]);
        return VAR(obj);
    });

    type->attr().set("color", vm->property(
        [](VM* vm, ArgsView args){
            PX_Object* obj = _get_px_obj(args[0]);
            PX_Object_ProcessBar* bar = PX_Object_GetProcessBar(obj);
            return VAR_T(Color, bar->Color);
        },
        [](VM* vm, ArgsView args){
            PX_Object* obj = _get_px_obj(args[0]);
            PX_Object_ProcessBar* bar = PX_Object_GetProcessBar(obj);
            bar->Color = CAST(Color&, args[1]);
            return vm->None;
        }));

    type->attr().set("value", vm->property(
        [](VM* vm, ArgsView args){
            PX_Object* obj = _get_px_obj(args[0]);
            PX_Object_ProcessBar* bar = PX_Object_GetProcessBar(obj);
            return VAR(bar->Value / 100.0);
        },
        [](VM* vm, ArgsView args){
            PX_Object* obj = _get_px_obj(args[0]);
            PX_Object_ProcessBar* bar = PX_Object_GetProcessBar(obj);
            bar->Value = (int)(CAST(f64, args[1]) * 100);
            if(bar->Value < 0) bar->Value = 0;
            if(bar->Value > 100) bar->Value = 100;
            return vm->None;
        }));
}