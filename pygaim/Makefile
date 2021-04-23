RM:=rm
CXX = g++
# GCCVERSION=$(shell gcc --version | grep ^gcc | sed 's/^.*//g')
GCCVERSIONG:= $(shell expr `gcc -dumpversion | cut -f1 -d.` \<= 6)
CPPFLAGS:=-std=c++11 -O3 -shared -fPIC -DPYTHON `python3 -m pybind11 --includes`
LIB:= -lm -lpthread -lconfig++
LIB_TEST:= -lm -lpthread -lconfig++

# Folders
SRC_DIR = ../src
INC = -I ../include

# ifeq ($(PYTHON), 1)
# 	CPPFLAGS += -DPYTHON
# endif

ifeq ($(GCCVERSION), 1)
	CPPFLAGS += -DOLDGCC
endif

.PHONY: clean

pygaim: pygaim.cpp $(SRC_DIR)/*.cpp
	$(CXX) $(CPPFLAGS) $(INC) $? -o $@`python3-config --extension-suffix` $(LIB)

clean:
	rm pygaim.*.so
