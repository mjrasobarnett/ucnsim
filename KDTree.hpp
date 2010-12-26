#ifndef __KD_TREE_HPP_
#define __KD_TREE_HPP_

#include <string>
#include <iostream>
#include <vector>

//______________________________________________________________________________
class Point {
   public :

      // constructors
      Point();
      Point(double x, double y, double z);
      Point(const Point& other);
      virtual ~Point();

      // getters
      double X() const {return fX;}
      double Y() const {return fY;}
      double Z() const {return fZ;}

      // setters
      void SetX(double x){fX=x;}
      void SetY(double y){fY=y;}
      void SetZ(double z){fZ=z;}

      std::string ToString() const;
      double DistanceTo(const Point& point) const;
      double SquaredDistanceTo(const Point& point) const;
      
   private:
      double fX,fY,fZ;
};

class sortX {
	public:
	bool operator() (Point *a, Point *b) { return ((*a).X() < (*b).X());}
};

class sortY {
	public:
	bool operator() (Point *a, Point *b) { return ((*a).Y() < (*b).Y());}
};

class sortZ {
	public:
	bool operator() (Point *a, Point *b) { return ((*a).Z() < (*b).Z());}
};

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
      const KDTreeNode* GetParent() const {return fParent;}
      const KDTreeNode& FindContainingNode(const Point& point) const;
      const KDTreeNode& CheckParentBranches(const Point& point, const KDTreeNode& prevBest) const;
      void OutputGraphViz(std::ostream& out) const;
      
   private:
      int fDepth;
      Point* fPoint;
      KDTreeNode *fParent;
      KDTreeNode *fLeft;
      KDTreeNode *fRight;	
};

//______________________________________________________________________________
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
