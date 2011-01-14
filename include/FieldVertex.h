#ifndef __FIELD_VERTEX_H
#define __FIELD_VERTEX_H

#include "TVector3.h"

class FieldVertex : public TObject {
   public:
      // constructors
      FieldVertex();
      FieldVertex(double x, double y, double z, double bx, double by, double bz);
      FieldVertex(const FieldVertex& other);
      FieldVertex& operator=(const FieldVertex&);
      virtual ~FieldVertex();
      
      bool operator==(const FieldVertex& other) const; 
      bool operator!=(const FieldVertex& other) const {return !(*this==other);}

      // getters
      double X() const {return fX;}
      double Y() const {return fY;}
      double Z() const {return fZ;}
      TVector3 FieldVector() const {return fBField;}
      
      // setters
      void SetX(double x){fX=x;}
      void SetY(double y){fY=y;}
      void SetZ(double z){fZ=z;}

      std::string ToString() const;
      double DistanceTo(const FieldVertex& vertex) const;
      double SquaredDistanceTo(const FieldVertex& vertex) const;
      
   private:
      double fX,fY,fZ;
      TVector3 fBField;
      
   ClassDef(FieldVertex, 1)
};

class sortX {
	public:
	bool operator() (const FieldVertex *a, const FieldVertex *b) const { return ((*a).X() < (*b).X());}
};

class sortY {
	public:
	bool operator() (const FieldVertex *a, const FieldVertex *b) const { return ((*a).Y() < (*b).Y());}
};

class sortZ {
	public:
	bool operator() (const FieldVertex *a, const FieldVertex *b) const { return ((*a).Z() < (*b).Z());}
};

#endif