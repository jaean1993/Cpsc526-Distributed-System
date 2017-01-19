# Macro definitions
CXXFLAGS = -O1 -g -Wall -std=c++14
OBJ = main.o Graph.o mongoose.o
TARGET = ./cs426_graph_server

# Rules
all: $(TARGET)
$(TARGET): $(OBJ)
	$(CXX) -o $@ $(OBJ)
clean:
	rm -f $(OBJ) $(TARGET)

# Dependencies

main.o: main.cpp Graph.hpp mongoose.h
Graph.o: Graph.cpp Graph.hpp
mongoose.o: mongoose.c mongoose.h

