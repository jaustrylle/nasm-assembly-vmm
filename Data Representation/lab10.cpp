// CS 4301
// Lab 10

#include <turing.h>

/*
Turing machine:
- string x elem of {a, b}*
- output 1 if # a's = 2 * #b's, else 0
*/

Turing::Turing(){
    name = "Test Testman";
    labNumber = 10;
	description = "{(x=, x=ans) | x is in {a,b}* and\n"
				  "               ans = 1 if x has twice as many a's as b's, and\n"
				  "               ans = 0 otherwise}";
}

/*
1 - scan L-R
2 - find b, mark it (b -> X)
3 - scan L to find first a and mark it (a -> Y)
4 - continue scanning, mark second a (a -> Y)
5 - if can't find 2 a's, output 0 and rej
6 - go to beg., find next unmarked b, repeat matching
7 - after all b's matched, if unmarked a leftover, rej. (0), else acc
*/

// LEFT = -1
// RIGHT = 1
// STOP = 0

void Turing::initializeMachine()
{
    // STATE 1 - +1 for leading blanks
    machine.insert({1, TableEntry(' ', ' ', 1, 1)}); 
    machine.insert({1, TableEntry('a', 'a', -1, 2)});
    machine.insert({1, TableEntry('b', 'b', -1, 2)});
    machine.insert({1, TableEntry('A', 'A', -1, 2)});
    machine.insert({1, TableEntry('B', 'B', -1, 2)});
    machine.insert({1, TableEntry('=', '=', 1, 7)}); 

    // STATE 2 - move back to start of input
    machine.insert({2, TableEntry('a', 'a', -1, 2)});
    machine.insert({2, TableEntry('b', 'b', -1, 2)});
    machine.insert({2, TableEntry('A', 'A', -1, 2)});
    machine.insert({2, TableEntry('B', 'B', -1, 2)});
    machine.insert({2, TableEntry('=', '=', -1, 2)});
    machine.insert({2, TableEntry(' ', ' ', 1, 3)});

    // STATE 3 - search for unmarked b's
    machine.insert({3, TableEntry('a', 'a', 1, 3)}); 
    machine.insert({3, TableEntry('A', 'A', 1, 3)});
    machine.insert({3, TableEntry('B', 'B', 1, 3)});
    machine.insert({3, TableEntry('b', 'B', -1, 10)});
    machine.insert({3, TableEntry('=', '=', -1, 6)});	// LEFTOVERS

    // STATE 4 - search for 1st unmarked 'a'
    machine.insert({4, TableEntry('A', 'A', 1, 4)});
    machine.insert({4, TableEntry('B', 'B', 1, 4)});
    machine.insert({4, TableEntry('b', 'b', 1, 4)});
    machine.insert({4, TableEntry('a', 'A', -1, 11)});
    machine.insert({4, TableEntry('=', '=', 0, 8)});	// REJECT

    // STATE 5 - search for 2nd unmarked 'a'
    machine.insert({5, TableEntry('A', 'A', 1, 5)});
    machine.insert({5, TableEntry('B', 'B', 1, 5)});
    machine.insert({5, TableEntry('b', 'b', 1, 5)});
    machine.insert({5, TableEntry('a', 'A', -1, 2)});
    machine.insert({5, TableEntry('=', '=', 0, 8)});	// REJECT

    // STATE 6 - search for leftover 'a'
    machine.insert({6, TableEntry('A', 'A', -1, 6)}); 
    machine.insert({6, TableEntry('B', 'B', -1, 6)}); 
    machine.insert({6, TableEntry('b', 'b', -1, 6)});
    machine.insert({6, TableEntry(' ', ' ', 1, 9)});
    machine.insert({6, TableEntry('a', 'a', 0, 8)});	// REJECT
	
    // STATE 7 - accept
    machine.insert({7, TableEntry(' ', '1', 0, 0)});
	
    // STATE 8 - reject
    machine.insert({8, TableEntry('a', 'a', 1, 8)});
    machine.insert({8, TableEntry('A', 'A', 1, 8)});
    machine.insert({8, TableEntry('B', 'B', 1, 8)});
    machine.insert({8, TableEntry('b', 'b', 1, 8)});
    machine.insert({8, TableEntry('=', '=', -1, 12)});

    // STATE 9 - unmark tape to the right
    machine.insert({9, TableEntry('A', 'a', 1, 9)}); 
    machine.insert({9, TableEntry('B', 'b', 1, 9)}); 
    machine.insert({9, TableEntry('a', 'a', 1, 9)}); 
    machine.insert({9, TableEntry('b', 'b', 1, 9)}); 
    machine.insert({9, TableEntry('=', '=', 1, 7)});
	
	// STATE 10 - reset head of tape (right)
    machine.insert({10, TableEntry('a', 'a', -1, 10)});
    machine.insert({10, TableEntry('A', 'A', -1, 10)});
    machine.insert({10, TableEntry('B', 'B', -1, 10)});
    machine.insert({10, TableEntry(' ', ' ', 1, 4)});
	
	// STATE 11 - reset head of tape (left)
    machine.insert({11, TableEntry('a', 'a', -1, 11)});
    machine.insert({11, TableEntry('b', 'b', -1, 11)});
    machine.insert({11, TableEntry('A', 'A', -1, 11)});
    machine.insert({11, TableEntry('B', 'B', -1, 11)});
    machine.insert({11, TableEntry(' ', ' ', 1, 5)});

    // STATE 12 - unmark tape to the left
    machine.insert({12, TableEntry('A', 'a', -1, 12)});
    machine.insert({12, TableEntry('B', 'b', -1, 12)});
    machine.insert({12, TableEntry('a', 'a', -1, 12)});
    machine.insert({12, TableEntry('b', 'b', -1, 12)});
    machine.insert({12, TableEntry(' ', ' ', 1, 13)});

    // STATE 13 - find '='
    machine.insert({13, TableEntry('a', 'a', 1, 13)});
    machine.insert({13, TableEntry('b', 'b', 1, 13)});
    machine.insert({13, TableEntry('=', '=', 1, 14)});

    // STATE 14 - final reject
    machine.insert({14, TableEntry(' ', '0', 0, 0)});
}