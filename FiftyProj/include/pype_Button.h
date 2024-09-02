#pragma once

#include "pype_common.h"
#include "pype_Node.h"

inline void _register_button_type(VM* vm, PyObject* mod){
    PyObject* type = mod->attr("Button");
    vm->bind_method<0>(type, "_px_obj_init", [](VM* vm, ArgsView args){
        PX_Object* obj = PX_Object_PushButtonCreate(
            &App.runtime.mp_game,
            get_px_obj(g_root), // PX_Object *Parent,
            0,
            0,
            120,
            40,
            "Button",             // const px_char *Text
            NULL                // PX_FontModule *fontmodule
        );
        inject_py_object(obj, args[0]);
        PX_ObjectRegisterEvent(obj, PX_OBJECT_EVENT_EXECUTE, [](PX_Object* obj, PX_Object_Event e, void* _){
            PyObject* py_obj = (PyObject*)obj->User_ptr;
            static const StrName m_on_click = "on_click";
            ::vm->call_method(py_obj, m_on_click);
        }, PX_NULL);
        return VAR(obj);
    });

    type->attr().set("text", vm->property(
        [](VM* vm, ArgsView args){
            PX_Object* obj = get_px_obj(args[0]);
            char* value = PX_Object_PushButtonGetText(obj);
            return VAR(Str(value));
        },
        [](VM* vm, ArgsView args){
            PX_Object* obj = get_px_obj(args[0]);
            char* value = CAST(Str&, args[1]).c_str_dup();
            PX_Object_PushButtonSetText(obj, value);
            free(value);
            return vm->None;
        }));
}