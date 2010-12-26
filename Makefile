CFLAGS=-O2 -g
LIBS=

all : TestKDTree TestExample

TestKDTree : TestKDTree.o KDTree.o MWC.o
	g++ -o TestKDTree TestKDTree.o KDTree.o MWC.o $(LIBS)

TestKDTree.o : TestKDTree.cpp
	g++ $(CFLAGS) -c -o TestKDTree.o TestKDTree.cpp

TestExample : TestExample.o KDTree.o MWC.o
	g++ -o TestExample TestExample.o KDTree.o MWC.o $(LIBS)

TestExample.o : TestExample.cpp
	g++ $(CFLAGS) -c -o TestExample.o TestExample.cpp


KDTree.o : KDTree.cpp KDTree.hpp
	g++ $(CFLAGS) -c -o KDTree.o KDTree.cpp

MWC.o : MWC.cpp MWC.hpp
	g++ $(CFLAGS) -c -o MWC.o MWC.cpp

clean :
	touch TestKDTree tmp.o
	rm TestKDTree *.o
