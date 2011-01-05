#ifndef __NODE_STACK_HPP_
#define __NODE_STACK_HPP_

#include <iostream>
#include <list>

#include "KDTreeNode.h"

typedef std::pair<const KDTreeNode*, double> StackElement;

class NodeStack : public std::list<StackElement> {
   public:
      NodeStack(const int size);
      NodeStack(const NodeStack& other);
      virtual ~NodeStack();
      
      bool operator==(const NodeStack& other) const;
      
      bool AddNode(const KDTreeNode* node, const double distance);
      
   private:
      unsigned int fSize;
};

#endif
