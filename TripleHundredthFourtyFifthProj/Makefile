
#------------------------------ plan A --------------------------------------------------
## SHELL=powershell
SHELL=cmd
CXX = g++
CXX = g++
CC = gcc
WINDRES = windres
ASMFLAGS	:=               
RCFLAGS		:= -O coff
CXXFLAGS	:= -std=c++14 -w -O3 -g -static -D_UNICODE -DUNICODE -D__UNICODE__ -DWIN32 -D_WIN32 -D__WIN32__ -D_WINDOWS -DWINDOWS -D__WINDOWS__ -D_GLFW_WIN32 -D_GLFW_WGL -D_GLFW_USE_OPENGL -DNANOVG_GLEW -DGLEW_STATIC -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -DLUA_COMPAT -DLUA_COMPAT_ALL -DLUA_COMPAT_MODULE -DLUA_COMPAT_5_3 -DLUA_COMPAT_5_2  -DFT2_BUILD_LIBRARY -DFT_CONFIG_OPTION_SYSTEM_ZLIB -DBUILDING_LIBCURL -DCURL_HIDDEN_SYMBOLS -DHAVE_CURL_CONFIG_H -DCURL_STATICLIB -DSFML_STATIC -DAL_BUILD_LIBRARY -DAL_LIBTYPE_STATIC -DAL_ALEXT_PROTOTYPES -DFLAC__CPU_X86_64 -DFLAC__HAS_X86INTRIN -DFLAC__ALIGN_MALLOC_DATA -DFLAC__NO_DLL -DFLAC__OVERFLOW_DETECT -DFLAC__HAS_OGG -DFLAC__ENABLE_64_BIT_WORDS  -DLOAD_PNG -DLOAD_JPG -DLOAD_WEBP -DLOAD_GIF -DCC_STATIC -DHAVE_STDINT_H -DBUILDING_WOLFSSL -DECC_SHAMIR -DECC_TIMING_RESISTANT -DGCM_TABLE_4BIT -DHAVE_AESGCM -DHAVE_CHACHA -DHAVE_DH_DEFAULT_PARAMS -DHAVE_ECC -DHAVE_ENCRYPT_THEN_MAC -DHAVE_EXTENDED_MASTER -DHAVE_FFDHE_2048 -DHAVE_HASHDRBG -DHAVE_HKDF -DHAVE_ONE_TIME_AUTH -DHAVE_POLY1305 -DHAVE_PTHREAD -DHAVE_SNI -DHAVE_SUPPORTED_CURVES -DHAVE_THREAD_LS -DHAVE_TLS_EXTENSIONS -DHAVE___UINT128_T -DNO_DES3 -DNO_DES3_TLS_SUITES -DNO_DSA -DNO_MD4 -DNO_PSK -DNO_RC4 -DOPENSSL_ALL -DTFM_ECC256 -DTFM_TIMING_RESISTANT -DWC_NO_ASYNC_THREADING -DWC_RSA_BLINDING -DWC_RSA_NO_PADDING -DWC_RSA_PSS -DWOLFSSL_BASE64_ENCODE -DWOLFSSL_CERT_NAME_ALL -DWOLFSSL_EITHER_SIDE -DWOLFSSL_ERROR_CODE_OPENSSL -DWOLFSSL_IGNORE_FILE_WARN -DWOLFSSL_NO_SHAKE128 -DWOLFSSL_NO_SHAKE256 -DWOLFSSL_PSS_LONG_SALT -DWOLFSSL_SHA224 -DWOLFSSL_SHA3 -DWOLFSSL_SHA384 -DWOLFSSL_SHA512 -DWOLFSSL_SYS_CA_CERTS -DWOLFSSL_TICKET_HAVE_ID -DWOLFSSL_TLS13 -DWOLFSSL_USE_ALIGN -DWOLFSSL_X86_64_BUILD -D_POSIX_THREADS  -DAL_BUILD_LIBRARY -DAL_ALEXT_PROTOTYPES -DCOCOS2D_DEBUG=1 -DCC_LUA_ENGINE_ENABLED=1 -DSTRICT -D_CRT_SECURE_NO_WARNINGS -DCC_USE_JSON=1 -DCC_USE_SQLITE=1 -DCC_USE_UNQLITE=1 -DCC_USE_PROTOBUF=1 -DCC_USE_SPROTO=1 -DCC_USE_CURL=1 -DCC_LUA_ENGINE_ENABLED=1 -DCC_ENABLE_CHIPMUNK_INTEGRATION=1 -DUSE_WIN32_CONSOLE=1 -D_SCL_SECURE_NO_WARNINGS_DEBUG -DCOCOS2D_DEBUG=1 -DGLFW_EXPOSE_NATIVE_WIN32 -DGLFW_EXPOSE_NATIVE_WGL -DCOCOS2DXWIN32_EXPORTS -DGL_GLEXT_PROTOTYPES -D_USEDLL -D_USE3DDLL -D_EXPORT_DLL_ -D_USRSTUDIODLL -D_USREXDLL -D_USEGUIDLL                                                                                                           
-DCC_ENABLE_CHIPMUNK_INTEGRATION=1      
CCFLAGS		:= -std=c11 -w -O3 -g -static -D_UNICODE -DUNICODE -D__UNICODE__ -DWIN32 -D_WIN32 -D__WIN32__ -D_WINDOWS -DWINDOWS -D__WINDOWS__ -D_GLFW_WIN32 -D_GLFW_WGL -D_GLFW_USE_OPENGL -DNANOVG_GLEW -DGLEW_STATIC -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -DLUA_COMPAT -DLUA_COMPAT_ALL -DLUA_COMPAT_MODULE -DLUA_COMPAT_5_3 -DLUA_COMPAT_5_2  -DFT2_BUILD_LIBRARY -DFT_CONFIG_OPTION_SYSTEM_ZLIB -DBUILDING_LIBCURL -DCURL_HIDDEN_SYMBOLS -DHAVE_CURL_CONFIG_H -DCURL_STATICLIB -DSFML_STATIC -DAL_BUILD_LIBRARY -DAL_LIBTYPE_STATIC -DAL_ALEXT_PROTOTYPES -DFLAC__CPU_X86_64 -DFLAC__HAS_X86INTRIN -DFLAC__ALIGN_MALLOC_DATA -DFLAC__NO_DLL -DFLAC__OVERFLOW_DETECT -DFLAC__HAS_OGG -DFLAC__ENABLE_64_BIT_WORDS  -DLOAD_PNG -DLOAD_JPG -DLOAD_WEBP -DLOAD_GIF -DCC_STATIC -DHAVE_STDINT_H -DBUILDING_WOLFSSL -DECC_SHAMIR -DECC_TIMING_RESISTANT -DGCM_TABLE_4BIT -DHAVE_AESGCM -DHAVE_CHACHA -DHAVE_DH_DEFAULT_PARAMS -DHAVE_ECC -DHAVE_ENCRYPT_THEN_MAC -DHAVE_EXTENDED_MASTER -DHAVE_FFDHE_2048 -DHAVE_HASHDRBG -DHAVE_HKDF -DHAVE_ONE_TIME_AUTH -DHAVE_POLY1305 -DHAVE_PTHREAD -DHAVE_SNI -DHAVE_SUPPORTED_CURVES -DHAVE_THREAD_LS -DHAVE_TLS_EXTENSIONS -DHAVE___UINT128_T -DNO_DES3 -DNO_DES3_TLS_SUITES -DNO_DSA -DNO_MD4 -DNO_PSK -DNO_RC4 -DOPENSSL_ALL -DTFM_ECC256 -DTFM_TIMING_RESISTANT -DWC_NO_ASYNC_THREADING -DWC_RSA_BLINDING -DWC_RSA_NO_PADDING -DWC_RSA_PSS -DWOLFSSL_BASE64_ENCODE -DWOLFSSL_CERT_NAME_ALL -DWOLFSSL_EITHER_SIDE -DWOLFSSL_ERROR_CODE_OPENSSL -DWOLFSSL_IGNORE_FILE_WARN -DWOLFSSL_NO_SHAKE128 -DWOLFSSL_NO_SHAKE256 -DWOLFSSL_PSS_LONG_SALT -DWOLFSSL_SHA224 -DWOLFSSL_SHA3 -DWOLFSSL_SHA384 -DWOLFSSL_SHA512 -DWOLFSSL_SYS_CA_CERTS -DWOLFSSL_TICKET_HAVE_ID -DWOLFSSL_TLS13 -DWOLFSSL_USE_ALIGN -DWOLFSSL_X86_64_BUILD -D_POSIX_THREADS   -DAL_BUILD_LIBRARY -DAL_ALEXT_PROTOTYPES -DCOCOS2D_DEBUG=1 -DCC_LUA_ENGINE_ENABLED=1  -DSTRICT -D_CRT_SECURE_NO_WARNINGS -DCC_USE_JSON=1 -DCC_USE_SQLITE=1 -DCC_USE_UNQLITE=1 -DCC_USE_PROTOBUF=1 -DCC_USE_SPROTO=1 -DCC_USE_CURL=1 -DCC_LUA_ENGINE_ENABLED=1 -DCC_ENABLE_CHIPMUNK_INTEGRATION=1 -DUSE_WIN32_CONSOLE=1 -D_SCL_SECURE_NO_WARNINGS_DEBUG -DCOCOS2D_DEBUG=1 -DGLFW_EXPOSE_NATIVE_WIN32 -DGLFW_EXPOSE_NATIVE_WGL -DCOCOS2DXWIN32_EXPORTS -DGL_GLEXT_PROTOTYPES -D_USEDLL -D_USE3DDLL -D_EXPORT_DLL_ -D_USRSTUDIODLL -D_USREXDLL -D_USEGUIDLL                                                                                                                  
## FLAG		:= -std=c++11 -m32 -w -O3 -g -static -fprofile-arcs -ftest-coverage -D_WIN32 -DWIN32 -D_DEBUG -DDEBUG -D_WINDOWS -DWINDOWS -D_UNICODE -DUNICODE -DNANOVG_GLEW -DGLEW_STATIC -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -DLUA_COMPAT -DLUA_COMPAT_ALL -DSK_IGNORE_ETC1_SUPPORT                   
LFLAGS = -lkernel32 -limm32 -lversion -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid  -lwinmm -lmingw32 -lMsimg32 -lopengl32 -lglu32 -lgdi32 -lgdiplus -ld3d9 -ldxguid -ld3d11 -lxinput -ldinput8 -ld2d1 -ldwrite -lwindowscodecs -lshlwapi -lWs2_32 -lIphlpapi -lcomctl32 -lpthread -ldsound -lddraw -lm -lbcrypt -lsetupapi -lsecur32 -lcrypt32  -lpsapi -luserenv -ld3dcompiler -ldwmapi  -lwsock32 
## LFLAGS = -municode -mwindows -fexec-charset=utf-8 -finput-charset=utf-8
## LFLAGS = -municode -mwindows -fexec-charset=utf-8 -finput-charset=utf-8
## LFLAGS = -Wl,--subsystem,console -municode -mconsole -fexec-charset=gbk -finput-charset=gbk
## LFLAGS = -Wl,--subsystem,windows -municode -mwindows
## LFLAGS = -Wl,--subsystem,console -municode -mconsole
OUTPUT	:= output
SRC		:= src 
INCLUDE	:= include
LIB		:= lib
RC		:= res
ASM		:= asm

ifeq ($(OS),Windows_NT)
MAIN	:= main.exe
SOURCEDIRS	:= $(SRC)
INCLUDEDIRS	:= $(INCLUDE)
LIBDIRS		:= $(LIB)
RCDIRS		:= $(RC)
ASMDIRS		:= $(ASM)
FIXPATH = $(subst /,\,$1)
RM			:= del
TARGET 		:=  /s /q /f
MD	:= mkdir
else
MAIN	:= main
SOURCEDIRS	:= $(shell find $(SRC) -type d)
INCLUDEDIRS	:= $(shell find $(INCLUDE) -type d)
LIBDIRS		:= $(shell find $(LIB) -type d)
RCDIRS		:= $(shell find $(RC) -type d)
ASMDIRS		:= $(shell find $(ASM) -type d)
FIXPATH = $1
RM = rm 
TARGET 		:= -f
MD	:= mkdir -p
endif

INCLUDES	:= $(patsubst %,-I%, $(INCLUDEDIRS:%/=%))
RCINCLUDES	:= $(patsubst %,-I%, $(RCDIRS:%/=%))
LIBS		:= $(patsubst %,-L%, $(LIBDIRS:%/=%))
ASMSOURCES		:= $(wildcard $(patsubst %,%/*.s, $(ASMDIRS)))
ASMOBJECTS		:= $(ASMSOURCES:.s=.o)
CXXSOURCES		:= $(wildcard $(patsubst %,%/*.cxx, $(SOURCEDIRS)))
CXXOBJECTS		:= $(CXXSOURCES:.cxx=.o)
CPPSOURCES		:= $(wildcard $(patsubst %,%/*.cpp, $(SOURCEDIRS)))
CPPOBJECTS		:= $(CPPSOURCES:.cpp=.o)
CCSOURCES		:= $(wildcard $(patsubst %,%/*.cc, $(SOURCEDIRS)))
CCOBJECTS		:= $(CCSOURCES:.cc=.o)
CSOURCES		:= $(wildcard $(patsubst %,%/*.c, $(SOURCEDIRS)))
COBJECTS		:= $(CSOURCES:.c=.o)
RCSOURCE 		:= $(wildcard $(patsubst %,%/*.rc, $(RCDIRS)))
RCOBJECTS		:= $(RCSOURCE:.rc=.o)
ASMDEPS		:= $(ASMOBJECTS:.o=.d)
CXXDEPS		:= $(CXXOBJECTS:.o=.d)
CPPDEPS		:= $(CPPOBJECTS:.o=.d)
CCDEPS		:= $(CCOBJECTS:.o=.d)
CDEPS		:= $(COBJECTS:.o=.d)
RCDEPS		:= $(RCOBJECTS:.o=.d)

OUTPUTMAIN	:= $(call FIXPATH,$(OUTPUT)/$(MAIN))

all: $(OUTPUT) $(MAIN)
	@echo Executing 'all' complete!

$(OUTPUT):
	$(MD) $(OUTPUT)

$(MAIN): $(ASMOBJECTS)  $(CPPOBJECTS) $(CXXOBJECTS) $(CCOBJECTS) $(COBJECTS) $(RCOBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(RCINCLUDES) -o $(OUTPUTMAIN) $(CPPOBJECTS) $(ASMOBJECTS) $(CXXOBJECTS) $(CCOBJECTS) $(COBJECTS) $(RCOBJECTS) $(LFLAGS) $(LIBS)
-include   $(CPPDEPS) $(CXXDEPS) $(CCDEPS) $(CDEPS) $(RCDEPS)
$(ASMDIRS)/%.o:$(ASMDIRS)/%.s
	$(CXX) $(ASMFLAGS) $(INCLUDES) $(RCINCLUDES) -c -MMD $<  -o $@
$(RCDIRS)/%.o:$(RCDIRS)/%.rc
	$(WINDRES) $(RCFLAGS) $< -o $@
%.o:%.cxx
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(RCINCLUDES) -c -MMD $<  -o $@
%.o:%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(RCINCLUDES) -c -MMD $<  -o $@
%.o:%.cc
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(RCINCLUDES) -c -MMD $<  -o $@
%.o:%.c
	$(CC) $(CCFLAGS) $(INCLUDES) $(RCINCLUDES) -c -MMD $<  -o $@ 
.PHONY: clean
clean:
	del $(call FIXPATH,src/*.o)
	del $(call FIXPATH,src/*.d)
	del $(call FIXPATH,res/*.d)
	del $(call FIXPATH,res/*.o)
	del $(call FIXPATH,asm/*.d)
	del $(call FIXPATH,asm/*.o)
	del $(OUTPUTMAIN)
	@echo Cleanup complete!
run: all
	./$(OUTPUTMAIN)
	@echo Executing 'run: all' complete!

#------------------------------ plan B --------------------------------------------------
# $(RM) $(TARGET) $(call FIXPATH,$(ASMOBJECTS))
# $(RM) $(TARGET) $(call FIXPATH,$(CXXOBJECTS))
# $(RM) $(TARGET) $(call FIXPATH,$(CPPOBJECTS))
# $(RM) $(TARGET) $(call FIXPATH,$(CCOBJECTS))
# $(RM) $(TARGET) $(call FIXPATH,$(COBJECTS))
# $(RM) $(TARGET) $(call FIXPATH,$(ASMDEPS))
# $(RM) $(TARGET) $(call FIXPATH,$(CXXDEPS))
# $(RM) $(TARGET) $(call FIXPATH,$(CPPDEPS))
# $(RM) $(TARGET) $(call FIXPATH,$(CCDEPS))
# $(RM) $(TARGET) $(call FIXPATH,$(CDEPS))
# del $(call FIXPATH,src/*.o)
# del $(call FIXPATH,src/*.d)
# del $(OUTPUTMAIN)

# build:
# 	gcc -std=c99 -Wunused-function -Wall -O2 -g -D_CRT_SECURE_NO_DEPRECATE -DNK_GLFW_GL2_IMPLEMENTATION -DIMGUI_API -DNANOVG_GLEW -DLUA_BUILD_AS_DLL -D_WINDOWS -D_GLFW_USE_CONFIG_H -DUNICODE -D_UNICODE -D_CRT_SECURE_NO_WARNINGS -DGLEW_STATIC -DLUA_COMPAT_ALL -Iinclude -c src/*.c 
# 	g++ -o output/main.exe *o -lkernel32 -limm32 -lversion -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid  -lgdi32 -lwinmm -lopengl32 -ldinput8 -lMsimg32 -lgdiplus -lshlwapi -ld3d9 -Llib
# # gcc -Wall -O2 -g -DIMGUI_API -DNANOVG_GLEW -DLUA_BUILD_AS_DLL -D_WINDOWS -D_GLFW_USE_CONFIG_H -DUNICODE -D_UNICODE -D_CRT_SECURE_NO_WARNINGS -DGLEW_STATIC -DLUA_COMPAT_ALL -Iinclude -c *.c 
# # gcc -o main.exe *.o -lkernel32 -limm32 -lversion -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid  -lgdi32 -lwinmm -lopengl32 -ldinput8 -lMsimg32 -lgdiplus -lshlwapi -ld3d9 -ldxguid -ld3d11  -ldxgi -ld3d12 -Llib
# # gcc -o output/main.exe *o -Wl,--subsystem,windows -municode -mwindows -lkernel32 -limm32 -lversion -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid  -lgdi32 -lwinmm -lopengl32 -ldinput8  -Llib
# # g++ -std=c++11 -Wall -g -DLUA_COMPAT_ALL  -Iinclude -c src/*.cpp
# # g++  -o output/main.exe *o -lkernel32 -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32 -lgdi32 -lwinmm -lopengl32  -Llib
# clean:
# 	rm *.o
# 	rm output/main
# run:
# 	output/main
# static:
#	ar -rcs libmain.a *.o

#------------------------------ plan c --------------------------------------------------
# ## 工具链定义
# CC      = gcc
# CXX     = g++
# WINDRES = windres
# AS      = nasm
# RM      = rm -f
# ## 目标配置
# TARGET  = app.exe
# MODE    = exe               # exe/lib/dll
# ARCH    = x86_64            # 可改为 i386
# ## 目录配置
# SRC_DIR = src
# ASM_DIR = asm
# RC_DIR  = res
# INC_DIR = include
# OBJ_DIR = obj
# LIB_DIR = lib
# BIN_DIR = output
# INCLUDES	:= $(patsubst %,-I%, $(INC_DIR:%/=%))
# LIBS		:= $(patsubst %,-L%, $(LIB_DIR:%/=%))
# ## 文件搜索
# C_SRCS   = $(wildcard $(patsubst %,%/*.c, $(SRC_DIR)))
# CPP_SRCS = $(wildcard $(patsubst %,%/*.cpp, $(SRC_DIR)))
# ## CXX_SRCS = $(wildcard $(patsubst %,%/*.cxx, $(SRC_DIR)))
# ## CC_SRCS = $(wildcard $(patsubst %,%/*.cc, $(SRC_DIR)))
# ASM_SRCS = $(wildcard $(ASM_DIR)/*.asm)
# RC_SRCS  = $(wildcard $(RC_DIR)/*.rc)
# ## 对象文件转换
# C_OBJS   = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(C_SRCS))
# CPP_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CPP_SRCS))
# ## CXX_OBJS = $(patsubst $(SRC_DIR)/%.cxx, $(OBJ_DIR)/%.o, $(CPP_SRCS))
# ## CC_OBJS = $(patsubst $(SRC_DIR)/%.cc, $(OBJ_DIR)/%.o, $(CPP_SRCS))
# ASM_OBJS = $(patsubst $(ASM_DIR)/%.asm, $(OBJ_DIR)/%.o, $(ASM_SRCS))
# RC_OBJS  = $(patsubst $(RC_DIR)/%.rc, $(OBJ_DIR)/%.o, $(RC_SRCS))
# ## 编译选项
# ##CFLAGS   = -Wall -O2 -I$(INC_DIR) -m$(ARCH)
# ##CXXFLAGS = $(CFLAGS) -std=c++11
# CXXFLAGS	:= -I$(INC_DIR),$(RC_DIR) -std=c++14 -w -O3 -g -static -DWIN32 -D_WIN32 -D__WIN32__ -D_WINDOWS -DWINDOWS -D__WINDOWS__ -D_GLFW_WIN32 -DNANOVG_GLEW -DGLEW_STATIC -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -DLUA_COMPAT -DLUA_COMPAT_ALL -DLUA_COMPAT_5_3 -DLUA_COMPAT_5_2  -DFT2_BUILD_LIBRARY -DFT_CONFIG_OPTION_SYSTEM_ZLIB -DBUILDING_LIBCURL -DCURL_HIDDEN_SYMBOLS -DHAVE_CURL_CONFIG_H -DCURL_STATICLIB -DSFML_STATIC -DAL_BUILD_LIBRARY -DAL_LIBTYPE_STATIC -DAL_ALEXT_PROTOTYPES -DFLAC__CPU_X86_64 -DFLAC__HAS_X86INTRIN -DFLAC__ALIGN_MALLOC_DATA -DFLAC__NO_DLL -DFLAC__OVERFLOW_DETECT -DFLAC__HAS_OGG -DFLAC__ENABLE_64_BIT_WORDS  -DLOAD_PNG -DLOAD_JPG -DLOAD_WEBP -DLOAD_GIF -DOGG_MUSIC -DMUSIC_MID_TIMIDITY -DMUSIC_MID_NATIVE -DMUSIC_OGG -DMUSIC_OPUS -DMUSIC_FLAC -DOPUS_CUSTOM_NOSTATIC -DOPUS_BUILD -DVAR_ARRAYS -DFPM_DEFAULT -DFIXED_POINT -DWIN32_LEAN_AND_MEAN             
# CFLAGS		:= -I$(INC_DIR),$(RC_DIR)  -std=c11 -w -O3 -g -static -DWIN32 -D_WIN32 -D__WIN32__ -D_WINDOWS -DWINDOWS -D__WINDOWS__ -D_GLFW_WIN32 -DNANOVG_GLEW -DGLEW_STATIC -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -DLUA_COMPAT -DLUA_COMPAT_ALL -DLUA_COMPAT_5_3 -DLUA_COMPAT_5_2  -DFT2_BUILD_LIBRARY -DFT_CONFIG_OPTION_SYSTEM_ZLIB -DBUILDING_LIBCURL -DCURL_HIDDEN_SYMBOLS -DHAVE_CURL_CONFIG_H -DCURL_STATICLIB -DSFML_STATIC -DAL_BUILD_LIBRARY -DAL_LIBTYPE_STATIC -DAL_ALEXT_PROTOTYPES -DFLAC__CPU_X86_64 -DFLAC__HAS_X86INTRIN -DFLAC__ALIGN_MALLOC_DATA -DFLAC__NO_DLL -DFLAC__OVERFLOW_DETECT -DFLAC__HAS_OGG -DFLAC__ENABLE_64_BIT_WORDS  -DLOAD_PNG -DLOAD_JPG -DLOAD_WEBP -DLOAD_GIF -DOGG_MUSIC -DMUSIC_MID_TIMIDITY -DMUSIC_MID_NATIVE -DMUSIC_OGG -DMUSIC_OPUS -DMUSIC_FLAC -DOPUS_CUSTOM_NOSTATIC -DOPUS_BUILD -DVAR_ARRAYS -DFPM_DEFAULT -DFIXED_POINT -DWIN32_LEAN_AND_MEAN               
# # ## FLAG		:= -std=c++11 -m32 -w -O3 -g -static -fprofile-arcs -ftest-coverage -D_WIN32 -DWIN32 -D_DEBUG -DDEBUG -D_WINDOWS -DWINDOWS -D_UNICODE -DUNICODE -DNANOVG_GLEW -DGLEW_STATIC -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -DLUA_COMPAT -DLUA_COMPAT_ALL -DSK_IGNORE_ETC1_SUPPORT                   
# LDFLAGS = -Wl,--subsystem,windows -municode -mwindows -fexec-charset=utf-8 -finput-charset=utf-8
# # ## LFLAGS = -municode -mwindows -fexec-charset=utf-8 -finput-charset=utf-8 -lkernel32 -limm32 -lversion -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid  -lwinmm -lmingw32 -lMsimg32 -lopengl32 -lglu32 -lgdi32 -lgdiplus -ld3d9 -ddxguid -dd3d11 -lshlwapi -lWs2_32 -lIphlpapi -lcomctl32 
# # ## LFLAGS = -municode -mwindows -fexec-charset=utf-8 -finput-charset=utf-8 -lkernel32 -limm32 -lversion -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid  -lwinmm -lmingw32 -lMsimg32 -lopengl32 -lglu32 -lgdi32 -lgdiplus -ld3d9 -ddxguid -dd3d11 -lshlwapi -lWs2_32 -lIphlpapi -lcomctl32 
# # ## LFLAGS = -Wl,--subsystem,console -municode -mconsole -fexec-charset=gbk -finput-charset=gbk -lkernel32 -limm32 -lversion -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid  -lwinmm -lmingw32 -lMsimg32 -lopengl32 -lglu32 -lgdi32 -lgdiplus -ld3d9 -ddxguid -dd3d11 -lshlwapi -lWs2_32 -lIphlpapi 
# # ## LFLAGS = -Wl,--subsystem,windows -municode -mwindows -lkernel32 -limm32 -lversion -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid  -lwinmm -lmingw32 -lMsimg32 -lopengl32 -lglu32 -lgdi32 -lgdiplus -ld3d9 -ddxguid -dd3d11 -lshlwapi -lWs2_32 -lIphlpapi
# # ## LFLAGS = -Wl,--subsystem,console -municode -mconsole -lkernel32 -limm32 -lversion -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid  -lwinmm -lmingw32 -lMsimg32 -lopengl32 -lglu32 -lgdi32 -lgdiplus -ld3d9 -ddxguid -dd3d11 -lshlwapi -lWs2_32 -lIphlpapi
# ASFLAGS  = -f elf64          # NASM 格式（64位）
# RCFLAGS  = -O coff --use-temp-file -F pe-$(ARCH)
# LIBS     = -lkernel32 -limm32 -lversion -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid  -lwinmm -lmingw32 -lMsimg32 -lopengl32 -lglu32 -lgdi32 -lgdiplus -ld3d9 -ldxguid -ld3d11 -lxinput -ldinput8 -ld2d1 -ldwrite -lwindowscodecs -lshlwapi -lWs2_32 -lIphlpapi -lcomctl32 -lpthread -ldsound -lddraw -lm -lbcrypt -lsetupapi -lsecur32 -lcrypt32  -lpsapi -luserenv
# ## 主规则
# all: setup $(BIN_DIR)/$(TARGET)
# ## 链接所有对象文件 $(CXX_OBJS) $(CC_OBJS)
# $(BIN_DIR)/$(TARGET): $(C_OBJS) $(CPP_OBJS) $(ASM_OBJS) $(RC_OBJS)
# 	$(CXX) $^ -o $@ $(LDFLAGS) $(LIBS)
# ## C 文件编译
# $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
# 	$(CC) $(CFLAGS) $(INCLUDES) -c -MMD $< -o $@
# ## C++ 文件编译
# $(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
# 	$(CXX) $(CXXFLAGS) -c -MMD $< -o $@
# # ## CXX 文件编译
# # $(OBJ_DIR)/%.o: $(SRC_DIR)/%.cxx
# # 	$(CXX) $(CXXFLAGS) -c -MMD $< -o $@
# # ## CC 文件编译
# # $(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc
# # 	$(CXX) $(CXXFLAGS) -c -MMD $< -o $@
# ## ASM 文件编译
# $(OBJ_DIR)/%.o: $(ASM_DIR)/%.asm
# 	$(AS) $(ASFLAGS) $< -o $@
# ## RC 文件编译
# $(OBJ_DIR)/%.o: $(RC_DIR)/%.rc
# 	$(WINDRES) $(RCFLAGS) $< $@
# ## 创建目录结构
# setup:
# 	@mkdir -p $(OBJ_DIR) $(BIN_DIR)
# ## 清理
# clean:
# 	$(RM) $(BIN_DIR)/$(TARGET) $(OBJ_DIR)/*.o
# .PHONY: all setup clean
