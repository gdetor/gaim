RM:=rm
CXX = g++

CPPFLAGS:=-c -std=c++11 -fPIC
LIB:= -lm -lpthread -lconfig++
LIB_TEST:= -lm -lpthread -lconfig++


ifeq (${mode}, debug)
	CPPFLAGS += -Wall -g -Wextra
else
	CPPFLAGS += -O3 -Ofast -march=native -mtune=native -flto -funroll-loops

endif

# Folders
INCDIR:=include
EXEC_DIR = gaim
SRC_DIR = src
BUILD_DIR = build
TEST_DIR = tests
DEMO_DIR = examples
BIN_DIR = bin
LIB_DIR = lib
LTARGET = libgaim.so
INC = -I./include

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

EXECS = $(wildcard $(EXEC_DIR)/*.cpp)
BINARY = $(patsubst $(EXEC_DIR)/%.cpp,$(BIN_DIR)/%,$(EXECS))

TESTS = $(wildcard $(TEST_DIR)/*.cpp)
TESTT = $(patsubst $(TEST_DIR)/%.cpp,$(BIN_DIR)/%,$(TESTS))

.PHONY: clean install distclean tests binary cleanall library

# all: $(TESTT) $(BINARY)
all: $(BINARY)

binary: $(BINARY)

tests: $(TESTT)

library: $(LIB_DIR)/$(LTARGET)


$(BINARY): $(BIN_DIR)/%: $(OBJS)
	@mkdir -p $(BIN_DIR)
	@echo "Linking $(BINARY)"; $(CXX) $(EXEC_DIR)/$*.cpp $^ -o $@ $(LIB) $(INC) 

$(TESTT): $(BIN_DIR)/%: $(OBJS)
	@mkdir -p $(BIN_DIR)
	@echo "Linking $(TESTT)"; $(CXX) $(TEST_DIR)/$*.cpp $^ -o $@ $(LIB_TEST) $(INC) 

$(LIB_DIR)/$(LTARGET): $(OBJS)
	@mkdir -p $(LIB_DIR)
	@echo "Building dynamic library"; $(CXX) -shared -o $@ $^ $(LIB) $(INC)


$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	@echo "Compiling $<"; $(CXX) $(CPPFLAGS) $(INC) -c -o $@ $<

cleanall:
	@echo "Cleaning objects"; rm -rf $(BUILD_DIR)/*
	@echo "Cleaning binaries"; rm -rf $(BIN_DIR)/*

clean:
	@echo "Cleaning objects"; rm -rf $(BUILD_DIR)/*
	@echo "Cleaning binaries"; rm -rf $(BIN_DIR)/*
	@echo "Cleaning library"; rm -rf $(LIB_DIR)/*

install:
	@echo "Installing $(EXECUTABLE)"; cp $(TARGET) $(INSTALLBINDIR)
  
distclean:
	@echo "Removing $(EXECUTABLE)"; rm $(INSTALLBINDIR)/$(EXECUTABLE)


