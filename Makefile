# Build Process for OpenCL program 

CC = gcc
CPP = g++
CCFLAGS = -g -o3
RM = rm

LIB = -l OpenCL

OBJECTS = main.o
PROJ = main.cpp
EXEC = MatrixMulti



.PHONY: all
.PHONY: clean
.PHONY: compile
.PHONY: acknowledge


#ifdef INTELOCL
# INC_DIRS = $(INTELOCL)/opencl/include
# LIB_DIRS = $(INTELOCL)/opencl/lib64
#else
#  ifdef AMDAPPSDKROOT
INC_DIRS = $(AMDAPPSDKROOT)/include
LIB_DIRS = $(AMDAPPSDKROOT)/lib/x86_64
#  endif
#endif

all: $(OBJECTS) compile acknowledge

$(OBJECTS) : $(PROJ)
	$(CPP) $(CCFLAGS) -o $@ -c $^ -I$(INC_DIRS) 


compile: $(EXEC)

$(EXEC) : $(OBJECTS)
	$(CPP) $(CCFLAGS) -o $@ $^ -L$(LIB_DIRS) $(LIB)

clean:
	$(RM) $(EXEC) $(OBJECTS)

acknowledge:
		@echo " "
		@echo "Compilation Done Successfully"
		@echo " "
