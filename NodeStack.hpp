#ifndef __NODE_STACK_HPP_
#define __NODE_STACK_HPP_

#include <iostream>
#include <list>

#include "KDTreeNode.hpp"

class NodeStack {
   public:
      NodeStack(const int size);
      NodeStack(const NodeStack& other);
      virtual ~NodeStack();
      
      bool AddNode(const KDTreeNode& node, const double distance);
      
   private:
      typedef std::pair<const KDTreeNode*, double> StackElement;
      std::list<StackElement> fNodeStack;
      unsigned int fSize;
};

#endif
