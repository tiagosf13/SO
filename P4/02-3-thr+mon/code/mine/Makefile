CXX = g++
CXXFLAGS = -Wall -Wfatal-errors -ggdb -pthread

TARGETS = ex2 ex3

.PHONY:	clean cleanall

all: $(TARGETS)

ex2: ex2.o utils.o thread.o
	$(CXX) -o $@ $^ $(CXXFLAGS)

ex3: ex3.o utils.o thread.o
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	rm -f *.o

cleanall:	clean
	rm -f $(TARGETS) 
