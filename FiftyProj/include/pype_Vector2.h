#pragma once

#include "pype_common.h"

using namespace pkpy;

struct Vector2 {
    PY_CLASS(Vector2, pype, Vector2)
    float x;
    float y;

    Vector2(float x, float y) : x(x), y(y) {}
    Vector2(px_point p) : x(p.x), y(p.y) {}

    operator px_point() const {
        return {x, y};
    }

    static void _register(VM* vm, PyObject* mod, PyObject* type){
        vm->bind_static_method<2>(type, "__new__", [](VM* vm, ArgsView args){
            float x = vm->num_to_float(args[0]);
            float y = vm->num_to_float(args[1]);
            return VAR_T(Vector2, x, y);
        });

        vm->bind_method<0>(type, "__repr__", [](VM* vm, ArgsView args){
            Vector2& self = CAST(Vector2&, args[0]);
            std::stringstream ss;
            ss << "Vector2(" << self.x << ", " << self.y << ")";
            return VAR(ss.str());
        });

        vm->bind_method<1>(type, "__add__", [](VM* vm, ArgsView args){
            Vector2& self = CAST(Vector2&, args[0]);
            Vector2& other = CAST(Vector2&, args[1]);
            return VAR_T(Vector2, self.x + other.x, self.y + other.y);
        });

        vm->bind_method<1>(type, "__sub__", [](VM* vm, ArgsView args){
            Vector2& self = CAST(Vector2&, args[0]);
            Vector2& other = CAST(Vector2&, args[1]);
            return VAR_T(Vector2, self.x - other.x, self.y - other.y);
        });

        vm->bind_method<1>(type, "__mul__", [](VM* vm, ArgsView args){
            Vector2& self = CAST(Vector2&, args[0]);
            f64 other = vm->num_to_float(args[1]);
            return VAR_T(Vector2, self.x * other, self.y * other);
        });

        vm->bind_method<1>(type, "__truediv__", [](VM* vm, ArgsView args){
            Vector2& self = CAST(Vector2&, args[0]);
            f64 other = vm->num_to_float(args[1]);
            return VAR_T(Vector2, self.x / other, self.y / other);
        });

        vm->bind_method<1>(type, "__eq__", [](VM* vm, ArgsView args){
            Vector2& self = CAST(Vector2&, args[0]);
            Vector2& other = CAST(Vector2&, args[1]);
            return VAR(self.x == other.x && self.y == other.y);
        });

        vm->bind_method<1>(type, "__ne__", [](VM* vm, ArgsView args){
            Vector2& self = CAST(Vector2&, args[0]);
            Vector2& other = CAST(Vector2&, args[1]);
            return VAR(self.x != other.x || self.y != other.y);
        });

        type->attr().set("x", vm->property([](VM* vm, ArgsView args){
            Vector2& self = _CAST(Vector2&, args[0]);
            return VAR(self.x);
        }, [](VM* vm, ArgsView args){
            Vector2& self = _CAST(Vector2&, args[0]);
            self.x = vm->num_to_float(args[1]);
            return vm->None;
        }));

        type->attr().set("y", vm->property([](VM* vm, ArgsView args){
            Vector2& self = _CAST(Vector2&, args[0]);
            return VAR(self.y);
        }, [](VM* vm, ArgsView args){
            Vector2& self = _CAST(Vector2&, args[0]);
            self.y = vm->num_to_float(args[1]);
            return vm->None;
        }));
    }
};


struct Rect{
    PY_CLASS(Rect, pype, Rect)

    float x;
    float y;
    float w;
    float h;

    Rect(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}
    Rect() {}

    float left() const { return x; }
    float top() const { return y; }
    float right() const { return x + w; }
    float bottom() const { return y + h; }
    
    static void _register(VM* vm, PyObject* mod, PyObject* type){
        vm->bind_static_method<-1>(type, "__new__", [](VM* vm, ArgsView args){
            if(args.size() == 2){
                Vector2& pos = CAST(Vector2&, args[0]);
                Vector2& size = CAST(Vector2&, args[1]);
                return VAR_T(Rect, pos.x, pos.y, size.x, size.y);
            }
            if(args.size() == 0){
                return VAR_T(Rect);
            }
            if(args.size() == 4){
                float x = vm->num_to_float(args[0]);
                float y = vm->num_to_float(args[1]);
                float w = vm->num_to_float(args[2]);
                float h = vm->num_to_float(args[3]);
                return VAR_T(Rect, x, y, w, h);
            }
            vm->TypeError("Rect.__new__ takes 2 or 4 arguments");
            return vm->None;
        });

        vm->bind_method<0>(type, "__repr__", [](VM* vm, ArgsView args){
            Rect& self = _CAST(Rect&, args[0]);
            std::stringstream ss;
            ss << "Rect(" << self.x << ", " << self.y << ", " << self.w << ", " << self.h << ")";
            return VAR(ss.str());
        });

        vm->bind_method<1>(type, "__eq__", [](VM* vm, ArgsView args){
            Rect& self = _CAST(Rect&, args[0]);
            Rect& other = CAST(Rect&, args[1]);
            return VAR(self.x == other.x && self.y == other.y && self.w == other.w && self.h == other.h);
        });

        vm->bind_method<1>(type, "__ne__", [](VM* vm, ArgsView args){
            Rect& self = _CAST(Rect&, args[0]);
            Rect& other = CAST(Rect&, args[1]);
            return VAR(self.x != other.x || self.y != other.y || self.w != other.w || self.h != other.h);
        });

        vm->bind_method<0>(type, "center", [](VM* vm, ArgsView args){
            Rect& self = _CAST(Rect&, args[0]);
            return VAR_T(Vector2, self.x + self.w / 2, self.y + self.h / 2);
        });

        vm->bind_method<0>(type, "size", [](VM* vm, ArgsView args){
            Rect& self = _CAST(Rect&, args[0]);
            return VAR_T(Vector2, self.w, self.h);
        });

#define PROPERTY_FLOAT(name)                                            \
        type->attr().set(#name, vm->property([](VM* vm, ArgsView args){ \
            Rect& self = _CAST(Rect&, args[0]);                         \
            return VAR(self.name);                                      \
        }, [](VM* vm, ArgsView args){                                   \
            Rect& self = _CAST(Rect&, args[0]);                         \
            self.x = vm->num_to_float(args[1]);                         \
            return vm->None;                                            \
        }));                                                            \

        PROPERTY_FLOAT(x);
        PROPERTY_FLOAT(y);
        PROPERTY_FLOAT(w);
        PROPERTY_FLOAT(h);

#undef PROPERTY_FLOAT
    }
};