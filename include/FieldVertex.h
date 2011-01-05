#ifndef __POINT_HPP_
#define __POINT_HPP_

class Point {
   public:
      // constructors
      Point();
      Point(double x, double y, double z);
      Point(const Point& other);
      virtual ~Point();
      
      bool operator==(const Point& other) const; 
      bool operator!=(const Point& other) const {return !(*this==other);}

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

#endif