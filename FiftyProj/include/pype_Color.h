#pragma once

#include "pype_common.h"
#include "PX_Typedef.h"

using namespace pkpy;

// Color是不可变类型
struct Color{
    PY_CLASS(Color, pype, Color)

    px_color clr;
    Color(px_uchar r, px_uchar g, px_uchar b, px_uchar a) : clr(PX_COLOR(a,r,g,b)) {}
    Color(const px_color& c) : clr(c) {}

    bool _cached_float_clr = false;
    float _r, _g, _b, _a;

    PX_TEXTURERENDER_BLEND* blend() {
        if(!_cached_float_clr){
            _r = clr._argb.r / 255.0f;
            _g = clr._argb.g / 255.0f;
            _b = clr._argb.b / 255.0f;
            _a = clr._argb.a / 255.0f;
            _cached_float_clr = true;
        }
        return (PX_TEXTURERENDER_BLEND*)&_r;
    }

    operator px_color&() { return clr; }

    px_uchar r() { return clr._argb.r; }
    px_uchar g() { return clr._argb.g; }
    px_uchar b() { return clr._argb.b; }
    px_uchar a() { return clr._argb.a; }

    static void _register(VM* vm, PyObject* mod, PyObject* type){
        vm->bind_static_method<-1>(type, "__new__", [](VM* vm, ArgsView args){
            if(args.size()!=3 && args.size()!=4){
                vm->TypeError("Color() takes 3 or 4 arguments");
            }
            px_uchar r = CAST(unsigned char, args[0]);
            px_uchar g = CAST(unsigned char, args[1]);
            px_uchar b = CAST(unsigned char, args[2]);
            px_uchar a = args.size()==4 ? CAST(unsigned char, args[3]) : 255;
            return VAR_T(Color, r, g, b, a);
        });

        vm->bind_method<0>(type, "__repr__", [](VM* vm, ArgsView args){
            Color& self = _CAST(Color&, args[0]);
            std::stringstream ss;
            ss << "Color(" << self.r() << ", " << self.g() << ", " << self.b() << ", " << self.a() << ")";
            return VAR(ss.str());
        });

        vm->bind_method<1>(type, "__add__", [](VM* vm, ArgsView args){
            Color& self = _CAST(Color&, args[0]);
            Color& other = CAST(Color&, args[1]);
            return VAR_T(Color, PX_ColorAdd(self.clr, other.clr));
        });

        vm->bind_method<1>(type, "__sub__", [](VM* vm, ArgsView args){
            Color& self = _CAST(Color&, args[0]);
            Color& other = CAST(Color&, args[1]);
            return VAR_T(Color, PX_ColorSub(self.clr, other.clr));
        });

        vm->bind_method<1>(type, "__mul__", [](VM* vm, ArgsView args){
            Color& self = _CAST(Color&, args[0]);
            float other = vm->num_to_float(args[1]);
            return VAR_T(Color, self.r()*other, self.g()*other, self.b()*other, self.a()*other);
        });

        vm->bind_method<1>(type, "__truediv__", [](VM* vm, ArgsView args){
            Color& self = _CAST(Color&, args[0]);
            float other = vm->num_to_float(args[1]);
            return VAR_T(Color, self.r()/other, self.g()/other, self.b()/other, self.a()/other);
        });

        vm->bind_method<1>(type, "__eq__", [](VM* vm, ArgsView args){
            Color& self = _CAST(Color&, args[0]);
            Color& other = CAST(Color&, args[1]);
            return VAR(self.clr._argb.ucolor == other.clr._argb.ucolor);
        });

        vm->bind_method<1>(type, "__ne__", [](VM* vm, ArgsView args){
            Color& self = _CAST(Color&, args[0]);
            Color& other = CAST(Color&, args[1]);
            return VAR(self.clr._argb.ucolor != other.clr._argb.ucolor);
        });

        PyObject* colors = vm->new_type_object(mod, "Colors", vm->tp_object);
        colors->attr().set("White", VAR_T(Color, 255, 255, 255, 255));
        colors->attr().set("Black", VAR_T(Color, 0, 0, 0, 255));
        colors->attr().set("Red", VAR_T(Color, 255, 0, 0, 255));
        colors->attr().set("Green", VAR_T(Color, 0, 255, 0, 255));
        colors->attr().set("Blue", VAR_T(Color, 0, 0, 255, 255));
        colors->attr().set("Yellow", VAR_T(Color, 255, 255, 0, 255));
        colors->attr().set("Cyan", VAR_T(Color, 0, 255, 255, 255));
        colors->attr().set("Magenta", VAR_T(Color, 255, 0, 255, 255));
        colors->attr().set("Transparent", VAR_T(Color, 0, 0, 0, 0));
    }
};
