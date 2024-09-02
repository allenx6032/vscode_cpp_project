#pragma once

#include "pype_common.h"

using namespace pkpy;

#ifdef __ANDROID__

#include <android/asset_manager.h>
#include <android/log.h>

extern AAssetManager* PX_assetManager;

inline void _platform_hook_read_file_cwd(VM* vm){
    auto f = [](const Str& path){
        std::string cpath = path.str();
        AAsset* asset = AAssetManager_open(PX_assetManager, cpath.c_str(), 0);
        if(!asset) return Bytes();
        int size = AAsset_getLength(asset);
        std::vector<char> buffer(size);
        AAsset_read(asset, buffer.data(), size);
        AAsset_close(asset);
        return Bytes(std::move(buffer));
    };
    set_read_file_cwd(f);
}

inline PyObject* _platform_list_dir(PyObject* path){
    std::string cpath = CAST(Str&, path).str();
    // 删除这些字符，保持兼容性
    if(!cpath.empty() && cpath[0]=='.') cpath.erase(cpath.begin());
    if(!cpath.empty() && cpath[0]=='/') cpath.erase(cpath.begin());
    AAssetDir* dir = AAssetManager_openDir(PX_assetManager, cpath.c_str());
    if(!dir) vm->IOError("AAssetManager_openDir() 调用失败");
    List list;
    const char* name = nullptr;
    while((name = AAssetDir_getNextFileName(dir)) != nullptr){
        list.push_back(VAR(name));
    }
    AAssetDir_close(dir);
    return VAR(list);
}

inline void _platform_log_info(const Str& msg){
    std::string cmsg = msg.str();
    __android_log_print(ANDROID_LOG_INFO, "pype", "%s", cmsg.c_str());
}

inline void _platform_log_error(const Str& msg){
    std::string cmsg = msg.str();
    __android_log_print(ANDROID_LOG_ERROR, "pype", "%s", cmsg.c_str());
}

#else

inline void _platform_hook_read_file_cwd(VM* vm){}

inline PyObject* _platform_list_dir(PyObject* path){
    static StrName m_os = "os";
    static StrName m_listdir = "listdir";
    PyObject* f = vm->_modules[m_os]->attr(m_listdir);
    return vm->call(f, path);
}

inline void _platform_log_info(const Str& msg){
    std::cout << msg.str();
}

inline void _platform_log_error(const Str& msg){
    std::cerr << msg.str();
}

#endif

namespace pkpy{
    inline void log_error(const Str& msg){
        _platform_log_error(msg + "\n");
    }

    inline void log_info(const Str& msg){
        _platform_log_info(msg + "\n");
    }
};