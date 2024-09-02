#pragma once

#include "pype_common.h"
#include "pype_easing.h"
#include "pype_gl.h"
#include "pype_Node.h"
#include "pype_Vector2.h"
#include "pype_Texture2D.h"
#include "pype_Input.h"
#include "pype_Font.h"

#include "pype_Button.h"
#include "pype_Label.h"
#include "pype_TextEdit.h"
#include "pype_Image.h"
#include "pype_ProgressBar.h"

using namespace pkpy;

inline void python_init(){
    vm = new VM(true);
    // 重定向sys.stdin和sys.stdout
    vm->_stdout = [](VM* _, const Str& s) { _platform_log_info(s); };
    vm->_stderr = [](VM* _, const Str& s) { _platform_log_error(s); };
    add_module_easing(vm);
    vm->bind_builtin_func<0>("input", [](VM* vm, ArgsView args){
        return VAR(pkpy::getline());
    });
    g_mod = vm->new_module("pype");
    GL::register_class(vm, g_mod);
    Vector2::register_class(vm, g_mod);
    Rect::register_class(vm, g_mod);
    Input::register_class(vm, g_mod);
    Texture2D::register_class(vm, g_mod);
    Color::register_class(vm, g_mod);
    PXFont::register_class(vm, g_mod);

    /*************全局私有函数*************/
    vm->bind_func<1>(g_mod, "_platform_read_bytes", [](VM* vm, ArgsView args){
        const Str& path = CAST(Str&, args[0]);
        return VAR(_read_file_cwd(path));
    });

    vm->bind_func<1>(g_mod, "_platform_list_dir", [](VM* vm, ArgsView args){
        return _platform_list_dir(args[0]);
    });

    vm->bind_func<1>(g_mod, "_PX_ObjectDelete", [](VM* vm, ArgsView args){
        PX_Object* obj = get_px_obj(args[0]);
        PX_ObjectDelete(obj);
        return vm->None;
    });

    vm->bind_func<1>(g_mod, "_PX_TextureCreateFromMemory", [](VM* vm, ArgsView args){
        const Bytes& content = CAST(Bytes&, args[0]);
        px_texture* tex = (px_texture*)malloc(sizeof(px_texture));
    	_PX_TextureCreateFromMemory(&App.runtime.mp_resources, (char*)content.data(), content.size(), tex);
        // Let `tex` leak on failure
        return VAR_T(Texture2D, tex);
    });

    vm->bind_func<1>(g_mod, "_PX_FontCreateFromMemory", [](VM* vm, ArgsView args){
        const Bytes& content = CAST(Bytes&, args[0]);
        PX_FontModule* fm = (PX_FontModule*)malloc(sizeof(PX_FontModule));
        PX_FontModuleInitialize(&App.runtime.mp_resources, fm);
        if(!PX_FontModuleLoad(fm, (px_byte*)content.data(), content.size())){
            free(fm);
            PXError("PX_FontModuleLoad() 失败 size=" + std::to_string(content.size()));
            return vm->None;
        }
        return VAR_T(PXFont, fm);
    });

    vm->bind_func<1>(g_mod, "_PX_WorldObjectXYtoScreenXY", [](VM* vm, ArgsView args){
        const Vector2& vec = CAST(Vector2&, args[0]);
        px_point ret = PX_WorldObjectXYtoScreenXY(&World, vec.x, vec.y);
        return VAR_T(Vector2, ret.x, ret.y);
    });

    vm->bind_func<1>(g_mod, "set_camera_position", [](VM* vm, ArgsView args){
        const Vector2& vec = CAST(Vector2&, args[0]);
        PX_WorldSetCamera(&World, vec);
        return vm->None;
    });

    vm->bind_func<0>(g_mod, "memory_usage", [](VM* vm, ArgsView args){
        auto f = [](px_memorypool* mp){
            size_t used = mp->Size - mp->FreeSize;
            double percent = (double)used / mp->Size * 100;
            double used_mb = used/1024.0/1024.0;
            double total_mb = mp->Size/1024.0/1024.0;
            percent = (int)(percent * 100 + 0.5) / 100.0;
            used_mb = (int)(used_mb * 100 + 0.5) / 100.0;
            total_mb = (int)(total_mb * 100 + 0.5) / 100.0;
            return fmt(used_mb, " / ", total_mb, " MB\t(", percent, "%)");
        };
        std::stringstream ss;
        ss << "TOTAL:     " << f(&App.runtime.mp) << std::endl;
        ss << "- UI:        " << f(&App.runtime.mp_ui) << std::endl;
        ss << "- RESOURCES: " << f(&App.runtime.mp_resources) << std::endl;
        ss << "- GAME:      " << f(&App.runtime.mp_game) << std::endl;
        double vm_usage = memory_usage()/1024.0/1024.0;
        vm_usage = (int)(vm_usage * 100 + 0.5) / 100.0;
        ss << "Python VM: " << vm_usage << " MB";
        log_info(ss.str());
        return vm->None;
    });

    // 注册Python库源码
    for(auto it = pe::kPythonLibs.begin(); it != pe::kPythonLibs.end(); ++it){
        CodeObject_ code = vm->compile(
            it->second,
            fmt("<", it->first.substr(2), ">"),
            EXEC_MODE
        );
        vm->_exec(code, g_mod);
    }

    // 初始化Node基类
    g_tp_node = OBJ_GET(Type, g_mod->attr("Node"));
    _register_node_type(vm, g_mod, vm->_t(g_tp_node));

    // 注册其他类
    _register_button_type(vm, g_mod);
    _register_label_type(vm, g_mod);
    _register_textedit_type(vm, g_mod);
    _register_image_type(vm, g_mod);
    _register_progressbar_type(vm, g_mod);

    // 创建根对象
    g_root = vm->call(vm->_t(g_tp_node));
    g_mod->attr().set("_root", g_root);
    PX_Object* px_root = get_px_obj(g_root);

    // 将根对象加入世界
    PX_WorldAddObject(&World, px_root);
    // 设置相机指向(0, 0)
    PX_WorldSetCamera(&World, px_point{0, 0, 0});
    // 创建GC代理，该代理用于对象树标记
    vm->_gc_marker_ex = [](VM* vm){
        PX_Object* px_root = _get_px_obj(g_root);
        traverse(px_root, [](PX_Object* obj){
            OBJ_MARK((PyObject*)obj->User_ptr);
        });
    };
}


// 帧率控制
struct FrameCounter{
	px_dword update_freq = 1000 / 50;
	px_dword accum_time = 0;
    px_dword elapsed = 0;
	bool flag = false;

    void set_frame_rate(int fps){
        update_freq = 1000 / fps;
    }

	bool do_update(px_dword* delta){
		if(flag) throw std::runtime_error("do_update() called twice in one frame");
		accum_time += *delta;
		if(accum_time >= update_freq){
            elapsed = accum_time;
            *delta = elapsed;
			accum_time = 0;
			flag = true;
		}
		return flag;
	}

	bool do_render(px_dword* delta){
		if(flag){
			flag = false;
            *delta = elapsed;
			return true;
		}
		return false;
	}
};

inline FrameCounter g_frame_counter;