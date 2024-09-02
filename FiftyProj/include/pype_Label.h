#pragma once

#include "pype_common.h"
#include "pype_Font.h"
#include "pype_Node.h"

inline void _register_label_type(VM* vm, PyObject* mod){
    PyObject* type = vm->new_type_object(mod, "Label", g_tp_node);
    vm->bind_method<0>(type, "_px_obj_init", [](VM* vm, ArgsView args){
        PX_Object* obj = PX_Object_LabelCreate(
            &App.runtime.mp_game,
            get_px_obj(g_root), // PX_Object *Parent,
            0,
            0,
            120,
            40,
            "Button",             // const px_char *Text
            NULL,                 // PX_FontModule *fontmodule,
            PX_COLOR(255, 255, 255, 255)
        );
        inject_py_object(obj, args[0]);
        return VAR(obj);
    });

    type->attr().set("text", vm->property(
        [](VM* vm, ArgsView args){
            PX_Object* obj = get_px_obj(args[0]);
            char* value = PX_Object_LabelGetText(obj);
            return VAR(Str(value));
        },
        [](VM* vm, ArgsView args){
            PX_Object* obj = get_px_obj(args[0]);
            char* value = CAST(Str&, args[1]).c_str_dup();
            PX_Object_LabelSetText(obj, value);
            free(value);
            return vm->None;
        }));

    type->attr().set("font", vm->property(
        [](VM* vm, ArgsView args){
            PX_Object* obj = get_px_obj(args[0]);
            PX_Object_Label* label = PX_Object_GetLabel(obj);
            return VAR_T(PXFont, label->fontModule);
        },
        [](VM* vm, ArgsView args){
            PX_Object* obj = get_px_obj(args[0]);
            PX_Object_Label* label = PX_Object_GetLabel(obj);
            label->fontModule = CAST(PXFont&, args[1]).ptr;
            return vm->None;
        }));
}