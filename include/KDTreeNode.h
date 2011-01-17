// Authors: Matt Raso-Barnett, Emyr James Dec 2010

#ifndef __KDTREE_NODE_HPP_
#define __KDTREE_NODE_HPP_

#include <iostream>
#include "FieldVertex.h"

class VertexStack;

class KDTreeNode {
public:
   KDTreeNode();
   KDTreeNode(const KDTreeNode& other);
   virtual ~KDTreeNode();
   
   void SetDepth(int depth) {fDepth = depth;}
   void SetPoint(const FieldVertex* otherPoint) {fPoint = otherPoint;}
   void SetParent(KDTreeNode* other) {fParent = other;}
   void SetLeftChild(KDTreeNode* otherLeft) {fLeft = otherLeft;}
   void SetRightChild(KDTreeNode* otherRight) {fRight = otherRight;}
   
   int GetDepth() const {return fDepth;}
   const FieldVertex& GetPoint() const {return *fPoint;}
   double DistanceTo(const FieldVertex& point) const {return fPoint->DistanceTo(point);}
   double SquaredDistanceTo(const FieldVertex& point) const {return fPoint->SquaredDistanceTo(point);}
   
   const KDTreeNode& FindNodeContaining(const FieldVertex& point) const;
   bool CheckParentForCloserNodes(const FieldVertex& point, VertexStack& neighbours) const;
   
   void OutputGraphViz(std::ostream& out) const;
   
private:
   unsigned int fDepth;
   const FieldVertex* fPoint;
   KDTreeNode *fParent;
   KDTreeNode *fLeft;
   KDTreeNode *fRight;	
   
   const KDTreeNode* GetParent() const {return fParent;}
   const KDTreeNode* GetLeftChild() const {return fLeft;}
   const KDTreeNode* GetRightChild() const {return fRight;}
   
   bool CheckOtherSideofSplittingPlane(const FieldVertex& point, const double radius) const;
   bool SearchChildren(const FieldVertex& point, VertexStack& neighbours) const;
};

#endif