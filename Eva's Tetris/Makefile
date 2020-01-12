CXXFLAGS += -std=c++11
GTKFLAGS = -lncurses -lpthread
ex: main.cpp tetris.o scores.o
	$(CXX) $(CXXFLAGS) -o Tetris main.cpp tetris.o scores.o $(GTKFLAGS)
tetris.o: tetris.h tetris.cpp
	$(CXX) $(CXXFLAGS) -c tetris.cpp $(GTKFLAGS)
scores.o: scores.cpp scores.h read_file.cpp read_file.h
	$(CXX) $(CXXFLAGS) -c scores.cpp read_file.cpp

clean:
	rm *.o Tetris -f
