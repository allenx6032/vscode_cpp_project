#pragma once

#include "pype_common.h"
#include "pype_platform.h"
#include "pype_Vector2.h"
#include "pype_pocketpy.h"

using namespace pkpy;

inline PX_Object* _get_px_obj(PyObject* obj){
    static const StrName m_px_obj = "_px_obj";
    PyObject* px_obj = obj->attr(m_px_obj);
    return _CAST(PX_Object*, px_obj);
}

inline PX_Object* get_px_obj(PyObject* obj){
    vm->isinstance(obj, g_tp_node);
    return _get_px_obj(obj);
}

// struct PX_ChildrenIter: BaseIter{
//     PyObject* ref;
//     PX_Object* curr;

//     PX_ChildrenIter(VM* vm, PyObject* ref): BaseIter(vm){
//         this->ref = ref;
//         curr = get_px_obj(ref)->pChilds;
//     }

//     PyObject* next() override{
//         if(curr == NULL) return vm->StopIteration;
//         PyObject* ret = (PyObject*)curr->User_ptr;
//         curr = curr->pNextBrother;
//         return ret;
//     }

//     void _gc_mark() const override {
//         OBJ_MARK(ref);
//     }
// };


inline void traverse(PX_Object* obj, void (*f)(PX_Object*)){
    if(obj == NULL) return;
    // 层序遍历
    std::queue<PX_Object*> q;
    q.push(obj);
    while(!q.empty()){
        PX_Object* cur = q.front(); q.pop();
        f(cur);    // 调用自定义函数
        PX_Object* child = cur->pChilds;
        while(child != NULL){
            q.push(child);
            child = child->pNextBrother;
        }
    }
}


inline void inject_py_object(PX_Object* obj, PyObject* py_obj, bool update=true, bool render=false){
    PX_ObjectSetUserPointer(obj, py_obj);

    if(update){
        obj->Func_ObjectUpdate = [](PX_Object* obj, unsigned int _){
            PyObject* self = (PyObject*)obj->User_ptr;
            static StrName m_update = "_update";
            try{
                ::vm->call_method(self, m_update);
            }catch(Exception& e){
                log_error(e.summary());
                std::getchar();
            }
        };
    }

    if(render){
        obj->Func_ObjectRender = [](px_surface* psurface, PX_Object* obj, px_uint elpased){
            PyObject* self = (PyObject*)obj->User_ptr;
            static StrName m_draw = "_draw";
            PyObject* method = ::vm->get_unbound_method(self, m_draw, &self);
            try{
                if(self != PY_NULL) ::vm->call(method, self);
            }catch(Exception& e){
                log_error(e.summary());
                std::getchar();
            }
        };
    }
}

inline void _register_node_type(VM* vm, PyObject* mod, PyObject* type){
    vm->bind_method<0>(type, "_px_obj_init", [](VM* vm, ArgsView args){
        PX_Object* px_root = nullptr;
        if(g_root != nullptr) px_root = get_px_obj(g_root);
        PX_Object* obj = PX_ObjectCreate(&App.runtime.mp_game, px_root, 0, 0, 0, 0, 0, 0);
        inject_py_object(obj, args[0], true, true);
        return VAR(obj);
    });

    type->attr().set("position", vm->property(
        [](VM* vm, ArgsView args){
            PX_Object* obj = _get_px_obj(args[0]);
            return VAR_T(Vector2, obj->x, obj->y);
        },
        [](VM* vm, ArgsView args){
            PX_Object* obj = _get_px_obj(args[0]);
            Vector2& pos = CAST(Vector2&, args[1]);
            obj->x = pos.x;
            obj->y = pos.y;
            return vm->None;
        }));
    
    // x
    type->attr().set("x", vm->property(
        [](VM* vm, ArgsView args){
            PX_Object* obj = _get_px_obj(args[0]);
            return VAR(obj->x);
        },
        [](VM* vm, ArgsView args){
            PX_Object* obj = _get_px_obj(args[0]);
            obj->x = CAST(f64, args[1]);
            return vm->None;
        }));

    // y
    type->attr().set("y", vm->property(
        [](VM* vm, ArgsView args){
            PX_Object* obj = _get_px_obj(args[0]);
            return VAR(obj->y);
        },
        [](VM* vm, ArgsView args){
            PX_Object* obj = _get_px_obj(args[0]);
            obj->y = CAST(f64, args[1]);
            return vm->None;
        }));

    type->attr().set("parent", vm->property(
        [](VM* vm, ArgsView args){
            PX_Object* obj = _get_px_obj(args[0]);
            if(obj->pParent == NULL) return vm->None;
            PyObject* p = (PyObject*)(obj->pParent->User_ptr);
            if(p == g_root) return vm->None;
            return p;
        },
        [](VM* vm, ArgsView args){
            PX_Object* obj = _get_px_obj(args[0]);
            PX_Object* pParent;
            if(args[1] == vm->None){
                pParent = _get_px_obj(g_root);
            }else{
                pParent = _get_px_obj(args[1]);
            }
            if(obj == pParent) vm->ValueError("can't set parent to self");
            PX_ObjectSetParent(obj, pParent);
            return vm->None;
        }));

    type->attr().set("children", vm->property(
        [](VM* vm, ArgsView args){
            PX_Object* curr = _get_px_obj(args[0])->pChilds;
            List children;
            while(curr != NULL){
                children.push_back((PyObject*)curr->User_ptr);
                curr = curr->pNextBrother;
            }
            return VAR(Tuple(std::move(children)));
        }));

    type->attr().set("child_count", vm->property(
        [](VM* vm, ArgsView args){
            PX_Object* curr = _get_px_obj(args[0])->pChilds;
            int count = 0;
            while(curr != NULL){
                count++;
                curr = curr->pNextBrother;
            }
            return VAR(count);
        }));

    type->attr().set("width", vm->property(
        [](VM* vm, ArgsView args){
            PX_Object* obj = _get_px_obj(args[0]);
            return VAR(obj->Width);
        },
        [](VM* vm, ArgsView args){
            PX_Object* obj = _get_px_obj(args[0]);
            obj->Width = vm->num_to_float(args[1]);
            return vm->None;
        }));
    type->attr().set("height", vm->property(
        [](VM* vm, ArgsView args){
            PX_Object* obj = _get_px_obj(args[0]);
            return VAR(obj->Height);
        },
        [](VM* vm, ArgsView args){
            PX_Object* obj = _get_px_obj(args[0]);
            obj->Height = vm->num_to_float(args[1]);
            return vm->None;
        }));

    type->attr().set("enabled", vm->property(
        [](VM* vm, ArgsView args){
            PX_Object* obj = _get_px_obj(args[0]);
            return VAR(obj->Enabled != 0);
        },
        [](VM* vm, ArgsView args){
            PX_Object* obj = _get_px_obj(args[0]);
            obj->Enabled = (int)CAST(bool, args[1]);
            return vm->None;
        }));

    type->attr().set("visible", vm->property(
        [](VM* vm, ArgsView args){
            PX_Object* obj = _get_px_obj(args[0]);
            return VAR(obj->Visible != 0);
        },
        [](VM* vm, ArgsView args){
            PX_Object* obj = _get_px_obj(args[0]);
            obj->Visible = (int)CAST(bool, args[1]);
            return vm->None;
        }));
}