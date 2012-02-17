// Authors: Matt Raso-Barnett, Emyr James Dec 2010

#ifndef __KD_TREE_HPP_
#define __KD_TREE_HPP_

#include <iostream>
#include <vector>

#include "FieldVertex.hpp"
#include "KDTreeNode.hpp"
#include "VertexStack.hpp"

class KDTree {
   public:
      KDTree();
      KDTree(std::vector<const FieldVertex*>& pointList);
      KDTree(const KDTree& other);
      virtual ~KDTree();
      
      const VertexStack* NearestNeighbours(const FieldVertex& vertex, const int numNeighbours) const;
      int GetVisited() {return fRoot->GetVisited();}
      
      void OutputGraphViz(std::ostream& out) const;
      
   private:
      KDTreeNode *fRoot;
      
      KDTreeNode* BuildNode(std::vector<const FieldVertex*>& pointList, KDTreeNode* parent, int depth=0);
      
};

#endif
