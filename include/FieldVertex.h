#ifndef __FIELD_VERTEX_H
#define __FIELD_VERTEX_H

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
      double X() const {return fX;}
      double Y() const {return fY;}
      double Z() const {return fZ;}

      // setters
      void SetX(double x){fX=x;}
      void SetY(double y){fY=y;}
      void SetZ(double z){fZ=z;}

      std::string ToString() const;
      double DistanceTo(const FieldVertex& vertex) const;
      double SquaredDistanceTo(const FieldVertex& vertex) const;
      
   private:
      double fX,fY,fZ;
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