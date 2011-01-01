#ifndef __KDTREE_NODE_HPP_
#define __KDTREE_NODE_HPP_

#include <iostream>
#include "Point.hpp"

//______________________________________________________________________________
class KDTreeNode {
public:
   KDTreeNode();
   KDTreeNode(const KDTreeNode& other);
   virtual ~KDTreeNode();
   
   void SetDepth(int depth) {fDepth = depth;}
   void SetPoint(Point* otherPoint) {fPoint = otherPoint;}
   void SetParent(KDTreeNode* other) {fParent = other;}
   void SetLeftChild(KDTreeNode* otherLeft) {fLeft = otherLeft;}
   void SetRightChild(KDTreeNode* otherRight) {fRight = otherRight;}
   
   int GetDepth() const {return fDepth;}
   const Point& GetPoint() const {return *fPoint;}
   
   const KDTreeNode& FindNodeContaining(const Point& point) const;
   const KDTreeNode& CheckParentForCloserNodes(const Point& point, const KDTreeNode& best) const;
   
   void OutputGraphViz(std::ostream& out) const;
   
private:
   unsigned int fDepth;
   Point* fPoint;
   KDTreeNode *fParent;
   KDTreeNode *fLeft;
   KDTreeNode *fRight;	
   
   const KDTreeNode* GetParent() const {return fParent;}
   const KDTreeNode* GetLeftChild() const {return fLeft;}
   const KDTreeNode* GetRightChild() const {return fRight;}
   
   bool CheckOtherSideofSplittingPlane(const Point& point, const double radius) const;
   const KDTreeNode& SearchChildren(const Point& point, const KDTreeNode& currentBest) const;
   bool IsCloserToPoint(const Point& point, const KDTreeNode& currentBest) const;
};

#endif