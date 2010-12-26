#ifndef __MWC_HPP__
#define __MWC_HPP__

// the multiply-with-carry random number generator
// http://en.wikipedia.org/wiki/Multiply-with-carry
typedef long long          int64;   // 64 bit signed integer
typedef unsigned long long uint64;  // 64 bit unsigned integer
typedef int                int32;   // 32 bit signed integer
typedef unsigned int       uint32;  // 32 bit unsigned integer
class MWC {
	public:
	   double rnd(); // Get floating point random number
	   uint32 generator(); // generate next int
	   MWC(); // constructor - initialise buffer from /dev/random
		uint32 *get_state(){return x;}
		uint32 get_state(int i){return x[i];}
	protected:
	   uint32 x[5]; // state buffer
};
#endif
