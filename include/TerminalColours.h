// -- This file contains a series of namespaces that contain the color/style codes
// -- for the bash shell, to give stylised output

// -- There are also two ostream manipulators: one to begin stylising what follows
// -- in a stream, the other to end stylising in the stream.

#include <string>
#include <iostream>
using namespace std;

// -- Text (Foreground) Colour codes
namespace Col {
   string none    = "0";
   string black   = "30";
   string red     = "31";
   string green   = "32";
   string yel     = "33";
   string blue    = "34";
   string pink    = "35";
   string cyan    = "36";
   string white   = "37";
}

// -- Style codes
namespace Style {
   string plain     = "0";
   string bold      = "1";
   string underline = "4";
   string strike    = "9";
}

// -- Background colour codes
namespace Bg {
   string black  = "";
   string grey   = "40";
   string red    = "41";
   string green  = "42";
   string yel    = "43";
   string blue   = "44";
   string pink   = "45";
   string cyan   = "46";
   string white  = "47";
}

// -- This is a kind of stream manipulator that takes arguments, so here we use it to begin
// -- colourising and stylising the rest of the stream. It takes three string arguments,
// -- that should be taken from the three namespaces defined above. The first
// -- is the colour of the text, the second is style of the text, the third is the
// -- background colour behind the text.

// -- This class is termed an 'Effector' (see Eckel's Thinking in C++ vol 2, Chapter 4 on Manipulators)

// -- Every instance of start_colour should be terminated by calling the stop_colour
// -- manipulator below.
class start_colour {
   string fColour;
   string fStyle;
   string fBackground;
public:
   start_colour(const string& colour, const string& style="0", const string& bg="") : fColour(colour), fStyle(style), fBackground(bg) {
      if (!fStyle.empty()) fStyle.append(";");
      if (!fBackground.empty()) fBackground.append(";");
   }
   friend ostream& operator<<(ostream& os, const start_colour& col) {
      return os << "\e[" << col.fStyle << col.fBackground << col.fColour << "m";
   }
};

// -- stop_colour takes no arguments and simply inserts the escape code to end
// -- stylised output into the stream. This should always be called after an instance
// -- of start_colour
ostream& stop_colour(ostream& os) {
  return os << "\e[m";
}

ostream& error(ostream& os) {
  return os << start_colour(Col::red, Style::bold);
}

ostream& info(ostream& os) {
  return os << start_colour(Col::green, Style::bold);
}

ostream& trash(ostream& os) {
  return os << start_colour(Col::black);
}

