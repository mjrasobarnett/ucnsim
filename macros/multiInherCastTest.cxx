// Test of downcasting problem in multiple inheritance
// This is why I cannot use multiple inheritance to fix ROOT interfaces since I always get 
// pointers to the ROOT types.

#include <iostream>

using namespace std;

class a {
protected:
   int a1;

public:
   a() {cout << "a constructor" << endl; a1=1;}
   ~a() {cout << "a destructor" << endl;}
   void geta1() const {cout << "a" << endl;}
};

class b {
public:
   b() {cout << "b constructor" << endl;}
   virtual ~b() {cout << "b destructor" << endl;}
   virtual void geta1() const = 0;
};

class c : public a, public b {
public:
   c() : a(), b() { cout << "c constructor" << endl;}
   virtual ~c() {cout << "c destructor" << endl;}
   virtual void geta1() const {cout << "c" << endl;}
};


int main() {
   c& objc = *new c();
   b& objb = objc;
   a& obja = objc;
   objc.geta1();
   objb.geta1();
   obja.geta1();
   
   // Show that downcasting from a to b doesn't work
   b* castb = static_cast<b*>(&obja);
   
   delete &objc;
   return 0;
}