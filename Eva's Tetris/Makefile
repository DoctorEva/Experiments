CXXFLAGS += -std=c++11
GTKFLAGS = -lncurses -lpthread
ex: games_main.o tetris.o
	$(CXX) $(CXXFLAGS) -o Games games_main.o tetris.o $(GTKFLAGS)
games_main.o: games_main.cpp tetris.h
	$(CXX) $(CXXFLAGS) -c games_main.cpp
tetris.o: tetris.h tetris.cpp
	$(CXX) $(CXXFLAGS) -c tetris.cpp $(GTKFLAGS)

clean:
	rm games_main.o tetris.o Games
