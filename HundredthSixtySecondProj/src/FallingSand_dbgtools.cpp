
#include "FallingSand_dbgtools.h"

#include <cstdio>
#include <cstring>

namespace ME {

#if defined(METADOT_DBG_CALLSTACK_UNIX) || defined(_MSC_VER)
typedef struct {
    char* out_ptr;
    const char* end_ptr;
} callstack_string_buffer_t;

static const char* alloc_string(callstack_string_buffer_t* buf, const char* str, size_t str_len) {
    char* res;

    if ((size_t)(buf->end_ptr - buf->out_ptr) < str_len + 1) return "<callstack buffer out of space!>";

    res = buf->out_ptr;
    buf->out_ptr += str_len + 1;
    memcpy(res, str, str_len);
    res[str_len] = '\0';
    return res;
}
#endif

#if defined(_MSC_VER)
#if defined(__clang__)
// when compiling with clang on windows, silence warnings from windows-code
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma clang diagnostic ignored "-Wignored-pragma-intrinsic"
#endif
#include <Windows.h>
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#include <Dbghelp.h>

int callstack(int skip_frames, void** addresses, int num_addresses) { return RtlCaptureStackBackTrace(skip_frames + 1, num_addresses, addresses, 0); }

typedef BOOL(__stdcall* SymInitialize_f)(_In_ HANDLE hProcess, _In_opt_ PCSTR UserSearchPath, _In_ BOOL fInvadeProcess);
typedef BOOL(__stdcall* SymFromAddr_f)(_In_ HANDLE hProcess, _In_ DWORD64 Address, _Out_opt_ PDWORD64 Displacement, _Inout_ PSYMBOL_INFO Symbol);
typedef BOOL(__stdcall* SymGetLineFromAddr64_f)(_In_ HANDLE hProcess, _In_ DWORD64 qwAddr, _Out_ PDWORD pdwDisplacement, _Out_ PIMAGEHLP_LINE64 Line64);
typedef DWORD(__stdcall* SymSetOptions_f)(_In_ DWORD SymOptions);

static struct {
    int initialized;
    int init_ok;
    SymInitialize_f SymInitialize;
    SymFromAddr_f SymFromAddr;
    SymGetLineFromAddr64_f SymGetLineFromAddr64;
    SymSetOptions_f SymSetOptions;
} dbghelp = {0, 0, 0, 0, 0, 0};

static HMODULE callstack_symbols_find_dbghelp() {
    // ... try to find dbghelp.dll in the same directory as the executable ...
    HMODULE mod;
    DWORD module_path_len;
    char module_path[4096];

    module_path_len = GetModuleFileName((HMODULE)0, (LPWSTR)module_path, sizeof(module_path));
    if (module_path_len > 0) {
        char* slash = strrchr(module_path, '\\');
        if (slash) *(slash + 1) = '\0';
        strncat(module_path, "dbghelp.dll", sizeof(module_path) - strlen(module_path) - 1);

        mod = LoadLibrary((LPWSTR)module_path);
        if (mod) return mod;
    }

    // ... try to find dbghelp.dll in the current working directory ...
    module_path_len = GetCurrentDirectory(sizeof(module_path), (LPWSTR)module_path);
    if (module_path_len > 0) {
        strncat(module_path, "\\dbghelp.dll", sizeof(module_path) - strlen(module_path) - 1);

        mod = LoadLibrary((LPWSTR)module_path);
        if (mod) return mod;
    }

    // ... fallback to the "normal" search-paths ...
    return LoadLibrary(L"dbghelp.dll");
}

static void callstack_symbols_build_search_path(char* search_path, int length) {
    // by default dbghelp will build a search-path by concatenating:
    // - The current working directory of the application
    // - The _NT_SYMBOL_PATH environment variable
    // - The _NT_ALTERNATE_SYMBOL_PATH environment variable
    //
    // let's do the same thing but also add the directory where the running
    // module lives.
    DWORD mod;
    char env_var[4096];

    search_path[0] = '\0';

    mod = GetModuleFileName((HMODULE)0, (LPWSTR)search_path, length);
    if (mod > 0) {
        char* slash = strrchr(search_path, '\\');
        if (slash) *slash = '\0';
    } else {
        search_path[0] = '\0';
    }

    if (length > 2) {
        if (search_path[0] != '\0') strncat(search_path, ";", length);
        strncat(search_path, ".", length);
    }

    mod = GetEnvironmentVariable(L"_NT_SYMBOL_PATH", (LPWSTR)env_var, sizeof(env_var));
    if (mod > 0 && mod < sizeof(env_var)) {
        if (search_path[0] != '\0') strncat(search_path, ";", length);
        strncat(search_path, env_var, length);
    }

    mod = GetEnvironmentVariable(L"_NT_ALTERNATE_SYMBOL_PATH", (LPWSTR)env_var, sizeof(env_var));
    if (mod > 0 && mod < sizeof(env_var)) {
        if (search_path[0] != '\0') strncat(search_path, ";", length);
        strncat(search_path, env_var, length);
    }
}

static int callstack_symbols_initialize() {
    HANDLE process;
    HMODULE mod;
    BOOL res;
    DWORD err;

    if (!dbghelp.initialized) {
        mod = callstack_symbols_find_dbghelp();
        if (mod) {
            dbghelp.SymInitialize = (SymInitialize_f)GetProcAddress(mod, "SymInitialize");
            dbghelp.SymFromAddr = (SymFromAddr_f)GetProcAddress(mod, "SymFromAddr");
            dbghelp.SymGetLineFromAddr64 = (SymGetLineFromAddr64_f)GetProcAddress(mod, "SymGetLineFromAddr64");
            dbghelp.SymSetOptions = (SymSetOptions_f)GetProcAddress(mod, "SymSetOptions");
        }

        // ... symbols fetched ok ...
        if (dbghelp.SymInitialize && dbghelp.SymFromAddr && dbghelp.SymGetLineFromAddr64 && dbghelp.SymSetOptions) {
            // ... enable debug-output if needed ...
            if (GetEnvironmentVariable(L"DBGTOOLS_SYMBOL_DEBUG_OUTPUT", 0, 0) > 0) dbghelp.SymSetOptions(SYMOPT_DEBUG);

            // ... generate the same search-paths for pdbs as default, just add our executable directory as well ...
            char search_path[4096];
            callstack_symbols_build_search_path(search_path, sizeof(search_path) - 1);

            // ... initialize sym system ...
            process = GetCurrentProcess();
            res = dbghelp.SymInitialize(process, search_path, TRUE);

            dbghelp.init_ok = 1;
            if (res == 0) {
                err = GetLastError();
                // ERROR_INVALID_PARAMETER seems to be returned IF symbols for a specific module could not be loaded.
                // However the lookup will still work for all other symbols so let us ignore this error!
                if (err != ERROR_INVALID_PARAMETER) dbghelp.init_ok = 0;
            }
        }
    }

    dbghelp.initialized = 1;
    return dbghelp.init_ok;
}

int callstack_symbols(void** addresses, callstack_symbol_t* out_syms, int num_addresses, char* memory, int mem_size) {
    HANDLE process;
    DWORD64 offset;
    DWORD line_dis;
    BOOL res;
    IMAGEHLP_LINE64 line;
    PSYMBOL_INFO sym_info;
    char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
    int num_translated = 0;
    callstack_string_buffer_t outbuf = {memory, memory + mem_size};

    memset(out_syms, 0x0, (size_t)num_addresses * sizeof(callstack_symbol_t));

    if (!callstack_symbols_initialize()) {
        out_syms[0].function = "failed to initialize dbghelp.dll";
        out_syms[0].offset = 0;
        out_syms[0].file = "";
        out_syms[0].line = 0;
        return 1;
    }

    process = GetCurrentProcess();
    sym_info = (PSYMBOL_INFO)buffer;
    sym_info->SizeOfStruct = sizeof(SYMBOL_INFO);
    sym_info->MaxNameLen = MAX_SYM_NAME;

    line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

    for (int i = 0; i < num_addresses; ++i) {
        res = dbghelp.SymFromAddr(process, (DWORD64)addresses[i], &offset, sym_info);
        if (res == 0)
            out_syms[i].function = "failed to lookup symbol";
        else
            out_syms[i].function = alloc_string(&outbuf, sym_info->Name, sym_info->NameLen);

        res = dbghelp.SymGetLineFromAddr64(process, (DWORD64)addresses[i], &line_dis, &line);
        if (res == 0) {
            out_syms[i].offset = 0;
            out_syms[i].file = "failed to lookup file";
            out_syms[i].line = 0;
        } else {
            out_syms[i].offset = (unsigned int)line_dis;
            out_syms[i].file = alloc_string(&outbuf, line.FileName, strlen(line.FileName));
            out_syms[i].line = (unsigned int)line.LineNumber;
        }

        ++num_translated;
    }
    return num_translated;
}

#else

int callstack(int skip_frames, void** addresses, int num_addresses) {
    (void)skip_frames;
    (void)addresses;
    (void)num_addresses;
    return 0;
}

int callstack_symbols(void** addresses, callstack_symbol_t* out_syms, int num_addresses, char* memory, int mem_size) {
    (void)addresses;
    (void)out_syms;
    (void)num_addresses;
    (void)memory;
    (void)mem_size;
    return 0;
}

#endif

#if defined(METADOT_DBG_CALLSTACK_UNIX)
#undef METADOT_DBG_CALLSTACK_UNIX
#endif

void print_callstack() {
    void* addresses[256];
    int num_addresses = callstack(0, addresses, 256);

    callstack_symbol_t symbols[256];
    char symbols_buffer[2048];
    num_addresses = callstack_symbols(addresses, symbols, num_addresses, symbols_buffer, 2048);

    int i;
    for (i = 0; i < num_addresses; ++i) printf("%3d) %-50s %s(%u)\n", i, symbols[i].function, symbols[i].file, symbols[i].line);
}

}  // namespace ME