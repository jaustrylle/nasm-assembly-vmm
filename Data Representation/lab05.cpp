// CS 3304 - Lab 05

#include <iostream>
#include <iomanip>
#include <climits>
#include <string>
#include <vector>

#include <type_traits>	// prepare bit mask

using namespace std;

/* NOTES:
1. receives an integral parameter
2. returns a string (internal binary rep. of input, aka two's complement)
3. must be portable (works no matter what machine is being used)
4. remember to use make clean command
*/

template <typename T>

string int2binary(T num){
	
	// type safety check
	static_assert(is_integral<T>::value, "int2binary requires an integral type.");
	
	// fail-safe
	using U = typename make_unsigned<T>::type;
	
	// get # of bits
	const int bits = sizeof(T) * CHAR_BIT;
	
	// convert to corresponding unsigned type
	U value = static_cast<U>(num);

	// extract bits from MSB to LSB, masking bits (avoid UB, handle negatives)
	string binary;
	binary.reserve(bits);
	
	for(int i = bits-1; i >= 0; --i){
		binary.push_back((value & (U(1) << i)) ? '1' : '0');
	}
	
	return binary;
}