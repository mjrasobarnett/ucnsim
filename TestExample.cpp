#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <time.h>
#include <string>

#include "KDTree.hpp"
#include "KDTreeNode.hpp"
#include "Point.hpp"
#include "MWC.hpp"

using namespace std;

int main(int argc, char **argv) {
   // Set up timing
   clock_t start, end;
   start = clock();
   
   
   // Output timing
   end = clock();
   cout << "--------------------" << endl;
   cout << "Time required for execution: ";
   cout << (double)(end-start)/CLOCKS_PER_SEC;
   cout << " seconds." << endl;
   cout << "--------------------" << endl;
   
	return 0;
}
