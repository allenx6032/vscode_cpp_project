CXX = g++
CC = gcc
CXXFLAGS	:= -std=c++14 -Wall -Wextra -g -D_CRT_SECURE_NO_DEPRECATE -DNK_GLFW_GL2_IMPLEMENTATION -DIMGUI_API -DNANOVG_GLEW -D_WINDOWS -D_GLFW_USE_CONFIG_H -DUNICODE -D_UNICODE -D_CRT_SECURE_NO_WARNINGS -DGLEW_STATIC -DLUA_COMPAT_ALL -DSDL_VIDEO_DRIVER_GAPI -D_WINDOWS -D_WIN32 -D_WINNT=0x0400 -Wunused-function -DWIN32 -DEBUG -D_WINDOWS -D_USRDLL -DDLL_EXPORT -DFT_DEBUG_LEVEL_ERROR -DFT_DEBUG_LEVEL_TRACE -DFT2_BUILD_LIBRARY -DSMALL_MEDIUM -DSLOW -DBUILDFIXED  -DFT_DEBUG_LEVEL_TRACE -DINITGUID -D__USE_W32_SOCKETS                       
CCFLAGS		:= -std=c11 -Wunused-function -Wall -O2 -g -D_CRT_SECURE_NO_DEPRECATE -DNK_GLFW_GL2_IMPLEMENTATION -DIMGUI_API -DNANOVG_GLEW -D_WINDOWS -D_GLFW_USE_CONFIG_H -DUNICODE -D_UNICODE -D_CRT_SECURE_NO_WARNINGS -DGLEW_STATIC -DLUA_COMPAT_ALL -DSDL_VIDEO_DRIVER_GAPI -D_WINDOWS -D_WIN32 -D_WINNT=0x0400 -Wunused-function -DWIN32 -DEBUG -D_WINDOWS -D_USRDLL -DDLL_EXPORT -DFT_DEBUG_LEVEL_ERROR -DFT_DEBUG_LEVEL_TRACE -DFT2_BUILD_LIBRARY -DSMALL_MEDIUM -DSLOW -DBUILDFIXED -DFT_DEBUG_LEVEL_TRACE -DINITGUID -D__USE_W32_SOCKETS                       
LFLAGS = -lkernel32 -limm32 -lversion -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid  -lgdi32 -lwinmm -lopengl32 -ldinput8 -lMsimg32 -lgdiplus -lshlwapi -ld3d9 -lWs2_32 -lIphlpapi 
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



# CC = gcc
# CXX = g++
# CFLAGS		:= -Wall -DWIN32 -D_WINDOWS -D_GLFW_USE_CONFIG_H -DUNICODE -D_UNICODE -D_CRT_SECURE_NO_WARNINGS -g
# CXXFLAGS	:= -std=c++17 -Wall -g
# LFLAGS = -lkernel32 -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32 -lgdi32 -lwinmm -lopengl32

# OUTPUT	:= output
# SRC		:= src
# INCLUDE	:= include
# LIB		:= lib

# ifeq ($(OS),Windows_NT)
# MAIN_C	:= main_c.exe
# MAIN_CPP	:= main_cpp.exe
# SOURCEDIRS	:= $(SRC)
# INCLUDEDIRS	:= $(INCLUDE)
# LIBDIRS		:= $(LIB)
# FIXPATH = $(subst /,\,$1)
# RM			:= del /q /f
# MD	:= mkdir
# else
# MAIN_C	:= main_c
# MAIN_CPP	:= main_cpp
# SOURCEDIRS	:= $(shell find $(SRC) -type d)
# INCLUDEDIRS	:= $(shell find $(INCLUDE) -type d)
# LIBDIRS		:= $(shell find $(LIB) -type d)
# FIXPATH = $1
# RM = rm -f
# MD	:= mkdir -p
# endif

# INCLUDES	:= $(patsubst %,-I%, $(INCLUDEDIRS:%/=%))
# LIBS		:= $(patsubst %,-L%, $(LIBDIRS:%/=%))
# SOURCES_C		:= $(wildcard $(patsubst %,%/*.c, $(SOURCEDIRS)))
# SOURCES_CPP		:= $(wildcard $(patsubst %,%/*.cpp, $(SOURCEDIRS)))
# OBJECTS_C		:= $(SOURCES_C:.c=.o)
# OBJECTS_CPP		:= $(SOURCES_CPP:.cpp=.o)
# OUTPUTMAIN_C	:= $(call FIXPATH,$(OUTPUT)/$(MAIN_C))
# OUTPUTMAIN_CPP	:= $(call FIXPATH,$(OUTPUT)/$(MAIN_CPP))
# all: $(OUTPUT) $(MAIN_C) $(MAIN_CPP)
# 	@echo Executing 'all' complete!
# $(OUTPUT):
# 	$(MD) $(OUTPUT)
# $(MAIN_C): $(OBJECTS_C) 
# 	$(CC) $(CFLAGS) $(INCLUDES) -o $(OUTPUTMAIN_C) $(OBJECTS_C) $(LFLAGS) $(LIBS)
# $(MAIN_CPP): $(OBJECTS_CPP) 
# 	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(OUTPUTMAIN_CPP) $(OBJECTS_CPP) $(LFLAGS) $(LIBS)
# .c.o:
# 	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@
# .cpp.o:
# 	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $<  -o $@
# .PHONY: clean
# clean:
# 	$(RM) $(OUTPUTMAIN_C)
# 	$(RM) $(OUTPUTMAIN_CPP)
# 	$(RM) $(call FIXPATH,$(OBJECTS_C))
# 	$(RM) $(call FIXPATH,$(OBJECTS_CPP))
# 	@echo Cleanup complete!
# run: all
# 	./$(OUTPUTMAIN_C)
# 	./$(OUTPUTMAIN_CPP)
# 	@echo Executing 'run: all' complete!

# # # c.cpp混合编译的makefile模板
# # BIN = main.exe
# # CC = gcc
# # CPP = g++
# # #这里只加入库头文件路径及库路径
# # # INCS = -I"/include"
# # # LIBS = -L"/lib"
# # INCS = include
# # LIBS = lib
# # SUBDIRS =
# # #生成依赖信息时的搜索目录，比如到下列目录中搜索一个依赖文件(比如.h文件)，例如 -I"./***/" -I"./base/"
# # DEFINC =
# # #给INCS加上依赖搜索路径，分开写可能会产生不一致情况，而且繁琐
# # INCS += $(DEFINC)
# # #maintest.c tree/rbtree.c  多了子目录，那就直接添加 目录/*.c即可   所有的源文件--  .c文件列表
# # CSRCS = $(wildcard ./src/*.c)
# # CPPSRCS = $(wildcard ./src/*.cpp)
# # #SRCS += $(wildcard ./../base/*.c)
# # #SRCS += $(wildcard ./*.c ./src/*.c ./src/base/*.c ./src/base/tree/*.c ./src/mem/*.c)

# # #
# # #所有的.o文件列表
# # COBJS := $(CSRCS:.c=.o)
# # CPPOBJS := $(CPPSRCS:.cpp=.o)
# # #
# # #生成依赖信息 -MM是只生成自己的头文件信息，-M 包含了标准库头文件信息。
# # #-MT 或 -MQ都可以改变生成的依赖  xxx.o:src/xxx.h 为 src/xxx.o:src/xxx.h 当然。前面的 src/xxx.o需自己指定
# # #格式为 -MM 输入.c或.cpp  查找依赖路径  -MT或-MQ  生成规则，比如src/xxx.o
# # MAKEDEPEND = gcc -MM -MT
# # CFLAGS += $(INCS)
# # CFLAGS += -std=c99 -O2 -Wall -fomit-frame-pointer -g -ansi
# # CPPFLAGS += $(INCS)
# # CPPFLAGS += -std=c++17 -O2 -Wall -g
# # LDFLAGS = -lkernel32 -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32 -lgdi32 -lwinmm -lopengl32

# # #-g 生成调试信息
# # #-pedantic参数与-ansi一起使用 会自动拒绝编译非ANSI程序
# # #-fomit-frame-pointer 去除函数框架
# # #-Wmissing-prototypes -Wstrict-prototypes 检查函数原型
# # #针对每个.c文件的.d依赖文件列表
# # COBJS = $(CSRCS:.c=.o)
# # CPPOBJS = $(CPPSRCS:.cpp=.o)
# # all:$(BIN)
# # #生成.o的对自己目录中.h .c的依赖信息.d文件到.c所在的路径中
# # #$(DEF)文件是.d文件名列表(含目录)，比如tree.d 匹配成功那么%就是tree，然后在尝试%.c，如果成功。则执行规则
# # # $(<:.c=.o)是获取此.c文件的名字(含路径)，然后变为.o比如 src/xxx.o。 以形成如下
# # # src/xxx.o : src/xxx.c ***.h  ***.h  最前面！！注意。  
# # # 此做法是每个.d都和生成他的.c在一个目录里，所以需要这样做。
# # # $(<:.c=.o)之类的 。此时的<相当于变量$< 。切记
# # # : : :  含义同下
# # $(COBJS) : %.o: %.c
# # 	$(CC) $(CFLAGS) -c $< -o $@ 
# # $(CPPOBJS) : %.o: %.cpp
# # 	$(CPP) $(CPPFLAGS) -c $< -o $@ 
# # #先删除依赖信息
# # #重新生成依赖信息
# # #这里出现了一个 $(MAKE) 没有定义的变量。这个变量是由 Make 自己定义的，它的值即为自己的位置，方便 Make 递归调用自己。
# # # depend:
# # #     rm $(CDEF)
# # #     rm $(CPPDEF)
# # #     $(MAKE) $(CDEF)
# # #     $(MAKE) $(CPPDEF)

# # #$(OBJS):%.o :%.c  先用$(OBJS)中的一项，比如foo.o: %.o : %.c  含义为:试着用%.o匹配foo.o。如果成功%就等于foo。如果不成功，
# # # Make就会警告，然后。给foo.o添加依赖文件foo.c(用foo替换了%.c里的%)
# # # 也可以不要下面的这个生成规则，因为下面的 include $(DEF)  就隐含了。此处为了明了，易懂。故留着
# # # $(COBJS) : %.o: %.c
# # #     $(CC) -c $< -o $@ $(CFLAGS)
# # # $(CPPOBJS) : %.o: %.cpp
# # #     $(CPP) -c $< -o $@ $(CPPFLAGS)

# # # $@--目标文件，$^--所有的依赖文件，$<--第一个依赖文件。每次$< $@ 代表的值就是列表中的
# # #
# # $(BIN) : $(COBJS) $(CPPOBJS)
# #     $(CC) -o $(BIN) $(COBJS) $(CPPOBJS) $(LDFLAGS) $(LIBS) 
# #     rm $(COBJS)
# #     rm $(CPPOBJS)
# #     # rm $(CDEF)
# #     # rm $(CPPDEF)
# # # 链接为最终目标

# # #引入了.o文件对.c和.h的依赖情况。以后.h被修改也会重新生成，可看看.d文件内容即知道为何
# # #引入了依赖就相当于引入了一系列的规则，因为依赖内容例如： 目录/xxx.o:目录/xxx.c 目录/xxx.h 也相当于隐含的引入了生成规则
# # #故上面不能在出现如： $(OBJS) : $(DEF)之类。切记
# # # include $(CDEF)
# # # include $(CPPDEF)
# # .PHONY:clean
# # #清除所有目标文件以及生成的最终目标文件
# # clean:            
# #     rm $(BIN) $(COBJS) $(CPPOBJS)


# # CXX = g++
# # CC = gcc
# # LINK = g++
# # CXXFLAGS	:= -std=c++17 -Wall -O2 -g
# # CFLAGS	:= -Wall -O2 -g
# # LFLAGS = -lkernel32 -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32 -lgdi32 -lwinmm -lopengl32

# # MAIN	:= main.exe

# # TOP_PATH = $(shell pwd)
# # INC_PATH = $(TOP_PATH)/include
# # SRC_PATH = $(TOP_PATH)
# # SRC_PATH += $(TOP_PATH)/src
# # ##########################################################  
# # # srcs  
# # ##########################################################  
# # SRCS_CPP += $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))  
# # SRCS_CC += $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cc))  
# # SRCS_C += $(foreach dir, $(DIRS), $(wildcard $(dir)/*.c))  
# # ##########################################################  
# # # objs  
# # ##########################################################  
# # OBJS_CPP = $(patsubst %.cpp, %.o, $(SRCS_CPP))  
# # OBJS_CC = $(patsubst %.cc, %.o, $(SRCS_CC))  
# # OBJS_C = $(patsubst %.c, %.o, $(SRCS_C))  

# # ##########################################################  
# # # building
# # ##########################################################  
# # all:$(MAIN)
# # $(MAIN) : $(OBJS_CPP) $(OBJS_CC) $(OBJS_C)  
# #     @ $(CXX) $^ -o $@ $(LIB_PATH) $(LIBS)  
# #     @ echo Create $(MAIN) ok...  
# # $(OBJS_CPP):%.o : %.cpp
# #     $(CXX) $(CXXFLAGS) $< -o $@ $(INC_PATH)
# # $(OBJS_CC):%.o : %.cc
# #     $(CC) $(CFLAGS) $< -o $@ $(INC_PATH)
# # $(OBJS_C):%.o : %.c
# #     $(CC) $(CFLAGS) $< -o $@ $(INC_PATH)
# # $(OBJS_C):%.o : %.c
# #     $(CC) $(CFLAGS) $< -o $@ $(INC_PATH)
# # .PHONY : clean  
# # clean:  
# #     @ $(RM) $(TARGET) $(OBJS_C)  



# # CXX = g++
# # CC = gcc
# # LINK = g++
# # CXXFLAGS	:= -std=c++17 -Wall -O2 -g
# # CFLAGS	:= -Wall -O2 -g
# # LFLAGS = -lkernel32 -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32 -lgdi32 -lwinmm -lopengl32
# # OUTPUT	:= output
# # SRC		:= SRC
# # INCLUDE	:= include
# # LIB		:= lib 
# # ifeq ($(OS),Windows_NT)
# # MAIN	:= main.exe
# # SOURCEDIRS	:= $(SRC)
# # INCLUDEDIRS	:= $(INCLUDE)
# # LIBDIRS		:= $(LIB)
# # FIXPATH = $(subst /,\,$1)
# # RM			:= del /q /f
# # MD	:= mkdir
# # else
# # MAIN	:= main
# # SOURCEDIRS	:= $(shell find $(SRC) -type d)
# # INCLUDEDIRS	:= $(shell find $(INCLUDE) -type d)
# # LIBDIRS		:= $(shell find $(LIB) -type d)
# # FIXPATH = $1
# # RM = rm -f
# # MD	:= mkdir -p
# # endif
# # INCLUDES	:= $(patsubst %,-I%, $(INCLUDEDIRS:%/=%))
# # LIBS		:= $(patsubst %,-L%, $(LIBDIRS:%/=%))
# # CSOURCES		:= $(wildcard $(patsubst %,%/*.c, $(SOURCEDIRS)))
# # CXXSOURCES		:= $(wildcard $(patsubst %,%/*.cpp, $(SOURCEDIRS)))
# # COBJECTS		:= $(CSOURCES:.c=.o)
# # CDEPS		:= $(COBJECTS:.o=.d)
# # CXXOBJECTS		:= $(CXXSOURCES:.cpp=.o)
# # CXXDEPS		:= $(CXXOBJECTS:.o=.d)
# # OBJECTS := $(CSOURCES:.c=.o) $(CXXSOURCES:.cpp=.o))
# # OUTPUTMAIN	:= $(call FIXPATH,$(OUTPUT)/$(MAIN))
# # all: $(OUTPUT) $(MAIN)
# # 	@echo Executing 'all' complete!
# # $(OUTPUT):
# # 	$(MD) $(OUTPUT)
# # $(MAIN): $(COBJECTS) $(CXXOBJECTS) 
# # 	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(OUTPUTMAIN) $(CXXOBJECTS) $(LFLAGS) $(LIBS)
# # # $(CC) $(CFLAGS) $(INCLUDES) -o $(OUTPUTMAIN) $(COBJECTS) $(LFLAGS) $(LIBS)
# # %.o:%.c:
# # 	$(CC) -o $@ -$(CFLAGS) $(INCLUDES) -c -MMD $<  -o $@
# # %.o:%.cpp:
# #     $(CXX) $(CXXFLAGS) $(INCLUDES) -c -MMD $<  -o $@
# # .PHONY: clean
# # clean:
# # 	$(RM) $(OUTPUTMAIN)
# # 	$(RM) $(call FIXPATH,$(OBJECTS))
# # 	@echo Cleanup complete!
# # run: all
# # 	./$(OUTPUTMAIN)
# # 	@echo Executing 'run: all' complete!


# # TARGET = exe
# # CC = gcc
# # CXX = g++
# # AllDIRS := $(shell ls -R | grep '^\./.*:$$' | awk '{gsub(":","");print}') .
# # CSRCS = $(foreach n,$(AllDIRS) , $(wildcard $(n)/*.c)) 
# # CXXSRCS = $(foreach n,$(AllDIRS) , $(wildcard $(n)/*.cpp)) 
# # OBJS := $(CSRCS:.c=.o) $(CXXSRCS:.cpp=.o)
# # SRC_PATH = .
# # INC_PATH = -I $(SRC_PATH) 
# # LFLAGS = -lkernel32 -luser32 -lwinspool -lodbc32 -lodbccp32 -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32 -lgdi32 -lwinmm -lopengl32
# # DEBUG_FLAG  = -g
# # CXXFLAGS	:= -std=c++17 -Wall -g 
# # CFLAGS	:= -std=c89 -Wall -g 
# # %.o : %.cpp
# # 	$(CXX) $(CXXFLAGS) $< -c -o $@ $(INC_PATH) $(LIB_PATH);
# # %.o : %.c
# # 	$(CC) $(CFLAGS) $< -c -o  $@ $(INC_PATH) $(LIB_PATH);
# # $(TARGET) : $(OBJS)
# # 	$(CXX) $(DEBUG_FLAG) $^ -o $@ $(INC_PATH) $(LFLAGS) $(LIBS);
# # 	@echo "> build success <"
# # .PHONY:clean
# # clean:
# # 	rm -f $(OBJS) $(TARGET)


# # MY_CFLAGS =  
# # MY_LIBS   =    
# # CPPFLAGS  = -Wall    
# # LDFLAGS   =  
# # SRCDIRS   =  
# # PROGRAM   =    
# # SRCEXTS = .c .C .cc .cpp .CPP .c++ .cxx .cp  
# # HDREXTS = .h .H .hh .hpp .HPP .h++ .hxx .hp  
# # CFLAGS  = -g -O2  
# # CXXFLAGS= -g -O2  
# # ETAGS = etags  
# # ETAGSFLAGS =  
# # CTAGS = ctags  
# # CTAGSFLAGS =  
# # SHELL   = /bin/sh  
# # EMPTY   =  
# # SPACE   = $(EMPTY) $(EMPTY)  
# # ifeq ($(PROGRAM),)  
# #   CUR_PATH_NAMES = $(subst /,$(SPACE),$(subst $(SPACE),_,$(CURDIR)))  
# #   PROGRAM = $(word $(words $(CUR_PATH_NAMES)),$(CUR_PATH_NAMES))  
# #   ifeq ($(PROGRAM),)  
# #     PROGRAM = a.out  
# #   endif  
# # endif  
# # ifeq ($(SRCDIRS),)  
# #   SRCDIRS = .  
# # endif  
# # SOURCES = $(foreach d,$(SRCDIRS),$(wildcard $(addprefix $(d)/*,$(SRCEXTS))))  
# # HEADERS = $(foreach d,$(SRCDIRS),$(wildcard $(addprefix $(d)/*,$(HDREXTS))))  
# # SRC_CXX = $(filter-out %.c,$(SOURCES))  
# # OBJS    = $(addsuffix .o, $(basename $(SOURCES)))  
# # DEPS    = $(OBJS:.o=.d)  
# # DEP_OPT = $(shell if `$(CC) --version | grep "GCC" >/dev/null`; then \  
# #                   echo "-MM -MP"; else echo "-M"; fi )  
# # DEPEND      = $(CC)  $(DEP_OPT)  $(MY_CFLAGS) $(CFLAGS) $(CPPFLAGS)  
# # DEPEND.d    = $(subst -g ,,$(DEPEND))  
# # COMPILE.c   = $(CC)  $(MY_CFLAGS) $(CFLAGS)   $(CPPFLAGS) -c  
# # COMPILE.cxx = $(CXX) $(MY_CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -c  
# # LINK.c      = $(CC)  $(MY_CFLAGS) $(CFLAGS)   $(CPPFLAGS) $(LDFLAGS)  
# # LINK.cxx    = $(CXX) $(MY_CFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS)  
# # .PHONY: all objs tags ctags clean distclean help show  
# # .SUFFIXES:  
# # all: $(PROGRAM)  
# # %.d:%.c  
# #     @echo -n $(dir $<) > $@  
# #     @$(DEPEND.d) $< >> $@  
# # %.d:%.C  
# #     @echo -n $(dir $<) > $@  
# #     @$(DEPEND.d) $< >> $@  
# # %.d:%.cc  
# #     @echo -n $(dir $<) > $@  
# #     @$(DEPEND.d) $< >> $@  
# # %.d:%.cpp  
# #     @echo -n $(dir $<) > $@  
# #     @$(DEPEND.d) $< >> $@  
# # %.d:%.CPP  
# #     @echo -n $(dir $<) > $@  
# #     @$(DEPEND.d) $< >> $@  
# # %.d:%.c++  
# #     @echo -n $(dir $<) > $@  
# #     @$(DEPEND.d) $< >> $@  
# # %.d:%.cp  
# #     @echo -n $(dir $<) > $@  
# #     @$(DEPEND.d) $< >> $@  
# # %.d:%.cxx  
# #     @echo -n $(dir $<) > $@  
# #     @$(DEPEND.d) $< >> $@  
# # objs:$(OBJS)  
# # %.o:%.c  
# #     $(COMPILE.c) $< -o $@  
# # %.o:%.C  
# #     $(COMPILE.cxx) $< -o $@  
# # %.o:%.cc  
# #     $(COMPILE.cxx) $< -o $@  
# # %.o:%.cpp  
# #     $(COMPILE.cxx) $< -o $@  
# # %.o:%.CPP  
# #     $(COMPILE.cxx) $< -o $@  
# # %.o:%.c++  
# #     $(COMPILE.cxx) $< -o $@  
# # %.o:%.cp  
# #     $(COMPILE.cxx) $< -o $@  
# # %.o:%.cxx  
# #     $(COMPILE.cxx) $< -o $@  
# # tags: $(HEADERS) $(SOURCES)  
# #     $(ETAGS) $(ETAGSFLAGS) $(HEADERS) $(SOURCES)  
# # ctags: $(HEADERS) $(SOURCES)  
# #     $(CTAGS) $(CTAGSFLAGS) $(HEADERS) $(SOURCES)  
# # $(PROGRAM):$(OBJS)  
# # ifeq ($(SRC_CXX),)              # C program  
# #     $(LINK.c)   $(OBJS) $(MY_LIBS) -o $@  
# #     @echo Type ./$@ to execute the program.  
# # else                            # C++ program  
# #     $(LINK.cxx) $(OBJS) $(MY_LIBS) -o $@  
# #     @echo Type ./$@ to execute the program.  
# # endif  
  
# # ifndef NODEP  
# # ifneq ($(DEPS),)  
# #   sinclude $(DEPS)  
# # endif  
# # endif  
# # clean:  
# #     $(RM) $(OBJS) $(PROGRAM) $(PROGRAM).exe  
# # distclean: clean  
# #     $(RM) $(DEPS) TAGS  
# # help:  
# #     @echo 'Generic Makefile for C/C++ Programs (gcmakefile) version 0.5'  
# #     @echo 'Copyright (C) 2007, 2008 whyglinux <whyglinux@hotmail.com>'  
# #     @echo  
# #     @echo 'Usage: make [TARGET]'  
# #     @echo 'TARGETS:'  
# #     @echo '  all       (=make) compile and link.'  
# #     @echo '  NODEP=yes make without generating dependencies.'  
# #     @echo '  objs      compile only (no linking).'  
# #     @echo '  tags      create tags for Emacs editor.'  
# #     @echo '  ctags     create ctags for VI editor.'  
# #     @echo '  clean     clean objects and the executable file.'  
# #     @echo '  distclean clean objects, the executable and dependencies.'  
# #     @echo '  show      show variables (for debug use only).'  
# #     @echo '  help      print this message.'  
# #     @echo  
# #     @echo 'Report bugs to <whyglinux AT gmail DOT com>.'  
# # show:  
# #     @echo 'PROGRAM     :' $(PROGRAM)  
# #     @echo 'SRCDIRS     :' $(SRCDIRS)  
# #     @echo 'HEADERS     :' $(HEADERS)  
# #     @echo 'SOURCES     :' $(SOURCES)  
# #     @echo 'SRC_CXX     :' $(SRC_CXX)  
# #     @echo 'OBJS        :' $(OBJS)  
# #     @echo 'DEPS        :' $(DEPS)  
# #     @echo 'DEPEND      :' $(DEPEND)  
# #     @echo 'COMPILE.c   :' $(COMPILE.c)  
# #     @echo 'COMPILE.cxx :' $(COMPILE.cxx)  
# #     @echo 'link.c      :' $(LINK.c)  
# #     @echo 'link.cxx    :' $(LINK.cxx)  