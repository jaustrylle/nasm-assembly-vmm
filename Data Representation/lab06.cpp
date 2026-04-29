// Serena Reese - CS 3304 - Lab 06

#include <string>

using namespace std;

/*
1. receive 2 unsigned binary integer strings
2. return string with binary difference subtracting 2nd from 1st
3. 1st arg is minuend, 2nd is subtrahend
4. strings are arbitrary length
5. results are same length as minuend
*/

// NOTES:
// subtrahend may be shorter, missing bits = 0
// if subtraction = neg, borrow normally with two's complement w/o overflow
// keep leading zeroes

// EXAMPLE:
// 1000 - 111 = 0001 (subtract R to L)

string subtract(string minuend, string subtrahend){
	
	// start from least sig bit
	int i = minuend.length() - 1;
	int j = subtrahend.length() - 1;
	int borrow = 0;
	
	string result = minuend;
	
	// track a borrow (0 or 1)
	while(i >= 0){
		int m = minuend[i] - '0';
		int s = (j >= 0) ? subtrahend[j] - '0' : 0;
		
		// each pos (diff = minuend_bit - subtrahend_bit - borrow)
		int diff = m - s - borrow;
		
		// if diff < 0, add 2 and set borrow to 1
		if(diff < 0){
			diff += 2;
			borrow = 1;
		}
		// else borrow is 0
		else{
			borrow = 0;
		}
		
		result[i] = diff + '0';
		
		i--;
		j--;
	}
	
	return result;
}
