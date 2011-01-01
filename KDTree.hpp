#ifndef __KD_TREE_HPP_
#define __KD_TREE_HPP_

#include <iostream>
#include <vector>

#include "Point.hpp"
#include "KDTreeNode.hpp"

class KDTree {
   public:
      KDTree(std::vector<Point*>& pointList);
      KDTree(const KDTree& other);
      virtual ~KDTree();
      
      const Point& NearestNeighbour(const Point& point) const;
      void OutputGraphViz(std::ostream& out) const;
      
   private:
      KDTreeNode *fRoot;
      
      KDTreeNode* BuildNode(std::vector<Point*>& pointList, KDTreeNode* parent, int depth=0);
      
};

#endif
