#ifndef __FIELD_VERTEX_H
#define __FIELD_VERTEX_H

#include "TVector3.h"

class FieldVertex {
   public:
      // constructors
      FieldVertex();
      FieldVertex(double x, double y, double z);
      FieldVertex(const FieldVertex& other);
      virtual ~FieldVertex();
      
      bool operator==(const FieldVertex& other) const; 
      bool operator!=(const FieldVertex& other) const {return !(*this==other);}

      // getters
      double X() const {return fPos.X();}
      double Y() const {return fPos.Y();}
      double Z() const {return fPos.Z();}

      // setters
      void SetX(double x) {fPos.SetX(x);}
      void SetY(double y) {fPos.SetY(y);}
      void SetZ(double z) {fPos.SetZ(z);}

      std::string ToString() const;
      double DistanceTo(const FieldVertex& vertex) const;
      double SquaredDistanceTo(const FieldVertex& vertex) const;
      
   private:
      TVector3 fPos;
};

class sortX {
	public:
	bool operator() (FieldVertex *a, FieldVertex *b) { return ((*a).X() < (*b).X());}
};

class sortY {
	public:
	bool operator() (FieldVertex *a, FieldVertex *b) { return ((*a).Y() < (*b).Y());}
};

class sortZ {
	public:
	bool operator() (FieldVertex *a, FieldVertex *b) { return ((*a).Z() < (*b).Z());}
};

#endif