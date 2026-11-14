CXX = g++
CXXFLAGS = -O3 -fopenmp
LDLIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

.PHONY: all run clean

all: pendul

pendul: main.cpp
	$(CXX) main.cpp -o pendul $(CXXFLAGS) $(LDLIBS)

run: pendul
	./pendul

clean:
	@if [ -f pendul ]; then rm pendul; fi
