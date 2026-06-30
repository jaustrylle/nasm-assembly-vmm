// CS 4301
// Lab 08

#include <pdt.h>

/*
Input is a string, output where # of c = min(# of a, # of b)
*/

PDT::PDT(){
    name = "Test Testman";
    labNumber = 8;
    description = "{a^ib^j,c^k | i, j > 0; k = min(i, j)}";
}

/*
EDGE CASES:
1. aab
2. aaab
3. aaaab
4. aaabb
*/

void PDT::initializeMachine()
{
	// ----------------------------------------

    // STATE 1: Reading a's

    // a -> push 'a'
    machine.insert({1, TableEntry('*', 'a', "+a", 1, '*')});

    // b -> pop 'a', output c, go to state 2
    machine.insert({1, TableEntry('a', 'b', "-a", 2, 'c')});

    // ----------------------------------------

    // STATE 2: Reading b's

    // b with 'a' on stack -> pop and output c
    machine.insert({2, TableEntry('a', 'b', "-a", 2, 'c')});

    // b with '@' on stack -> ignore
    machine.insert({2, TableEntry('@', 'b', "*", 2, '*')});
	
	// ----------------------------------------

    // % with '@' -> accept
    machine.insert({2, TableEntry('@', '%', "*", 0, '*')});
	machine.insert({2, TableEntry('a', '*', "-a", 2, '*')});
}