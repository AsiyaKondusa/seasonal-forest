CXX      = g++
CXXFLAGS = -std=c++17 -Wall
LIBS     = -lSDL2
SRCS     = $(wildcard *.cpp)
HDRS     = $(wildcard *.h)

forest: $(SRCS) $(HDRS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o forest $(LIBS)

run: forest
	./forest

clean:
	rm -f forest