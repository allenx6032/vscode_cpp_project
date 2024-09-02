#pragma once


#include "pype_common.h"
#include "pype_Node.h"
#include "pype_Texture2D.h"

inline void _register_image_type(VM* vm, PyObject* mod){
    PyObject* type = vm->new_type_object(mod, "Image", g_tp_node);
    vm->bind_method<0>(type, "_px_obj_init", [](VM* vm, ArgsView args){
        PX_Object* obj = PX_Object_ImageCreate(
            &App.runtime.mp_game,
            get_px_obj(g_root), // PX_Object *Parent,
            0,
            0,
            120,
            40,
            NULL                // texture
        );
        inject_py_object(obj, args[0]);
        return VAR(obj);
    });

    vm->bind_method<0>(type, "set_native_size", [](VM* vm, ArgsView args){
        PX_Object* obj = get_px_obj(args[0]);
        PX_Object_Image* image = PX_Object_GetImage(obj);
        obj->Width = image->pTexture->width;
        obj->Height = image->pTexture->height;
        return vm->None;
    });

    type->attr().set("texture", vm->property(
        [](VM* vm, ArgsView args){
            PX_Object* obj = get_px_obj(args[0]);
            PX_Object_Image* image = PX_Object_GetImage(obj);
            return VAR_T(Texture2D, image->pTexture);
        },
        [](VM* vm, ArgsView args){
            PX_Object* obj = get_px_obj(args[0]);
            PX_Object_Image* image = PX_Object_GetImage(obj);
            image->pTexture = CAST(Texture2D&, args[1]).ptr;
            return vm->None;
        }));
}