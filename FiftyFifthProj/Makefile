# CXX = g++
# CC = gcc
# LINK = g++
# CXXFLAGS	:= -std=c++17 -Wall -O2 -g
# CFLAGS	:= -std=c11 -Wall -O2 -g
# LFLAGS = -lkernel32 -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32 -lgdi32 -lwinmm -lopengl32

# MAIN	:= main.exe

# TOP_PATH = $(shell pwd)
# INC_PATH = nclude
# SRC_PATH = src
# LIB_PATH = lib
# OUTPUT_PATH = output


# ifeq ($(OS),Windows_NT)
# MAIN	:= main.exe
# SOURCEDIRS	:= $(SRC_PATH)
# INCLUDEDIRS	:= $(INC_PATH)
# LIBDIRS		:= $(LIB_PATH)
# FIXPATH = $(subst /,\,$1)
# # RM			:= del /s /q /f
# RM			:= rm  -f
# MD	:= mkdir
# else
# MAIN	:= main
# SOURCEDIRS	:= $(shell find $(SRC) -type d)
# INCLUDEDIRS	:= $(shell find $(INCLUDE) -type d)
# LIBDIRS		:= $(shell find $(LIB) -type d)
# FIXPATH = $1
# RM = rm -f
# MD	:= mkdir -p
# endif

# OUTPUTMAIN	:= $(call FIXPATH,$(OUTPUT_PATH)/$(MAIN))
# INCLUDES	:= $(patsubst %,-I%, $(INCLUDEDIRS:%/=%))
# LIBS		:= $(patsubst %,-L%, $(LIBDIRS:%/=%))


# ##########################################################  
# # srcs  
# ##########################################################  
# SRCS_CPP += $(foreach dir, $(SOURCEDIRS), $(wildcard $(dir)/*.cpp))  
# SRCS_CXX += $(foreach dir, $(SOURCEDIRS), $(wildcard $(dir)/*.cxx))  
# SRCS_CC += $(foreach dir, $(SOURCEDIRS), $(wildcard $(dir)/*.cc))  
# SRCS_C += $(foreach dir, $(SOURCEDIRS), $(wildcard $(dir)/*.c))  
# ##########################################################  
# # objs  
# ##########################################################  
# OBJS_CPP = $(patsubst %.cpp, %.o, $(SRCS_CPP))  
# OBJS_CXX = $(patsubst %.cxx, %.o, $(SRCS_CXX))  
# OBJS_CC = $(patsubst %.cc, %.o, $(SRCS_CC))  
# OBJS_C = $(patsubst %.c, %.o, $(SRCS_C))  

# ##########################################################  
# # building
# ##########################################################  
# all:$(OUTPUT) $(MAIN)
# $(MAIN) : $(OBJS_CPP) $(OBJS_CXX) $(OBJS_CC) $(OBJS_C)  
# 	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(OUTPUT_PATH) $(OBJS_CPP) $(OBJS_CXX) $(OBJS_CC) $(OBJS_C) $(LFLAGS) $(LIBS)
# 	@echo Executing 'all' complete!
# $(OUTPUT):
# 	$(MD) $(OUTPUT)
# $(MAIN): $(OBJS_CPP) $(OBJS_CXX)  $(OBJS_CC) $(OBJS_C)
# 	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(OUTPUTMAIN) $(OBJS_CPP) $(OBJS_CXX) $(OBJS_CC) $(OBJS_C) $(LFLAGS) $(LIBS)
# $(OBJS_CPP):%.o : %.cpp
#     $(CXX) $(CXXFLAGS) $< -o $@ $(INCLUDES)
# $(OBJS_CXX):%.o : %.cxx
#     $(CXX) $(CFLAGS) $< -o $@ $(INCLUDES)
# $(OBJS_CC):%.o : %.cc
#     $(CC) $(CFLAGS) $< -o $@ $(INCLUDES)
# $(OBJS_C):%.o : %.c
#     $(CC) $(CFLAGS) $< -o $@ $(INCLUDES)
# .PHONY : clean  
# clean:  
#     @ $(RM) $(TARGET) $(OBJS_C)  



CXX = g++
CC = gcc
CXXFLAGS	:= -std=c++20 -Wunused-function -Wall -Wextra -O2 -g -D_CRT_SECURE_NO_DEPRECATE -D_WINDOWS -D_GLFW_USE_CONFIG_H -DUNICODE -D_UNICODE -D_CRT_SECURE_NO_WARNINGS -DGLEW_STATIC -DLUA_COMPAT_ALL -D_WINDOWS -D_WIN32 -D_WINNT=0x0400 -DWIN32 -DEBUG -D_WINDOWS -D__USE_W32_SOCKETS -D__WINPTHREAD_ENABLE_WRAP_API                  
CCFLAGS		:= -std=c11 -Wunused-function -Wall -Wextra -O2 -g -D_CRT_SECURE_NO_DEPRECATE -D_WINDOWS -D_GLFW_USE_CONFIG_H -DUNICODE -D_UNICODE -D_CRT_SECURE_NO_WARNINGS -DGLEW_STATIC -DLUA_COMPAT_ALL -D_WINDOWS -D_WIN32 -D_WINNT=0x0400 -DWIN32 -DEBUG -D_WINDOWS -D__USE_W32_SOCKETS -D__WINPTHREAD_ENABLE_WRAP_API                    
# LFLAGS = -lkernel32 -limm32 -lversion -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid  -lwinmm -lmingw32 -lMsimg32 -lopengl32 -lglu32 -lgdi32 -lgdiplus -ld3d9 -ddxguid -dd3d11 -lshlwapi -lWs2_32 -lIphlpapi 
LFLAGS = -fexec-charset=utf-8 -finput-charset=utf-8 -lkernel32 -limm32 -lversion -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid  -lwinmm -lmingw32 -lMsimg32 -lopengl32 -lglu32 -lgdi32 -lgdiplus -ld3d9 -ddxguid -dd3d11 -lshlwapi -lWs2_32 -lIphlpapi 
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
CXXSOURCES		:= $(wildcard $(patsubst %,%/*.cxx, $(SOURCEDIRS)))
CXXOBJECTS		:= $(CXXSOURCES:.cxx=.o)
CPPSOURCES		:= $(wildcard $(patsubst %,%/*.cpp, $(SOURCEDIRS)))
CPPOBJECTS		:= $(CPPSOURCES:.cpp=.o)
CCSOURCES		:= $(wildcard $(patsubst %,%/*.cc, $(SOURCEDIRS)))
CCOBJECTS		:= $(CCSOURCES:.cc=.o)
CSOURCES		:= $(wildcard $(patsubst %,%/*.c, $(SOURCEDIRS)))
COBJECTS		:= $(CSOURCES:.c=.o)
CXXDEPS		:= $(CXXOBJECTS:.o=.d)
CPPDEPS		:= $(CPPOBJECTS:.o=.d)
CCDEPS		:= $(CCOBJECTS:.o=.d)
CDEPS		:= $(COBJECTS:.o=.d)

OUTPUTMAIN	:= $(call FIXPATH,$(OUTPUT)/$(MAIN))

all: $(OUTPUT) $(MAIN)
	@echo Executing 'all' complete!

$(OUTPUT):
	$(MD) $(OUTPUT)

$(MAIN): $(CXXOBJECTS) $(CPPOBJECTS)  $(CCOBJECTS) $(COBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(OUTPUTMAIN) $(CXXOBJECTS) $(CPPOBJECTS) $(CCOBJECTS) $(COBJECTS) $(LFLAGS) $(LIBS)
-include $(CXXDEPS)  $(CPPDEPS) $(CCDEPS)  $(CDEPS)
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
	$(RM) $(call FIXPATH,$(CXXOBJECTS))
	$(RM) $(call FIXPATH,$(CPPOBJECTS))
	$(RM) $(call FIXPATH,$(CCOBJECTS))
	$(RM) $(call FIXPATH,$(COBJECTS))
	$(RM) $(call FIXPATH,$(CXXDEPS))
	$(RM) $(call FIXPATH,$(CPPDEPS))
	$(RM) $(call FIXPATH,$(CCDEPS))
	$(RM) $(call FIXPATH,$(CDEPS))
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