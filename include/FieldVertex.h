#ifndef __FIELD_VERTEX_H
#define __FIELD_VERTEX_H

#include "TVector3.h"
#include "Point.h"

class FieldVertex : public TObject {
   private:
      Point fPoint;
      TVector3 fField;

   public:
      // constructors
      FieldVertex();
      FieldVertex(double x, double y, double z, double t, double fx, double fy, double fz);
      FieldVertex(const Point& point, const TVector3& field);
      FieldVertex(const FieldVertex& other);
      FieldVertex& operator=(const FieldVertex&);
      virtual ~FieldVertex();
      
      bool operator==(const FieldVertex& other) const; 
      bool operator!=(const FieldVertex& other) const {return !(*this==other);}

      // getters
      double X() const {return fPoint.X();}
      double Y() const {return fPoint.Y();}
      double Z() const {return fPoint.Z();}
      double T() const {return fPoint.T();}
      double Fx() const {return fField.X();}
      double Fy() const {return fField.Y();}
      double Fz() const {return fField.Z();}
      const Point& GetPoint() const {return fPoint;}
      const TVector3& GetField() const {return fField;}
      
      // setters
      void SetPoint(const Point& point) {fPoint = point;}
      void SetPosition(double x, double y, double z) {fPoint.SetPoint(x,y,z,fPoint.T());}
      void SetField(const TVector3& field) {fField = field;}
      void SetField(double fx, double fy, double fz) {fField.SetXYZ(fx,fy,fz);}
      void SetT(double t) {fPoint.SetT(t);}

      std::string ToString() const;
      double DistanceTo(const FieldVertex& vertex) const;
      double SquaredDistanceTo(const FieldVertex& vertex) const;

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
