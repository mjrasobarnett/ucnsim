#ifndef __KD_TREE_HPP_
#define __KD_TREE_HPP_

#include <iostream>
#include <vector>

#include "FieldVertex.h"
#include "KDTreeNode.h"
#include "VertexStack.h"

class KDTree {
   public:
      KDTree(std::vector<FieldVertex*>& pointList);
      KDTree(const KDTree& other);
      virtual ~KDTree();
      
      const FieldVertex& NearestNeighbour(const FieldVertex& vertex) const;
      const VertexStack* NearestNeighbours(const FieldVertex& vertex, const int numNeighbours) const;
      
      void OutputGraphViz(std::ostream& out) const;
      
   private:
      KDTreeNode *fRoot;
      
      KDTreeNode* BuildNode(std::vector<FieldVertex*>& pointList, KDTreeNode* parent, int depth=0);
      
};

#endif
