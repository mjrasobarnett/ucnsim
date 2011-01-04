CFLAGS=-O2 -g
LIBS=

.PHONY : clean

all : TestKDTree TestExample

TestKDTree : TestKDTree.o KDTree.o KDTreeNode.o Point.o NodeStack.o MWC.o
	g++ -o $@ $^ $(LIBS)

TestKDTree.o : TestKDTree.cpp
	g++ $(CFLAGS) -c -o $@ $<

TestExample : TestExample.o KDTree.o KDTreeNode.o Point.o NodeStack.o MWC.o
	g++ -o $@ $^ $(LIBS)

TestExample.o : TestExample.cpp
	g++ $(CFLAGS) -c -o $@ $<


KDTree.o : KDTree.cpp KDTree.hpp 
	g++ $(CFLAGS) -c -o $@ $<

KDTreeNode.o : KDTreeNode.cpp KDTreeNode.hpp
	g++ $(CFLAGS) -c -o $@ $<

Point.o : Point.cpp Point.hpp
	g++ $(CFLAGS) -c -o $@ $<

NodeStack.o : NodeStack.cpp NodeStack.hpp
	g++ $(CFLAGS) -c -o $@ $<


MWC.o : MWC.cpp MWC.hpp
	g++ $(CFLAGS) -c -o $@ $<

clean :
	rm TestKDTree TestExample *.o
