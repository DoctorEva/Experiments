CXXFLAGS += -std=c++11
GTKFLAGS = -lncurses -lpthread
ex: main.o tetris.o
	$(CXX) $(CXXFLAGS) -o Tetris main.o tetris.o $(GTKFLAGS)
main.o: main.cpp tetris.h
	$(CXX) $(CXXFLAGS) -c main.cpp
tetris.o: tetris.h tetris.cpp
	$(CXX) $(CXXFLAGS) -c tetris.cpp $(GTKFLAGS)

clean:
	rm *.o Tetris
