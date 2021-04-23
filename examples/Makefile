CXX = g++
CPPFLAGS:=-std=c++11 -O3 -march=native -mtune=native -flto -funroll-loops
LIB:= -lm -lpthread -lconfig++
MPIFLAGS:=-I/usr/lib/x86_64-linux-gnu/openmpi/include/openmpi -I/usr/lib/x86_64-linux-gnu/openmpi/include/openmpi/opal/mca/event/libevent2022/libevent -I/usr/lib/x86_64-linux-gnu/openmpi/include/openmpi/opal/mca/event/libevent2022/libevent/include -I/usr/lib/x86_64-linux-gnu/openmpi/include 
MPI_LIB:= -L/usr//lib -L/usr/lib/x86_64-linux-gnu/openmpi/lib -lmpi_cxx -lmpi

SRC=../src
INC=-I ../include
src=$(wildcard $(SRC)/*.cpp)
src:=$(filter-out main.cpp, $(src))


custom_ga: $(src) main_custom_ga.cpp 
	$(CXX) $(CPPFLAGS) -DCUSTOM $(INC) $^ -o $@ $(LIB)

independent_runs: $(src) main_independent_run.cpp
	$(CXX) $(CPPFLAGS) $(INC) $^ -o $@ $(LIB)

island_model: $(src) main_island_model.cpp
	$(CXX) $(CPPFLAGS) $(INC) $^ -o $@ $(LIB)

mpi_independent_runs: $(src) main_mpi_independent_runs.cpp
	$(CXX) $(CPPFLAGS) $(MPIFLAGS) $(INC) $^ -o $@ $(LIB) $(MPI_LIB)

mpi_island: $(src) main_mpi_island_model.cpp
	$(CXX) $(CPPFLAGS) $(MPIFLAGS) $(INC) $^ -o $@ $(LIB) $(MPI_LIB)

lib_exe: main_lib.cpp
	$(CXX) $(CPPFLAGS) $(INC) $< -o $@ $(LIB) -L ../lib -lgaim
