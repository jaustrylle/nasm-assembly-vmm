// CS 4301
// Lab 07

#include <pda.h>

/* PDA accepting w or w el of {a,b}*, with w having 2x as a's as b's
1. main appends % to each line when reading in
2. halt is 0, start is 1
3. stack initialized to @ for each input string tested
*/

PDA::PDA(){
    name = "Test Testman";
    labNumber = 7;
    description = "{w | w in {a, b}* and w has twice as many a's as b's}";
}

// for every 1 b, there must be 2 a's, so #a = 2 * #b

/* stack tracks the balance between a and b
- @ marks the bottom
- A represents an unmatched a
*/

void PDA::initializeMachine() {

    // ----------------- STATE 1 ----------------------
    
    // --- Input 'a' ---
	
    // If we have b-debt (B), cancel an a
    machine.insert({1, TableEntry('B', 'a', "-B", 1)});
	
    // If no debt, push an A (surplus a)
    machine.insert({1, TableEntry('A', 'a', "+A", 1)});
    machine.insert({1, TableEntry('@', 'a', "+A", 1)});

    // --- Input 'b' ---
	
    // 1 'b' is worth two units, so the first unit here; then move to State 2 for the second
	
    // If surplus 'a' exists, pop one A
    machine.insert({1, TableEntry('A', 'b', "-A", 2)});
	
    // If no surplus, push one B-debt
    machine.insert({1, TableEntry('B', 'b', "+B", 2)});
    machine.insert({1, TableEntry('@', 'b', "+B", 2)});

    // --- Accept Condition ---
	
    // Reach end of string '%' and stack is exactly the initial '@'
    machine.insert({1, TableEntry('@', '%', "*", 0)});


    // ----------------- STATE 2 ----------------------
	
    // Epsilon symbol does 2nd half of 2-1 ratio b balance 

    // Cancel the second a from surplus if it exists
    machine.insert({2, TableEntry('A', '*', "-A", 1)});
	
    // If no surplus 'a' remains, push the second 'B' debt
    machine.insert({2, TableEntry('B', '*', "+B", 1)});
	
    // If just popped the last 'A' and hit bottom, push 'B' for the second half
    machine.insert({2, TableEntry('@', '*', "+B", 1)});
}