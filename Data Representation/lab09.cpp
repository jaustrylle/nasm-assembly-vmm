// CS 4301
// Lab 09

#include <turing.h>

/*
Turing machine:
- Output 1 if string has odd # occurrences of 'aa', else 0
*/

Turing::Turing(){
    name = "Serena Reese";
    labNumber = 9;
	description = "{(x=, x=ans) | x is in {a,b}*,\n"
				  "               ans = 1 if x contains an odd number of occurrences of the substring aa\n"
				  "               ans = 0 otherwise}";
}

/*
1) Scan L -> R
2) Every time 'aa' seen, toggle flag from even to odd
3) When '=' seen, write 1 for odd, 0 for even
*/

void Turing::initializeMachine()
{
    // STATE 1 (even, prev != 'a')
    machine.insert({1, TableEntry('a','a',1,2)});
    machine.insert({1, TableEntry('b','b',1,1)});
	machine.insert({1, TableEntry(' ',' ',1,1)});
	machine.insert({1, TableEntry('=','=',1,5)});
	
    // STATE 2 (even, prev = 'a')
    machine.insert({2, TableEntry('a','a',1,4)}); // 'aa'
    machine.insert({2, TableEntry('b','b',1,1)});
	machine.insert({2, TableEntry('=','=',1,5)});

    // STATE 3 (odd, prev != 'a')
    machine.insert({3, TableEntry('a','a',1,4)});
    machine.insert({3, TableEntry('b','b',1,3)});
	machine.insert({3, TableEntry('=','=',1,6)});

    // STATE 4 (odd, last = 'a')
    machine.insert({4, TableEntry('a','a',1,2)}); // 'aa'
    machine.insert({4, TableEntry('b','b',1,3)});
	machine.insert({4, TableEntry('=','=',1,6)});

	machine.insert({5, TableEntry(' ','0',0,0)});
	machine.insert({6, TableEntry(' ','1',0,0)});
}