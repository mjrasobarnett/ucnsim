/************************** MOTHER.CPP ****************** AgF 2007-08-01 *
*  'Mother-of-All' random number generator                               *
*                                                                        *
*  This is a multiply-with-carry type of random number generator         *
*  invented by George Marsaglia.  The algorithm is:                      *
*  S = 2111111111*X[n-4] + 1492*X[n-3] + 1776*X[n-2] + 5115*X[n-1] + C   *
*  X[n] = S modulo 2^32                                                  *
*  C = floor(S / 2^32)                                                   *
*                                                                        *
*  Note:                                                                 *
*  This implementation uses 64-bit integers for intermediate             *
*  calculations. Works only on compilers that support 64-bit integers.   *
*                                                                        *
* © 1999 - 2007 A. Fog.                                                  *
* GNU General Public License www.gnu.org/copyleft/gpl.html               *
*************************************************************************/

#include <fstream>

#include "MWC.hpp"

// Output random bits
uint32 MWC::generator() {
  uint64 sum;
  sum = (uint64)2111111111ul * (uint64)x[3] +
     (uint64)1492 * (uint64)(x[2]) +
     (uint64)1776 * (uint64)(x[1]) +
     (uint64)5115 * (uint64)(x[0]) +
     (uint64)x[4];
  x[3] = x[2];  x[2] = x[1];  x[1] = x[0];
  x[4] = uint32(sum >> 32);            // Carry
  x[0] = uint32(sum);                  // Low 32 bits of sum
  return x[0];
} 


// returns a random number between 0 and 1:
double MWC::rnd() {
   return (double) generator() * (1./(65536.*65536.));
}

// this function initializes the random number generator:
MWC::MWC () {
	std::ifstream dev_random;
	dev_random.open("/dev/random",std::ios::in);

	for (int i = 0; i < 5; i++) {
		uint32 b1=dev_random.get();
		uint32 b2=dev_random.get();
		uint32 b3=dev_random.get();
		uint32 b4=dev_random.get();
		uint32 seed=b1 | (b2 << 8) | (b3 << 16) | (b4 << 24);
		x[i] = seed;
	}
	// randomize some more
	for (int i=0; i<19; i++) generator();
}
