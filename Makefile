CFLAGS=-O2 -g
LIBS=

all : TestKDTree TestExample

TestKDTree : TestKDTree.o KDTree.o KDTreeNode.o Point.o MWC.o
	g++ -o TestKDTree TestKDTree.o KDTree.o KDTreeNode.o Point.o MWC.o $(LIBS)

TestKDTree.o : TestKDTree.cpp
	g++ $(CFLAGS) -c -o TestKDTree.o TestKDTree.cpp

TestExample : TestExample.o KDTree.o KDTreeNode.o Point.o MWC.o
	g++ -o TestExample TestExample.o KDTree.o KDTreeNode.o Point.o MWC.o $(LIBS)

TestExample.o : TestExample.cpp
	g++ $(CFLAGS) -c -o TestExample.o TestExample.cpp


KDTree.o : KDTree.cpp KDTree.hpp 
	g++ $(CFLAGS) -c -o KDTree.o KDTree.cpp

KDTreeNode.o : KDTreeNode.cpp KDTreeNode.hpp
	g++ $(CFLAGS) -c -o KDTreeNode.o KDTreeNode.cpp

Point.o : Point.cpp Point.hpp
	g++ $(CFLAGS) -c -o Point.o Point.cpp

MWC.o : MWC.cpp MWC.hpp
	g++ $(CFLAGS) -c -o MWC.o MWC.cpp

clean :
	touch TestKDTree TestExample tmp.o
	rm TestKDTree TestExample *.o
