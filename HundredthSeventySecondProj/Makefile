# SHELL=powershell
SHELL=cmd
CXX = g++
CXX = g++
CC = gcc
ASMFLAGS	:=               
CXXFLAGS	:= -std=c++14 -w -O3 -g -static -DUNICODE -D_UNICODE -DWIN32 -D_WIN32 -D__WIN32__ -D_GLFW_WIN32 -DNANOVG_GLEW -DGLEW_STATIC -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -DLUA_COMPAT -DLUA_COMPAT_ALL -DLUA_COMPAT_5_3 -DLUA_COMPAT_5_2 -DLUA_COMPAT_5_1 -DFT2_BUILD_LIBRARY -DFT_CONFIG_OPTION_SYSTEM_ZLIB                
CCFLAGS		:= -std=c11 -w -O3 -g -static -DUNICODE -D_UNICODE -DWIN32 -D_WIN32 -D__WIN32__ -D_GLFW_WIN32 -DNANOVG_GLEW -DGLEW_STATIC -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -DLUA_COMPAT -DLUA_COMPAT_ALL -DLUA_COMPAT_5_3 -DLUA_COMPAT_5_2 -DLUA_COMPAT_5_1 -DFT2_BUILD_LIBRARY -DFT_CONFIG_OPTION_SYSTEM_ZLIB                 
# FLAG		:= -std=c++11 -m32 -w -O3 -g -static -fprofile-arcs -ftest-coverage -D_WIN32 -DWIN32 -D_DEBUG -DDEBUG -D_WINDOWS -DWINDOWS -D_UNICODE -DUNICODE -DNANOVG_GLEW -DGLEW_STATIC -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -DLUA_COMPAT -DLUA_COMPAT_ALL -DSK_IGNORE_ETC1_SUPPORT                   
LFLAGS = -lkernel32 -limm32 -lversion -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid  -lwinmm -lmingw32 -lMsimg32 -lopengl32 -lglu32 -lgdi32 -lgdiplus -ld3d9 -ldxguid -ld3d11 -lxinput -ldinput8 -ld2d1 -ldwrite -lwindowscodecs -lshlwapi -lWs2_32 -lIphlpapi -lcomctl32 -lpthread -ldsound -lddraw -lm -lbcrypt -lsetupapi -lsecur32 -lcrypt32  -lpsapi -luserenv
# LFLAGS = -municode -mwindows -fexec-charset=utf-8 -finput-charset=utf-8 -lkernel32 -limm32 -lversion -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid  -lwinmm -lmingw32 -lMsimg32 -lopengl32 -lglu32 -lgdi32 -lgdiplus -ld3d9 -ddxguid -dd3d11 -lshlwapi -lWs2_32 -lIphlpapi -lcomctl32 
# LFLAGS = -municode -mwindows -fexec-charset=utf-8 -finput-charset=utf-8 -lkernel32 -limm32 -lversion -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid  -lwinmm -lmingw32 -lMsimg32 -lopengl32 -lglu32 -lgdi32 -lgdiplus -ld3d9 -ddxguid -dd3d11 -lshlwapi -lWs2_32 -lIphlpapi -lcomctl32 
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
RM			:= del
TARGET 		:=  /s /q /f
# RM			:= rm 
# TARGET 		:= -f
MD	:= mkdir
else
MAIN	:= main
SOURCEDIRS	:= $(shell find $(SRC) -type d)
INCLUDEDIRS	:= $(shell find $(INCLUDE) -type d)
LIBDIRS		:= $(shell find $(LIB) -type d)
FIXPATH = $1
RM = rm 
TARGET 		:= -f
MD	:= mkdir -p
endif

INCLUDES	:= $(patsubst %,-I%, $(INCLUDEDIRS:%/=%))
LIBS		:= $(patsubst %,-L%, $(LIBDIRS:%/=%))
ASMSOURCES		:= $(wildcard $(patsubst %,%/*.s, $(SOURCEDIRS)))
ASMOBJECTS		:= $(ASMSOURCES:.s=.o)

CXXSOURCES		:= $(wildcard $(patsubst %,%/*.cxx, $(SOURCEDIRS)))
CXXOBJECTS		:= $(CXXSOURCES:.cxx=.o)

CPPSOURCES		:= $(wildcard $(patsubst %,%/*.cpp, $(SOURCEDIRS)))
CPPOBJECTS		:= $(CPPSOURCES:.cpp=.o)

CCSOURCES		:= $(wildcard $(patsubst %,%/*.cc, $(SOURCEDIRS)))
CCOBJECTS		:= $(CCSOURCES:.cc=.o)

CSOURCES		:= $(wildcard $(patsubst %,%/*.c, $(SOURCEDIRS)))
COBJECTS		:= $(CSOURCES:.c=.o)

ASMDEPS		:= $(ASMOBJECTS:.o=.d)
CXXDEPS		:= $(CXXOBJECTS:.o=.d)
CPPDEPS		:= $(CPPOBJECTS:.o=.d)
CCDEPS		:= $(CCOBJECTS:.o=.d)
CDEPS		:= $(COBJECTS:.o=.d)

OUTPUTMAIN	:= $(call FIXPATH,$(OUTPUT)/$(MAIN))

all: $(OUTPUT) $(MAIN)
	@echo Executing 'all' complete!

$(OUTPUT):
	$(MD) $(OUTPUT)

$(MAIN):  $(CPPOBJECTS) $(CXXOBJECTS) $(CCOBJECTS) $(COBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(OUTPUTMAIN) $(CPPOBJECTS) $(CXXOBJECTS) $(CCOBJECTS) $(COBJECTS) $(LFLAGS) $(LIBS)
-include   $(CPPDEPS) $(CXXDEPS) $(CCDEPS) $(CDEPS)
.s.o:
	$(CXX) $(ASMFLAGS) $(INCLUDES) -c -MMD $<  -o $@
.cxx.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -MMD $<  -o $@
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -MMD $<  -o $@
.cc.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -MMD $<  -o $@
.c.o:
	$(CC) $(CCFLAGS) $(INCLUDES) -c -MMD $<  -o $@
.PHONY: clean
clean:
	del $(call FIXPATH,src/*.o)
	del $(call FIXPATH,src/*.d)
	del $(OUTPUTMAIN)
	@echo Cleanup complete!
run: all
	./$(OUTPUTMAIN)
	@echo Executing 'run: all' complete!


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
