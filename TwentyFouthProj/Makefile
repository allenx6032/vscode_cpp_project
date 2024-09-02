CXX = g++
CC = gcc
CXXFLAGS	:= -std=c++11 -Wimplicit-function-declaration -Wall -Wextra -g -DWIN32 -DTARGET_HOST_MS_WINDOWS -DFREEGLUT_EXPORTS -D_CRT_SECURE_NO_DEPRECATE -DNK_GLFW_GL2_IMPLEMENTATION -DIMGUI_API -DNANOVG_GLEW -DLUA_BUILD_AS_DLL -D_WINDOWS -D_GLFW_USE_CONFIG_H -DUNICODE -D_UNICODE -D_CRT_SECURE_NO_WARNINGS -DGLEW_STATIC -DLUA_COMPAT_ALL -DLUA_COMPAT_5_1 -DLUA_COMPAT_5_2 -DLUA_COMPAT_5_3
CCFLAGS		:= -std=c99 -Wimplicit-function-declaration -Wunused-function -Wall -O2 -g -DWIN32 -DTARGET_HOST_MS_WINDOWS -DFREEGLUT_EXPORTS -D_CRT_SECURE_NO_DEPRECATE -DNK_GLFW_GL2_IMPLEMENTATION -DIMGUI_API -DNANOVG_GLEW -DLUA_BUILD_AS_DLL -D_WINDOWS -D_GLFW_USE_CONFIG_H -DUNICODE -D_UNICODE -D_CRT_SECURE_NO_WARNINGS -DGLEW_STATIC -DLUA_COMPAT_ALL -DLUA_COMPAT_5_1 -DLUA_COMPAT_5_2 -DLUA_COMPAT_5_3
LFLAGS = -lkernel32 -limm32 -lversion -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid  -lgdi32 -lwinmm -lopengl32 -ldinput8 -lMsimg32 -lgdiplus -lshlwapi -ld3d9
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
	./$(OUTPUTMAIN) foreman.cif
	@echo Executing 'run: all' complete!