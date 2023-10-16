CXX = g++
CXXFLAGS = -Wall -Wfatal-errors -ggdb -pthread

TARGETS = main ex2 ex3

.PHONY:	clean cleanall

all: $(TARGETS)

main: main.o process.o utils.o
	$(CXX) -o $@ $^ $(CXXFLAGS)

ex2: ex2.o process.o utils.o
	$(CXX) -o $@ $^ $(CXXFLAGS)

ex3: ex3.o process.o utils.o
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	rm -f *.o

cleanall:	clean
	rm -f $(TARGETS) 
