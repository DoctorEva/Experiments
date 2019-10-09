CXXFLAGS += -std=c++11
GTKFLAGS = -lncurses -lpthread
ex: main.cpp tetris.o
	$(CXX) $(CXXFLAGS) -o Tetris main.cpp tetris.o $(GTKFLAGS)
tetris.o: tetris.h tetris.cpp
	$(CXX) $(CXXFLAGS) -c tetris.cpp $(GTKFLAGS)

clean:
	rm *.o Tetris
