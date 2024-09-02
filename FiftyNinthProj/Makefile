CXX = g++
CC = gcc
CXXFLAGS	:= -std=c++11 -m32 -w -O3 -g -static -DNANOVG_GLEW -DGLEW_STATIC -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -DLUA_COMPAT -DLUA_COMPAT_ALL -DHAVE_STDINT_H  -DHAVE_STDDEF_H -DZ_HAVE_UNISTD_H -DDNO_FSEEKO         
CCFLAGS		:= -std=c11 -m32 -w -O3 -g -static -DNANOVG_GLEW -DGLEW_STATIC -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -DLUA_COMPAT -DLUA_COMPAT_ALL -DHAVE_STDINT_H  -DHAVE_STDDEF_H -DZ_HAVE_UNISTD_H -DDNO_FSEEKO                   
# CCFLAGS		:= -std=c11 -m32 -w -O0 -g -static -DNANOVG_GLEW -DGLEW_STATIC -D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -D_WINDOWS -DWINDOWS -D_WIN32 -DWIN32 -D_WINNT=0x0400 -DLUA_COMPAT -DLUA_COMPAT_ALL -DMARKUP_STL -DUSE_MSDOS_MEMMGR  -fprofile-arcs -ftest-coverage                     
# LFLAGS = -lkernel32 -limm32 -lversion -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid  -lwinmm -lmingw32 -lMsimg32 -lopengl32 -lglu32 -lgdi32 -lgdiplus -ld3d9 -ddxguid -dd3d11 -lshlwapi -lWs2_32 -lIphlpapi 
LFLAGS = -fexec-charset=utf-8 -finput-charset=utf-8 -lkernel32 -limm32 -lversion -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid  -lwinmm -lmingw32 -lMsimg32 -lopengl32 -lglu32 -lgdi32 -lgdiplus -ld3d9 -ddxguid -dd3d11 -lshlwapi -lWs2_32 -lIphlpapi -lcomctl32 
# LFLAGS = -Wl,--subsystem,console -municode -mconsole -fexec-charset=gbk -finput-charset=gbk -lkernel32 -limm32 -lversion -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid  -lwinmm -lmingw32 -lMsimg32 -lopengl32 -lglu32 -lgdi32 -lgdiplus -ld3d9 -ddxguid -dd3d11 -lshlwapi -lWs2_32 -lIphlpapi 
# LFLAGS = -Wl,--subsystem,windows -municode -mwindows -lkernel32 -limm32 -lversion -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid  -lwinmm -lmingw32 -lMsimg32 -lopengl32 -lglu32 -lgdi32 -lgdiplus -ld3d9 -ddxguid -dd3d11 -lshlwapi -lWs2_32 -lIphlpapi
# LFLAGS = -Wl,--subsystem,console -municode -mconsole -lkernel32 -limm32 -lversion -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid  -lwinmm -lmingw32 -lMsimg32 -lopengl32 -lglu32 -lgdi32 -lgdiplus -ld3d9 -ddxguid -dd3d11 -lshlwapi -lWs2_32 -lIphlpapi
OUTPUT	:= output
SRC		:= src
INCLUDE	:= include
LIB		:= lib

ifeq ($(OS),Windows_NT)
MAIN	:= main.exe
SOURCEDIRS	:= $(SRC)
INCLUDEDIRS	:= $(INCLUDE)
LIBDIRS		:= $(LIB)
FIXPATH = $(subst /,\,$1)
# RM			:= del /s /q /f
RM			:= rm  -f
MD	:= mkdir
else
MAIN	:= main
SOURCEDIRS	:= $(shell find $(SRC) -type d)
INCLUDEDIRS	:= $(shell find $(INCLUDE) -type d)
LIBDIRS		:= $(shell find $(LIB) -type d)
FIXPATH = $1
RM = rm -f
MD	:= mkdir -p
endif

INCLUDES	:= $(patsubst %,-I%, $(INCLUDEDIRS:%/=%))
LIBS		:= $(patsubst %,-L%, $(LIBDIRS:%/=%))
CXXSOURCES		:= $(wildcard $(patsubst %,%/*.cpp, $(SOURCEDIRS)))
CXXOBJECTS		:= $(CXXSOURCES:.cpp=.o)
CCSOURCES		:= $(wildcard $(patsubst %,%/*.c, $(SOURCEDIRS)))
CCOBJECTS		:= $(CCSOURCES:.c=.o)
CXXDEPS		:= $(CXXOBJECTS:.o=.d)
CCDEPS		:= $(CCOBJECTS:.o=.d)

OUTPUTMAIN	:= $(call FIXPATH,$(OUTPUT)/$(MAIN))

all: $(OUTPUT) $(MAIN)
	@echo Executing 'all' complete!

$(OUTPUT):
	$(MD) $(OUTPUT)

$(MAIN): $(CXXOBJECTS)  $(CCOBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(OUTPUTMAIN) $(CXXOBJECTS) $(CCOBJECTS) $(LFLAGS) $(LIBS)
-include $(CXXDEPS)  $(CCDEPS)
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -MMD $<  -o $@
.c.o:
	$(CC) $(CCFLAGS) $(INCLUDES) -c -MMD $<  -o $@
.PHONY: clean
clean:
	$(RM) $(call FIXPATH,$(CXXOBJECTS))
	$(RM) $(call FIXPATH,$(CCOBJECTS))
	$(RM) $(call FIXPATH,$(CXXDEPS))
	$(RM) $(call FIXPATH,$(CCDEPS))
	$(RM) $(OUTPUTMAIN)
	@echo Cleanup complete!
run: all
	./$(OUTPUTMAIN)
	@echo Executing 'run: all' complete!




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