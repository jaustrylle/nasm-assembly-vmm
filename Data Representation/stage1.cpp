// CS4301
// Stage 1

#include <stage1.h>

#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include <stdio.h>		// to close files
#include <cctype>		// nextChar, nextToken
#include <iomanip>		// format in/out streams using setw
#include <ctime>		// time in listing header
#include <vector>		// for ids() lists
#include <sstream>		// for varStmts(), parsing "many, some"

#include <stack>		// pushOperand, pushOperator...
#include <set>			// emitted tracking

using namespace std;

/*
Our specs enforce this structure STRICTLY:
1. program
2. progStmt
3. const (optional)
4. var (optional)
5. begin
6. beginEndStmt
7. EOF

Each grammar function assumes token already holds the current symbol.

Exceptional cases:
018.asm
036.asm

Even if a program has errors, the .lst file must contain:
1. header
2. source listing
3. specific error message
4. compilation terminated trailer

SYSTEM MUST FOLLOW:
operand1 = RIGHT (1st to pop)
operand2 = LEFT (2nd to pop)
*/

// ------------------------------------------------------------- //
// PUBLIC functions declared in stage0.h //
/*
string getInternalName() const
storeTypes getDataType() const
modes getMode() const
string getValue() const
allocation getAlloc() const
int getUnits() const
void setInternalName(string s)
void setDataType(storeTypes st)
void setMode(modes m)
void setValue(string s)
void setAlloc(allocation a)
void setUnits(int i)
*/

// ------------------------------------------------------------- //
/*

Six new keywords:
-- integer arithmetic operators --
1. mod
2. div
-- boolean operators --
3. and
4. or
-- input/output --
5. read
6. write

Nine new tokens:
-- assignment operator --	// have scanner check whether '=' follows ':'
1. :=
-- multiplication of integer operands --
2. *
-- grouping expressions --
3. (
4. )
-- relational operators --
5. <>
6. <
7. <=
8. >=
9. >

--> result of arithmetic op is always integer
--> result of boolean op is boolean
--> result of relational op is boolean

- A (eax) and D (edx) will be used, D mostly for remainder of mod division
- A reg assigned to at most one operand at a time

*/
// ------------------------------------------------------------- //
// GLOBAL VARIABLES AND FUNCTIONS

int I_count = 0;
int B_count = 0;

bool listingHeaderCreated = false;
bool begChar = true;

stack<string> tempStack;
std::set<string> emitted;

// ------------------------------------------------------------- //

Compiler::Compiler(char **argv)		// constructor
{
    if (!argv || !argv[1] || !argv[2] || !argv[3])
        processError("missing command line arguments");

    sourceFile.open(argv[1]);
    listingFile.open(argv[2]);
    objectFile.open(argv[3]);
	
    if (!sourceFile)
        processError("Cannot open source file");
    if (!listingFile)
        processError("Cannot open listing file");
    if (!objectFile)
        processError("Cannot open object file");

    lineNo = 1;
}

// this function closes all files
Compiler::~Compiler()							// destructor
{
	// add error handling for if file already open
	sourceFile.close();
	listingFile.close();
	objectFile.close();
}

//////////////////// OUTPUT ////////////////////

void Compiler::createListingHeader()
{
    time_t now = time(0);

    listingFile << "STAGE1:  TEST TESTMAN"
                << "\t\t"
				<< ctime(&now)
				<< endl;

	listingFile << std::left
				<< std::setw(22) << "LINE NO."
				<< "SOURCE STATEMENT\n\n";

    listingHeaderCreated = true;
}

void Compiler::parser()
{
	// main does this: createListingHeader, parser, createListingTrailer
    nextChar();
    token = nextToken();

    if (token != "program")
        processError("keyword 'program' expected");

    prog();
	return;
}

void Compiler::createListingTrailer()
{
    listingFile << "COMPILATION TERMINATED      "
                << errorCount
                << (errorCount == 1 ? " ERROR ENCOUNTERED" : " ERRORS ENCOUNTERED")
                << endl;
}

//////////////////// GRAMMAR ////////////////////

void Compiler::prog()           // stage 0, production 1
{
	// EOF is NOT the same as END_OF_FILE!
    if (token != "program")
        processError("keyword 'program' expected");

    progStmt();

    if (token == "const")
        consts();

    if (token == "var")
        vars();

    if (token != "begin")
        processError("keyword 'begin' expected");

    beginEndStmt();

    if (token != "EOF")
        processError("no text may follow 'end'");
}

void Compiler::progStmt()       // stage 0, production 2
{
	// BOTH returns AND assigns "token"
	// also, our specs require "x = nextToken()"
	// #deterministicTokenFlow
	
    if (token != "program")
        processError("keyword 'program' expected");

    string x = nextToken();

    if (!isNonKeyId(token))
        processError("program name expected");

    if (nextToken() != ";")
        processError("semicolon expected");

    nextToken();

    code("program", x);

    insert(x, PROG_NAME, CONSTANT, x, NO, 0);
}

void Compiler::consts()         // stage 0, production 3
{
    if (token == "const") 
    {
        token = nextToken();
        constStmts();
    }
}

void Compiler::vars()           // stage 0, production 4
{
    if (token == "var") 
    {
        token = nextToken();
        varStmts();
    }
}

void Compiler::beginEndStmt()   // stage 0, production 5
// REVISION: 'begin' EXEC_STMTS 'end' '.'
{	
	if (token != "begin")
		processError("keyword 'begin' expected");

	token = nextToken();
	
	execStmts();

	if (token != "end")
		processError("keyword 'end' expected");

	token = nextToken();

	if (token != ".")
		processError("period expected");

	token = nextToken();   // should now be EOF or nothing

	code("end", ".");
	
	listingFile << "\n";
}

//////////////////// CONST ////////////////////

void Compiler::constStmts()     // stage 0, production 6
{
    while (isNonKeyId(token))
    {
        string x = token;
        token = nextToken();

        if (token != "=")
            processError("'=' expected");

        token = nextToken();

        string y;

        if (token == "+" || token == "-")
        {
            string sign = token;
            token = nextToken();

            if (!isInteger(token))
                processError("integer expected after sign");

            y = sign + token;
        }
        else if (token == "not")
        {
            token = nextToken();

            if (!isBoolean(token))
                processError("boolean literal expected after 'not'");

            y = (token == "true") ? "false" : "true";
        }
        else
        {
            y = token;
        }

        token = nextToken();

        if (token != ";")
            processError("semicolon expected");

        storeTypes t = whichType(y);
        insert(x, t, CONSTANT, whichValue(y), YES, 1);

        token = nextToken();
    }
}

//////////////////// VAR ////////////////////

void Compiler::varStmts()       // stage 0, production 7
{
    while (isNonKeyId(token))
    {
        string idList = ids();

        if (token != ":")
            processError("\":\" expected");

        token = nextToken();

        if (token != "integer" && token != "boolean")
            processError("illegal type follows \":\"");

        string type = token;
        token = nextToken();

        if (token != ";")
            processError("semicolon expected");

        // insert identifiers
        stringstream ss(idList);
        string id;

        while (getline(ss, id, ','))
        {
            insert(id,
                   (type == "integer" ? INTEGER : BOOLEAN),
                   VARIABLE, "", YES, 1);
        }

        token = nextToken();  // move to next declaration OR begin
    }
}

string Compiler::ids()          // stage 0, production 8
{
    if (!isNonKeyId(token))
        processError("non-keyword identifier expected");

    string result = token;

    token = nextToken();

    while (token == ",")
    {
        token = nextToken();

        if (!isNonKeyId(token))
            processError("non-keyword identifier expected");

        result = result + "," + token;

        token = nextToken();
    }

    return result;
}

//////////////////// EXPRESSIONS ////////////////////

void Compiler::execStmts() // stage 1, production 2
{
	// EXEC_STMT EXEC_STMTS | epsilon
	// NON_KEY_ID, 'read', 'write', 'end'
	
    while (token == "read" || token == "write" || isNonKeyId(token))
    {
        try
        {
            execStmt();
        }
        catch (...)
        {
            // synchronize to next statement boundary
            while (token != ";" && token != "end" && token != "EOF")
                token = nextToken();

            if (token == ";")
                token = nextToken();
        }
    }
}

void Compiler::execStmt() // stage 1, production 3
{
	// ASSIGN_STMT | READ_STMT | WRITE_STMT
	// NON_KEY_ID, 'read', 'write'

    if (token == "read")
        readStmt();
    else if (token == "write")
        writeStmt();
    else if (isNonKeyId(token))
        assignStmt();
    else
        processError("statement expected");
}

void Compiler::assignStmt() // stage 1, production 4
{
	// NON_KEY_ID ':=' EXPRESS ';'
	// NON_KEY_ID

    string lhs = token;
    token = nextToken(); // consume id

    if (token != ":=")
        processError("':=' expected");

    token = nextToken(); // move into expression

    express();

    if (token != ";")
        processError("one of \"*\", \"and\", \"div\", \"mod\", \")\", \"+\", \"-\", \";\", \"<\", \"<=\", \"<>\", \"=\", \">\", \">=\", or \"or\" expected");

    string rhs = popOperand();  // expression result

    code(":=", rhs, lhs);

    token = nextToken();
}

void Compiler::readStmt() // stage 1, production 5
{
	// 'read' READ_LIST ';'
	// 'read'

    token = nextToken();  // consume 'read'

    if (token != "(")
        processError("'(' expected");

    token = nextToken();

    string x = ids();

    if (token != ")")
        processError("')' expected");

    token = nextToken();

    if (token != ";")
        processError("semicolon expected");

    code("read", x);

    token = nextToken();   // next statement
}

void Compiler::writeStmt() // stage 1, production 7
{
	// 'write' WRITE_LIST ';'
	// 'write'

    token = nextToken();

    if (token != "(")
        processError("'(' expected");

    token = nextToken();

    string x = ids();   // assumes ids() handles the identifier list

    if (token != ")")
        processError("')' expected");

    token = nextToken();

    code("write", x);

    if (token != ";")
        processError("semicolon expected");

    token = nextToken();
}

void Compiler::express() // stage 1, production 9
{
	// TERM EXPRESSES
	// 'not', 'true', 'false', '(', '+', '-', INTEGER, NON_KEY_ID

    term();
    expresses();
}

void Compiler::expresses() // stage 1, production 10
{
	// REL_OP TERM EXPRESSES
	// '<>', '=', '<=', '>=', '<', '>', '), ',', ';'

    if (token == "=" || token == "<>" || token == "<" ||
        token == "<=" || token == ">" || token == ">=")
    {
        string op = token;
        token = nextToken();
        term();

        string right = popOperand();
        string left  = popOperand();

        code(op, right, left); 

        expresses(); 
    }
}

void Compiler::term() // stage 1, production 11
{
	// FACTOR TERMS
	// 'not', 'true', 'false', '(', '+', '-', INTEGER, NON_KEY_ID
	
    factor();
    terms();
}

void Compiler::terms() // stage 1, production 12
{
    if (token == "+" || token == "-" || token == "or")
    {
        string op = token;
        // No need to pushOperator here if you pass it directly to code()
        token = nextToken();
        factor();

        string right = popOperand();
        string left  = popOperand();

        // code() calls emitAdditionCode, which handles getTemp and pushOperand
        code(op, right, left); 

        terms(); // Continue recursion
    }
}

void Compiler::factor() // stage 1, production 13
{
	// PART FACTORS
	// 'not', 'true', 'false', '(', '+', '-', INTEGER, NON_KEY_ID

    part();
    factors();
}

void Compiler::factors() // stage 1, production 14
{
    if (token == "*" || token == "div" ||
        token == "mod" || token == "and")
    {
        string op = token;
        token = nextToken();
        part();

        string right = popOperand();
        string left  = popOperand();

        code(op, right, left);

        factors();
    }
}

void Compiler::part() // stage 1, production 15
// Handles unary ops, literals, ids, parentheses.
{
    string unaryOp = "";

    if (token == "not")
    {
        token = nextToken();
        part();
        string op = popOperand();
        code("not", op);
        return;
    }
    else if (token == "+")
    {
        token = nextToken();
        // Do not call part() recursively. Fall through to the unsigned logic below.
    }
    else if (token == "-")
    {
        unaryOp = "-";
        token = nextToken();
        // Do not call part() recursively. Fall through to the unsigned logic below.
    }

    // --- UNSIGNED LOGIC SECTION ---
    // If we are here, we MUST find an operand. 
    // If the token is another '+', it will hit the 'else' and throw the error.

	if (isInteger(token))
    {
        // Combine sign with literal if we have a unary minus
        string literalValue = (unaryOp == "-") ? "-" + token : token;

        if (symbolTable.count(literalValue) == 0)
            insert(literalValue, INTEGER, CONSTANT, literalValue, YES, 1);
        
        pushOperand(literalValue);
        token = nextToken();
        
        // Clear unaryOp so we don't emit a 'neg' instruction later
        unaryOp = ""; 
    }
    else if (isBoolean(token))
    {
        if (symbolTable.count(token) == 0)
            insert(token, BOOLEAN, CONSTANT, token, YES, 1);
        pushOperand(token);
        token = nextToken();
    }
	else if (isNonKeyId(token))
    {
        // Validation occurs HERE, not in emit
        if (symbolTable.count(token) == 0)
        {
            processError("reference to undefined symbol " + token);
        }

        // Now that we know it exists, it's safe to check type and push
        whichType(token); 
        pushOperand(token);
        token = nextToken();
    }
    else if (token == "(")
    {
        token = nextToken();
        express();
        if (token != ")")
            processError("')' expected");
        token = nextToken();
    }
    else
    {
        // This catches the "+ + +5" case because token is '+' 
        // where an operand was expected.
        processError("expected '(', integer, or non-keyword id; found " + token);
    }

    // If we had a unary minus, emit negation code now
    if (unaryOp == "-")
    {
        string op = popOperand();
        code("neg", op);
    }
}

//////////////////// HELPER ////////////////////

bool Compiler::isKeyword(string s) const  // determines if s is a keyword
{
    return (s == "program" || s == "const" || s == "var" ||
            s == "begin" || s == "end" ||
            s == "integer" || s == "boolean" ||
            s == "read" || s == "write");
}

bool Compiler::isSpecialSymbol(char c) const
{
	// determines if c is a special symbol
	
    string symbols = "+-*/()=,;:.<>";

    return symbols.find(c) != string::npos;
}

bool Compiler::isNonKeyId(string s) const // determines if s is a non_key_id
{
    if (s.empty() || !islower(s[0]))
        return false;

    for (char c : s)
        if (!isalnum(c) && c != '_')
            return false;

    return !isKeyword(s);
}

bool Compiler::isInteger(string s) const  // determines if s is an integer
{
    if (s.empty()) return false;

    int i = 0;

    if (s[0] == '+' || s[0] == '-')
        i = 1;

    if (i == (int)s.size())
        return false;

    for (; i < (int)s.size(); i++)
        if (!isdigit(s[i]))
            return false;

    return true;
}

bool Compiler::isBoolean(string s) const  // determines if s is a boolean
{
	return (s == "true" || s == "false");
}

bool Compiler::isLiteral(string s) const  // determines if s is a literal
{
	return isInteger(s) || isBoolean(s);
}

//////////////////// ACTION ////////////////////

void Compiler::insert(string externalName, storeTypes inType, modes inMode,
                      string inValue, allocation inAlloc, int inUnits)
{
    if (symbolTable.count(externalName) > 0)
        processError("symbol " + externalName + " is multiply defined");

    if (isKeyword(externalName))
        processError("keyword cannot be used as an identifier");

    string internalName;

    // RULE: If it's a Temporary (T0, T1...), the Internal Name IS the External Name.
    // Otherwise, use genInternalName for standard variables/constants.
    if (isTemporary(externalName))
    {
        internalName = externalName;
    }
    else if (inMode == CONSTANT && externalName == "true")
    {
        internalName = "TRUE";
    }
    else if (inMode == CONSTANT && externalName == "false")
    {
        internalName = "FALSE";
    }
    else
    {
        internalName = genInternalName(inType);
    }

    symbolTable.insert({
        externalName,
        SymbolTableEntry(internalName, inType, inMode, inValue, inAlloc, inUnits)
    });
}
			
storeTypes Compiler::whichType(string name)
{
    if (symbolTable.count(name) > 0)
        return symbolTable.at(name).getDataType();
    
    if (isInteger(name)) // Make sure isInteger handles a leading '-'
        return INTEGER;
        
    if (isBoolean(name))
        return BOOLEAN;

    return UNKNOWN;
}

string Compiler::whichValue(string name)
{
    // 1. Check if it's already an established symbol
    if (symbolTable.count(name) > 0)
        return symbolTable.at(name).getValue();

    // 2. Handle Boolean Literals (Normalize to -1 and 0)
    if (name == "true")
        return "-1";
    if (name == "false")
        return "0";

    // 3. It's a literal integer (e.g., "5" or "-5")
    return name;
}

void Compiler::code(string op, string operand1, string operand2)
{
    if (op == "program")
        emitPrologue(operand1);

    else if (op == "end")
        emitEpilogue();

    else if (op == "read")
        emitReadCode(operand1);

    else if (op == "write")
        emitWriteCode(operand1);
    
    else if (op == "div")
        emitDivisionCode(operand1, operand2);

    else if (op == "mod")
        emitModuloCode(operand1, operand2);

    else if (op == "+")
        emitAdditionCode(operand1, operand2);

    else if (op == "-")
        emitSubtractionCode(operand1, operand2);

    else if (op == "neg")
        emitNegationCode(operand1);

    else if (op == "not")
        emitNotCode(operand1);

    else if (op == "*")
        emitMultiplicationCode(operand1, operand2);

    else if (op == "and")
        emitAndCode(operand1, operand2);

    else if (op == "or")
        emitOrCode(operand1, operand2);

    else if (op == "=")
        emitEqualityCode(operand1, operand2);

    else if (op == "<>")
        emitInequalityCode(operand1, operand2);

    else if (op == "<")
        emitLessThanCode(operand1, operand2);

    else if (op == "<=")
        emitLessThanOrEqualToCode(operand1, operand2);

    else if (op == ">")
        emitGreaterThanCode(operand1, operand2);

    else if (op == ">=")
        emitGreaterThanOrEqualToCode(operand1, operand2);

    else if (op == ":=")
    {
        // Check if we are trying to assign to the program name
        if (whichType(operand2) == PROG_NAME)
            processError("destination of assignment cannot be a program name");
            
        emitAssignCode(operand1, operand2);
    }
    else
        processError("compiler error since function code should not be called with illegal arguments");
}

//////////////////// ACTION ROUTINES ////////////////////

void Compiler::pushOperator(string op)	// push name onto operatorStk
{
	operatorStk.push(op);
	return;
}

void Compiler::pushOperand(string operand)
{
    // identifiers must exist and be valid
    if (isNonKeyId(operand))
    {
        whichType(operand); // validate declaration (must trigger error if undefined)
    }

    // insert constants only if truly literal
    else if (isLiteral(operand))
    {
        if (symbolTable.count(operand) == 0)
        {
            insert(operand,
                   whichType(operand),
                   CONSTANT,
                   whichValue(operand),
                   YES,
                   1);
        }
    }

    operandStk.push(operand);
}

string Compiler::popOperator()	// pop name from operatorStk
{
    if (!operatorStk.empty())
    {
        string top = operatorStk.top();
        operatorStk.pop();
        return top;
    }

    processError("compiler error; operator stack underflow");
    return "";
}

string Compiler::popOperand()		// pop name from operandStk
{
    if (!operandStk.empty())
    {
        string top = operandStk.top();
        operandStk.pop();
        return top;
    }

    processError("compiler error; operand stack underflow");
    return "";
}

//////////////////// EMIT ////////////////////

void Compiler::emit(string label, string instruction, string operands,
		string comment)
{
    objectFile << std::left                      // left justification in objectFile
               << std::setw(8)  << label        // label field of width 8
               << std::setw(8)  << instruction  // instruction field of width 8
               << std::setw(24) << operands     // operands in field of width 24
               << comment << "\n";                        // output comment
}
		
void Compiler::emitPrologue(string progName, string)
{
    time_t now = time(0);
    string timeStr = ctime(&now);

    objectFile << "; TEST TESTMAN      " << timeStr;

    objectFile << "%INCLUDE \"Along32.inc\"" << endl;
    objectFile << "%INCLUDE \"Macros_Along.inc\"" << endl;
	objectFile << endl;
	
    emit("SECTION", ".text");
	
	// Truncate the program name to 15 characters here
    string truncatedName = progName.substr(0, 15);
    emit("global", "_start", "", "; program " + truncatedName);
	
	objectFile << endl;
	
	emit("_start:");
}

void Compiler::emitEpilogue(string, string)
{
    emit("", "Exit", "{0}");
	objectFile << endl;
    emitStorage();
}

void Compiler::emitStorage()
{
    emit("SECTION", ".data");

    // Order matters for some diffs! 
    // If your diffs still show FALSE and TRUE out of place, 
    // you may need to sort this loop or manually emit them first.
    for (const auto& pair : symbolTable)
    {
        const SymbolTableEntry& entry = pair.second;
        if (entry.getAlloc() == YES && entry.getMode() == CONSTANT)
        {
            string value = entry.getValue();
            if (value == "true") value = "-1";
            else if (value == "false") value = "0";
            
            // Limit comment length as per your original logic
            string commentName = pair.first.substr(0, 15);
            emit(entry.getInternalName(), "dd", value, "; " + commentName);
        }
    }
    
    objectFile << endl; 

    emit("SECTION", ".bss");
    std::set<string> emitted; // To track what we've already put in .bss

    // Single loop for all variables and temporaries
    for (const auto& pair : symbolTable)
    {
        const SymbolTableEntry& entry = pair.second;
        string internal = entry.getInternalName();

        // Check if it's something that belongs in .bss (Variables or Temps)
        if (entry.getMode() == VARIABLE || (entry.getMode() == VARIABLE && isTemporary(pair.first)))
        {
            if (emitted.find(internal) == emitted.end())
            {
                string commentName = pair.first.substr(0, 15);
                emit(internal, "resd", to_string(entry.getUnits()), "; " + commentName);
                emitted.insert(internal);
            }
        }
    }
}

//////////////////// EMIT ROUTINES ////////////////////

// REL_OP: '=', '<>', '<=', '>=', '<', '>'
// ADD_LEVEL_OP: '+', '-', 'or'
// MULT_LEVEL_OP: '*', 'div', 'mod', 'and'

void Compiler::emitReadCode(string operand, string operand2)
{
    stringstream ss(operand);
    string name;
    while (getline(ss, name, ','))
    {
        name.erase(0, name.find_first_not_of(" "));
        name.erase(name.find_last_not_of(" ") + 1);
        if (name == "") continue;

        if (symbolTable.count(name) == 0)
            processError("reference to undefined symbol " + name);
        if (symbolTable.at(name).getDataType() != INTEGER)
            processError("can't read variables of this type");
        if (symbolTable.at(name).getMode() != VARIABLE)
            processError("attempting to read to a read-only location");

        emit("", "call", "ReadInt", "; read int; value placed in eax");
        emit("", "mov", "[" + symbolTable.at(name).getInternalName() + "],eax", 
             "; store eax at " + name);

        // Force a clear so the next instruction doesn't try to optimize
        contentsOfAReg = ""; 
    }
}

void Compiler::emitWriteCode(string operand, string operand2)
{
    stringstream ss(operand);
    string name;
    while (getline(ss, name, ','))
    {
        name.erase(0, name.find_first_not_of(" "));
        name.erase(name.find_last_not_of(" ") + 1);
        if (name == "") continue;

        if (symbolTable.count(name) == 0)
            processError("reference to undefined symbol " + name);

        // The original compiler always performs this load for Write
        emit("", "mov", "eax,[" + symbolTable.at(name).getInternalName() + "]", 
             "; load " + name + " in eax");
        
        if (symbolTable.at(name).getDataType() == INTEGER || 
            symbolTable.at(name).getDataType() == BOOLEAN)
        {
            emit("", "call", "WriteInt", "; write int in eax to standard out");
        }

        emit("", "call", "Crlf", "; write \\r\\n to standard out");

        // Clear register state to match the reference compiler's "dumb" behavior
        contentsOfAReg = ""; 

        if (isTemporary(name)) freeTemp();
    }
}

void Compiler::emitAssignCode(string operand1, string operand2)
{
    // 1. Immediate Optimization: If source and dest are same, do nothing.
    if (operand1 == operand2) return;

    // 2. Semantic Checks
    if (whichType(operand1) != whichType(operand2))
        processError("incompatible types for operator ':='");

    if (symbolTable.at(operand2).getMode() != VARIABLE)
        processError("symbol on left-hand side of assignment must have a storage mode of VARIABLE");

    // 3. Load source into eax if not already there
    if (contentsOfAReg != operand1)
    {
        emit("", "mov", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand1);
        contentsOfAReg = operand1;
    }

    // 4. Store AReg into destination
    emit("", "mov", "[" + symbolTable.at(operand2).getInternalName() + "],eax", "; " + operand2 + " = AReg");

    // 5. Cleanup
    if (isTemporary(operand1)) freeTemp();

    // 6. Reset tracking
    // To match reference 101.asm/107.asm, we usually clear AReg 
    // unless the result of the assignment is needed (which it isn't in Pascal).
    contentsOfAReg = operand2; // Now eax "officially" contains the destination variable
}

void Compiler::emitAdditionCode(string op1, string op2)
{
    if (whichType(op1) != INTEGER || whichType(op2) != INTEGER) {
        processError("binary '+' requires integer operands");
    }

    // 1. Manage A Register state
    // If AReg holds a different value, clear it. If it's a temp, deassign it.
    if (contentsOfAReg != "" && contentsOfAReg != op1 && contentsOfAReg != op2) 
    {
        if (isTemporary(contentsOfAReg)) 
        {
            emit("", "mov", "[" + symbolTable.at(contentsOfAReg).getInternalName() + "],eax", "; deassign AReg");
            symbolTable.at(contentsOfAReg).setAlloc(YES);
        }
        contentsOfAReg = "";
    }

    // 2. Load Operand (Matches the "load [name] in eax" comment style)
    if (contentsOfAReg != op1 && contentsOfAReg != op2)
    {
        // Reference usually loads the second operand for addition
        emit("", "mov", "eax,[" + symbolTable.at(op2).getInternalName() + "]", "; load " + op2 + " in eax");
        contentsOfAReg = op2;
    }

    // 3. Perform Addition
    // Ensure the comment matches: "; AReg = left + right"
    string left = (contentsOfAReg == op1) ? op1 : op2;
    string right = (contentsOfAReg == op1) ? op2 : op1;
    
    emit("", "add", "eax,[" + symbolTable.at(right).getInternalName() + "]", 
         "; AReg = " + left + " + " + right);

    // 4. Cleanup
    if (isTemporary(op1)) freeTemp();
    if (isTemporary(op2)) freeTemp();

    // 5. THE MANDATORY STORE (Deassign)
    // This is the "non-lazy" fix that removes the T variable diffs.
    string temp = getTemp();
    symbolTable.at(temp).setDataType(INTEGER);

    emit("", "mov", "[" + symbolTable.at(temp).getInternalName() + "],eax", "; deassign AReg");
    symbolTable.at(temp).setAlloc(YES);

    contentsOfAReg = temp; 
    pushOperand(temp);
}

void Compiler::emitSubtractionCode(string operand1, string operand2)
{
    string right = operand1;
    string left = operand2;

    if (whichType(left) != INTEGER || whichType(right) != INTEGER)
        processError("binary '-' requires integer operands");

    // --- NEW: DEASSIGN CHECK ---
    // If AReg contains a temporary that isn't one of our current operands,
    // we must save it to memory before overwriting EAX.
    if (isTemporary(contentsOfAReg) && contentsOfAReg != left && contentsOfAReg != right)
    {
        emit("", "mov", "[" + symbolTable.at(contentsOfAReg).getInternalName() + "],eax", "; deassign AReg");
        symbolTable.at(contentsOfAReg).setAlloc(YES); 
        contentsOfAReg = ""; 
    }

    // Now proceed with the load
    if (contentsOfAReg != left)
    {
        emit("", "mov", "eax,[" + symbolTable.at(left).getInternalName() + "]", "; AReg = " + left);
        contentsOfAReg = left;
    }

    emit("", "sub", "eax,[" + symbolTable.at(right).getInternalName() + "]", 
         "; AReg = " + left + " - " + right);

    // Standard housekeeping
    if (isTemporary(right)) freeTemp();
    if (isTemporary(left)) freeTemp();

    string temp = getTemp();
    symbolTable.at(temp).setDataType(INTEGER);
    
    contentsOfAReg = temp;
    pushOperand(temp);
}

void Compiler::emitMultiplicationCode(string op1, string op2)
{
    if (whichType(op1) != INTEGER || whichType(op2) != INTEGER)
        processError("binary '*' requires integer operands");

    // 1. Load logic: Multiplication is commutative, so check both.
    // If neither is in AReg, we must load one.
    if (contentsOfAReg != op1 && contentsOfAReg != op2)
    {
        emit("", "mov", "eax,[" + symbolTable.at(op2).getInternalName() + "]", "; AReg = " + op2);
        contentsOfAReg = op2;
    }

    // 2. Emit the imul
    if (contentsOfAReg == op1)
    {
        emit("", "imul", "dword [" + symbolTable.at(op2).getInternalName() + "]", 
             "; AReg = " + op1 + " * " + op2);
    }
    else // contentsOfAReg is op2
    {
        emit("", "imul", "dword [" + symbolTable.at(op1).getInternalName() + "]", 
             "; AReg = " + op2 + " * " + op1);
    }

    // 3. Cleanup temps used in the calculation
    if (isTemporary(op1)) freeTemp();
    if (isTemporary(op2)) freeTemp();

    // 4. THE LAZY RULE:
    // Unlike Addition, do NOT emit a 'mov [temp], eax' here.
    // Just grab a temp name to represent the value currently in eax.
    string temp = getTemp();
    symbolTable.at(temp).setDataType(INTEGER);
    
    contentsOfAReg = temp; // Register now logically holds this temp
    pushOperand(temp);
}

void Compiler::emitDivisionCode(string operand1, string operand2)
{
    string right = operand1; // divisor
    string left = operand2;  // dividend
    
    if (whichType(right) != INTEGER || whichType(left) != INTEGER)
        processError("binary 'div' requires integer operands");

    // 1. DEASSIGN LOGIC
    // If something else is in eax, and it's a temp we need later, save it.
    if (isTemporary(contentsOfAReg) && contentsOfAReg != left)
    {
        emit("", "mov", "[" + symbolTable.at(contentsOfAReg).getInternalName() + "],eax", "; deassign AReg");
    }

    // 2. LOAD DIVIDEND
    // Division is NOT commutative. The dividend MUST be in eax.
    if (contentsOfAReg != left)
    {
        emit("", "mov", "eax,[" + symbolTable.at(left).getInternalName() + "]", "; AReg = " + left);
    }

    // 3. PREPARE AND DIVIDE
    emit("", "cdq", "", "; sign extend dividend from eax to edx:eax");
    emit("", "idiv", "dword [" + symbolTable.at(right).getInternalName() + "]", "; AReg = " + left + " div " + right);

    // 4. CLEANUP
    if (isTemporary(right)) freeTemp();
    if (isTemporary(left)) freeTemp();

    // 5. THE LAZY RULE
    // Do NOT emit a 'mov [temp], eax' here.
    string temp = getTemp();
    symbolTable.at(temp).setDataType(INTEGER);

    contentsOfAReg = temp; 
    pushOperand(temp);
}

void Compiler::emitModuloCode(string operand1, string operand2)
{
    string right = operand1;
    string left = operand2;
    
    if (whichType(right) != INTEGER || whichType(left) != INTEGER)
        processError("binary 'mod' requires integer operands");

    // 1. Load left operand if not already in AReg
	if (contentsOfAReg != left)
	{
		emit("", "mov", "eax,[" + symbolTable.at(left).getInternalName() + "]", "; AReg = " + left);
		contentsOfAReg = left;
	}

    emit("", "cdq", "", "; sign extend dividend from eax to edx:eax");

    // 2. Change: Reference expects 'div' in the comment even for mod
    emit("", "idiv", "dword [" + symbolTable.at(right).getInternalName() + "]", 
         "; AReg = " + left + " div " + right);

    // 3. Change: Use xchg and match the reference comment
    emit("", "xchg", "eax,edx", "; exchange quotient and remainder");
    
    // 4. Handle Temporaries
    if (isTemporary(right)) freeTemp();
    if (isTemporary(left)) freeTemp();

    // 5. Change: Get a temp but DON'T emit the mov instruction yet (Lazy Store)
    string temp = getTemp();
    symbolTable.at(temp).setDataType(INTEGER);

    // Update AReg tracking and push to operand stack
    contentsOfAReg = temp; 
    pushOperand(temp);
}

void Compiler::emitNegationCode(string operand1, string)
{
    if (whichType(operand1) != INTEGER)
        processError("unary '-' requires integer operand");
	
	// --- DEASSIGN CHECK ---
    // If AReg holds a temp we need later, save it before loading operand1
    if (isTemporary(contentsOfAReg) && contentsOfAReg != operand1)
    {
        emit("", "mov", "[" + symbolTable.at(contentsOfAReg).getInternalName() + "],eax", "; deassign AReg");
        symbolTable.at(contentsOfAReg).setAlloc(YES);
        contentsOfAReg = "";
    }

    // Load if not already there
    if (contentsOfAReg != operand1)
    {
        emit("", "mov", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand1);
		contentsOfAReg = operand1;
    }

    emit("", "neg", "eax", "; AReg = -AReg");

    if (isTemporary(operand1)) freeTemp();

    string temp = getTemp();
    symbolTable.at(temp).setDataType(INTEGER);
    
    // DO NOT emit a mov [temp], eax here
    contentsOfAReg = temp;
    pushOperand(temp);
}
	
void Compiler::emitNotCode(string operand1, string)
{
    if (whichType(operand1) != BOOLEAN)
        processError("unary 'not' requires boolean operand");

    // --- CONSTANT FOLDING ---
    if (symbolTable.at(operand1).getMode() == CONSTANT)
    {
        string val = symbolTable.at(operand1).getValue();
        // If it's "true", we want "false", and vice versa
        string foldedValue = (val == "true") ? "false" : "true";
        
        // Push the existing constant from the symbol table onto the stack
        // instead of generating code. 
        // Note: Make sure "false"/"true" are already in your symbolTable.
        pushOperand(foldedValue); 
        return; 
    }

    // --- DEASSIGN CHECK ---
    // If AReg holds a temp we need later, save it before loading operand1
    if (isTemporary(contentsOfAReg) && contentsOfAReg != operand1)
    {
        emit("", "mov", "[" + symbolTable.at(contentsOfAReg).getInternalName() + "],eax", "; deassign AReg");
        symbolTable.at(contentsOfAReg).setAlloc(YES);
        contentsOfAReg = "";
    }

    if (contentsOfAReg != operand1)
    {
        emit("", "mov", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand1);
        contentsOfAReg = operand1;
    }

    emit("", "not", "eax", "; AReg = !AReg");

    if (isTemporary(operand1)) freeTemp();

    string temp = getTemp();
    symbolTable.at(temp).setDataType(BOOLEAN);
    contentsOfAReg = temp;
    pushOperand(temp);
}

void Compiler::emitAndCode(string operand1, string operand2)
{
    string right = operand1;
    string left = operand2;

    if (symbolTable.at(right).getDataType() != BOOLEAN || 
        symbolTable.at(left).getDataType() != BOOLEAN)
    {
        processError("binary 'and' requires boolean operands");
    }

    // 1. Manage A Register state
    if (contentsOfAReg != "" && contentsOfAReg != left)
    {
        if (isTemporary(contentsOfAReg))
        {
            emit("", "mov", "[" + symbolTable.at(contentsOfAReg).getInternalName() + "],eax", 
                 "; deassign AReg");
            // Change true to YES
            symbolTable.at(contentsOfAReg).setAlloc(YES);
        }
        contentsOfAReg = "";
    }

    // 2. Load Left operand
    if (contentsOfAReg != left)
    {
        emit("", "mov", "eax,[" + symbolTable.at(left).getInternalName() + "]", 
             "; load " + left + " in eax");
        contentsOfAReg = left;
    }

    // 3. The operation
    emit("", "and", "eax,[" + symbolTable.at(right).getInternalName() + "]", 
         "; AReg = " + left + " and " + right);

    // 4. Free involved temporaries
    if (isTemporary(right)) freeTemp();
    if (isTemporary(left)) freeTemp();

    // 5. The Mandatory Store (Deassign)
    string temp = getTemp(); 
    symbolTable.at(temp).setDataType(BOOLEAN);

    emit("", "mov", "[" + symbolTable.at(temp).getInternalName() + "],eax", "; deassign AReg");
    // Change true to YES
    symbolTable.at(temp).setAlloc(YES);

    contentsOfAReg = temp;
    pushOperand(temp);
}

void Compiler::emitOrCode(string operand1, string operand2)
{
    string right = operand1;
    string left = operand2;
    
    if (whichType(right) != BOOLEAN || whichType(left) != BOOLEAN)
        processError("binary 'or' requires boolean operands");

    // 1. Commutative Deassign Logic
    // If eax holds a temp we don't need, save it. 
    // Note: 'or' is commutative, but the reference usually loads 'left'.
    if (isTemporary(contentsOfAReg) && contentsOfAReg != left)
    {
        emit("", "mov", "[" + symbolTable.at(contentsOfAReg).getInternalName() + "],eax", "; deassign AReg");
    }

    // 2. Load Left if not already there
    if (contentsOfAReg != left)
    {
        emit("", "mov", "eax,[" + symbolTable.at(left).getInternalName() + "]", "; AReg = " + left);
    }

    // 3. The Operation
    emit("", "or", "eax,[" + symbolTable.at(right).getInternalName() + "]", 
         "; AReg = " + left + " or " + right);

    // 4. Free operands
    if (isTemporary(right)) freeTemp();
    if (isTemporary(left)) freeTemp();

    // 5. Update Tracker (Don't clear it!)
    string temp = getTemp();
    symbolTable.at(temp).setDataType(BOOLEAN);

    contentsOfAReg = temp; // Register now logically contains the result
    pushOperand(temp);
}

void Compiler::emitEqualityCode(string operand1, string operand2)
{
    string right = operand1; // operand1
    string left = operand2;  // operand2

    if (whichType(right) != whichType(left))
        processError("incompatible types for equality");

    // 1. Deassign AReg if it contains a temp that isn't either operand
    if (contentsOfAReg != "" && isTemporary(contentsOfAReg) && 
        contentsOfAReg != left && contentsOfAReg != right)
    {
        emit("", "mov", "[" + symbolTable.at(contentsOfAReg).getInternalName() + "],eax", "; deassign AReg");
        symbolTable.at(contentsOfAReg).setAlloc(YES);
        contentsOfAReg = "";
    }
    
    // 2. Deassign AReg if it holds a non-temp that isn't either operand
    if (contentsOfAReg != "" && !isTemporary(contentsOfAReg) && 
        contentsOfAReg != left && contentsOfAReg != right)
    {
        contentsOfAReg = "";
    }

    // 3. Compare Logic (Smart Load)
    if (contentsOfAReg == right) 
    {
        // If right is already in AReg, compare it to left in memory
        emit("", "cmp", "eax,[" + symbolTable.at(left).getInternalName() + "]", 
             "; compare " + right + " and " + left);
    }
    else 
    {
        // Otherwise, ensure left is in AReg (Standard behavior)
        if (contentsOfAReg != left)
        {
            emit("", "mov", "eax,[" + symbolTable.at(left).getInternalName() + "]", "; AReg = " + left);
            contentsOfAReg = left;
        }
        emit("", "cmp", "eax,[" + symbolTable.at(right).getInternalName() + "]", 
             "; compare " + left + " and " + right);
    }

    // 4. Label Generation
    string trueLabel = "." + getLabel(); // Ln
    string endLabel  = "." + getLabel(); // L(n+1)

    emit("", "je", trueLabel, "; if " + left + " = " + right + " then jump to set eax to TRUE");

    // FALSE case
    emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");
    emit("", "jmp", endLabel, "; unconditionally jump");

    // TRUE label (Ln)
    emit(trueLabel + ":", "", "", "");
    emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

    // END label (L(n+1))
    emit(endLabel + ":", "", "", "");

    // 5. Cleanup and Result
    if (isTemporary(right)) freeTemp();
    if (isTemporary(left)) freeTemp();

    string temp = getTemp();
    symbolTable.at(temp).setDataType(BOOLEAN);
    
    contentsOfAReg = temp;
    pushOperand(temp);
}

void Compiler::emitInequalityCode(string operand1, string operand2)
{
    string right = operand1; // popped first
    string left = operand2;  // popped second

    if (whichType(left) != whichType(right))
        processError("incompatible types for operator '<>'");

    // 1. Load left operand if not in AReg
	if (contentsOfAReg != left)
	{
		emit("", "mov", "eax,[" + symbolTable.at(left).getInternalName() + "]", "; AReg = " + left);
		contentsOfAReg = left;
	}

    // 2. Comparison
    emit("", "cmp", "eax,[" + symbolTable.at(right).getInternalName() + "]", 
         "; compare " + left + " and " + right);

    // 3. Jump Logic (jne for <>)
    string labelFalse = "." + getLabel();
    string labelExit = "." + getLabel();

    emit("", "jne", labelFalse, "; if " + left + " <> " + right + " then jump to set eax to TRUE");
    emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");
    emit("", "jmp", labelExit, "; unconditionally jump");
    emit(labelFalse + ":", "", "", "");
    emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");
    emit(labelExit + ":", "", "", "");

    // 4. Housekeeping
    if (isTemporary(left)) freeTemp();
    if (isTemporary(right)) freeTemp();

    // 5. Just track it in the register.
    string temp = getTemp();
    symbolTable.at(temp).setDataType(BOOLEAN);
    
    contentsOfAReg = temp;
    pushOperand(temp);
}

void Compiler::emitLessThanCode(string operand1, string operand2)
{
	string right = operand1;
	string left = operand2;
	
    if (whichType(right) != INTEGER ||
        whichType(left) != INTEGER)
    {
        processError("incompatible types for less than");
    }

    // AReg handling
    if (contentsOfAReg != "" &&
        isTemporary(contentsOfAReg) &&
        contentsOfAReg != right &&
        contentsOfAReg != left)
    {
        emit("", "mov",
             "[" + symbolTable.at(contentsOfAReg).getInternalName() + "],eax",
             "; deassign AReg");

        symbolTable.at(contentsOfAReg).setAlloc(YES);
        contentsOfAReg = "";
    }
    else if (contentsOfAReg != "" &&
             !isTemporary(contentsOfAReg) &&
             contentsOfAReg != right &&
             contentsOfAReg != left)
    {
        contentsOfAReg = "";
    }

    // load LEFT into eax
	if (contentsOfAReg != left)
	{
		emit("", "mov", "eax,[" + symbolTable.at(left).getInternalName() + "]", "; AReg = " + left);
		contentsOfAReg = left;
	}

    // compare LEFT vs RIGHT
    emit("", "cmp",
         "eax,[" + symbolTable.at(right).getInternalName() + "]",
         "; compare " + left + " and " + right);

    string trueLabel = getLabel();
    string endLabel  = getLabel();

    trueLabel = "." + trueLabel;
    endLabel  = "." + endLabel;

    emit("", "jl", trueLabel, "; if " + left + " < " + right + " then jump to set eax to TRUE");

    // FALSE
    emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");
    emit("", "jmp", endLabel, "; unconditionally jump");

    // TRUE
    emit(trueLabel + ":", "", "", "");
    emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

    emit(endLabel + ":", "", "", "");

	if (isTemporary(right)) freeTemp();
	if (isTemporary(left)) freeTemp();

    string temp = getTemp();
    symbolTable.at(temp).setDataType(BOOLEAN);

    contentsOfAReg = temp;
    pushOperand(temp);
}

void Compiler::emitLessThanOrEqualToCode(string operand1, string operand2)
{
    string right = operand1;
    string left = operand2;
    
    if (whichType(right) != whichType(left))
        processError("incompatible types for operator '<='");

    // 1. Deassign AReg if it holds an unrelated temporary
    if (isTemporary(contentsOfAReg) && contentsOfAReg != left && contentsOfAReg != right)
    {
        emit("", "mov", "[" + symbolTable.at(contentsOfAReg).getInternalName() + "],eax", "; deassign AReg");
        contentsOfAReg = "";
    }

    // 2. Load LEFT operand (Actually emit the mov!)
    if (contentsOfAReg != left)
    {
        emit("", "mov", "eax,[" + symbolTable.at(left).getInternalName() + "]", "; AReg = " + left);
        contentsOfAReg = left;
    }

    // 3. Comparison
    emit("", "cmp", "eax,[" + symbolTable.at(right).getInternalName() + "]", "; compare " + left + " and " + right);

    string trueLabel = "." + getLabel();
    string endLabel  = "." + getLabel();

    emit("", "jle", trueLabel, "; if " + left + " <= " + right + " then jump to set eax to TRUE");
    emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");
    emit("", "jmp", endLabel, "; unconditionally jump");
    emit(trueLabel + ":", "", "", "");
    emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");
    emit(endLabel + ":", "", "", "");

    if (isTemporary(right)) freeTemp();
    if (isTemporary(left)) freeTemp();

    // 4. Update tracking but DO NOT store to memory
    string temp = getTemp();
    symbolTable.at(temp).setDataType(BOOLEAN);

    contentsOfAReg = temp;
    pushOperand(temp);
}

void Compiler::emitGreaterThanCode(string operand1, string operand2) {
    string right = operand1;
    string left = operand2;
    
    // Fix: Correct the error message and type check (expecting INTEGER for >)
    if (whichType(right) != INTEGER || whichType(left) != INTEGER)
        processError("operator '>' requires integer operands");

    // 1. If AReg has something else, store it
    if (contentsOfAReg != "" && contentsOfAReg != left) {
        // ... emit mov to store current AReg ...
        contentsOfAReg = "";
    }

    // 2. LOAD LEFT (Actually emit the instruction!)
    if (contentsOfAReg != left) {
        emit("", "mov", "eax,[" + symbolTable.at(left).getInternalName() + "]", "; load " + left + " in eax");
        contentsOfAReg = left;
    }

    // 3. Compare
    emit("", "cmp", "eax,[" + symbolTable.at(right).getInternalName() + "]", "; compare " + left + " and " + right);

    string trueLabel = "." + getLabel();
    string endLabel  = "." + getLabel();

    emit("", "jg", trueLabel, "; if " + left + " > " + right + " then jump to set eax to TRUE");
    emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");
    emit("", "jmp", endLabel, "; unconditionally jump");
    emit(trueLabel + ":", "", "", "");
	
    emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");
    emit(endLabel + ":", "", "", "");
	
    // 4. Update TRUE/FALSE in Symbol Table
    // Ensure "TRUE" and "FALSE" are added to symbolTable so generateData() finds them
    insert("FALSE", BOOLEAN, CONSTANT, "0", YES, 1);
	insert("TRUE", BOOLEAN, CONSTANT, "-1", YES, 1);

    // 5. Cleanup
    if (isTemporary(right)) freeTemp();
    if (isTemporary(left)) freeTemp();

    string temp = getTemp();
    symbolTable.at(temp).setDataType(BOOLEAN);
    
    contentsOfAReg = temp; 
    pushOperand(temp);
}

void Compiler::emitGreaterThanOrEqualToCode(string operand1, string operand2)
{
    string right = operand1;
    string left = operand2;
    
    if (whichType(right) != whichType(left))
        processError("incompatible types");

    // 1. Optimized Load: Only load if it's not already there
    if (contentsOfAReg != left) {
        emit("", "mov", "eax,[" + symbolTable.at(left).getInternalName() + "]", "; AReg = " + left);
        contentsOfAReg = left;
    }
    
    // 2. Ensure TRUE/FALSE exist in symbol table for SECTION .data
    // Use the specific internal/external naming your instructor's diff expects
    if (symbolTable.find("FALSE") == symbolTable.end())
        insert("FALSE", BOOLEAN, CONSTANT, "0", YES, 1); 
    if (symbolTable.find("TRUE") == symbolTable.end())
        insert("TRUE", BOOLEAN, CONSTANT, "-1", YES, 1);

    emit("", "cmp", "eax,[" + symbolTable.at(right).getInternalName() + "]", 
         "; compare " + left + " and " + right);

    string trueLabel = "." + getLabel();
    string endLabel  = "." + getLabel();

    emit("", "jge", trueLabel, "; if " + left + " >= " + right + " then jump");
    emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");
    emit("", "jmp", endLabel, "; unconditionally jump");

    emit(trueLabel + ":", "", "", "");
    emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");
    emit(endLabel + ":", "", "", "");
    
    // 3. Temporary Management
    if (isTemporary(right)) freeTemp();
    if (isTemporary(left)) freeTemp();

    string temp = getTemp(); // Ensure this starts as Allocated = NO
    symbolTable.at(temp).setDataType(BOOLEAN);

    contentsOfAReg = temp;
    pushOperand(temp);
}

//////////////////// LEXICON ////////////////////

char Compiler::nextChar()			// gets raw chars
{
    char next;

    if (!sourceFile.get(next)) {
        ch = END_OF_FILE;
        return ch;   // removed extra newline
    }

    ch = next;

    if (listingHeaderCreated && begChar) {
        listingFile << std::right << std::setw(5) << lineNo << "|";
        begChar = false;
    }

    if (listingHeaderCreated)
        listingFile << ch;

	// int errLine = (ch == '\n') ? lineNo - 1 : lineNo;
    if (ch == '\n') {
        lineNo++;
        begChar = true;
    }

    return ch;
}
	
string Compiler::nextToken()			// builds tokens out of raw chars
{
    token = "";

    while (token == "")
    {
        // skip whitespace
        if (isspace(ch))
        {
            nextChar();
            continue;
        }

        // comments
        if (ch == '{')
        {
            do {
                nextChar();
            } while (ch != '}' && ch != END_OF_FILE);

            if (ch == END_OF_FILE)
                processError("unexpected end of file");

            nextChar();
            continue;
        }

        if (ch == '}')
        {
            processError("'}' cannot begin token");
            nextChar();
            continue;
        }

        // EOF
        if (ch == END_OF_FILE)
        {
            token = "EOF";
            return token;
        }

        // =========================
        // multi-character operators
        // =========================

        // :=
        if (ch == ':')
        {
            nextChar();

            if (ch == '=')
            {
                token = ":=";
                nextChar();
            }
            else
            {
                token = ":";
            }
            return token;
        }

        // <=, <>, <
        if (ch == '<')
        {
            nextChar();

            if (ch == '=')
            {
                token = "<=";
                nextChar();
            }
            else if (ch == '>')
            {
                token = "<>";
                nextChar();
            }
            else
            {
                token = "<";
            }
            return token;
        }

        // >=, >
        if (ch == '>')
        {
            nextChar();

            if (ch == '=')
            {
                token = ">=";
                nextChar();
            }
            else
            {
                token = ">";
            }
            return token;
        }

        // =========================
        // identifiers
        // =========================
        if (islower(ch))
        {
            token = ch;

            nextChar();
            while (isalnum(ch) || ch == '_')
            {
                token += ch;
                nextChar();
            }

            return token;
        }

        // integers
        if (isdigit(ch))
        {
            token = ch;

            nextChar();
            while (isdigit(ch))
            {
                token += ch;
                nextChar();
            }

            return token;
        }

        // single-character symbols
        if (isSpecialSymbol(ch))
        {
            token = ch;
            nextChar();
            return token;
        }

        // fallback error
        processError("illegal symbol");
        nextChar();
    }

    return token;
}
  
//////////////////// ERROR ////////////////////

string Compiler::genInternalName(storeTypes stype) const
{
    if (stype == INTEGER)
    {
        return "I" + to_string(I_count++);
    }
    else if (stype == BOOLEAN)
    {
        return "B" + to_string(B_count++);
    }
    else
    {
        return "X";
    }
}

void Compiler::processError(string err)
{
    errorCount++;

    int errLine = lineNo;

    // if we are past newline, we are already on NEXT line
    if (begChar == true)
        errLine--;   // adjust back to actual source line

    listingFile << endl
                << "Error: "
                << "Line " << errLine << ": "
                << err << endl << endl;

    createListingTrailer();

    listingFile.flush();
    objectFile.flush();

    exit(EXIT_FAILURE);
}

//////////////////// TEMP HANDLING ////////////////////

void Compiler::freeTemp() {
    if (currentTempNo > -1) currentTempNo--;
    else processError("compiler error: currentTempNo underflow");
}

string Compiler::getTemp()
{
    currentTempNo++;
    string temp = "T" + to_string(currentTempNo);

    if (currentTempNo > maxTempNo)
    {
        // insert() now checks isTemporary(temp) and uses "T#" as internal name
        insert(temp, UNKNOWN, VARIABLE, temp, YES, 1);
        maxTempNo = currentTempNo;
    }
    return temp;
}

string Compiler::getLabel()
{
    static int labelNo = 0;			// resets label to 0 every time

    return "L" + to_string(labelNo++);
}

bool Compiler::isTemporary(string s) const	// determines if s represents a temporary
{
    return !s.empty() && s[0] == 'T';
}

// ------------------------------------------------------------- //
// PRIVATE variables declared in stage0.h //
/*
map<string, SymbolTableEntry> symbolTable;
ifstream sourceFile;
ofstream listingFile;
ofstream objectFile;
string token;          // the next token
char ch;               // the next character of the source file
uint errorCount = 0;   // total number of errors encountered
uint lineNo = 0;       // line numbers for the listing

stack<string> operatorStk; // operator stack
stack<string> operandStk; // operand stack
int currentTempNo = -1; // current temp number
int maxTempNo = -1; // max temp number
string contentsOfAReg; // symbolic contents of A register
*/
// ------------------------------------------------------------- //

// To test the program:
/*
make stage1

DATA FILES:
ls /usr/local/4301/data/stage1/

cp /usr/local/4301/data/stage1/101.dat .
cat 101.dat

./stage1 101.dat 101.lst 101.asm

cat 101.lst
cat 101.asm

EDIT MAKEFILE:
targetsAsmLanguage = 101
make 101
./101

diff /usr/local/4301/data/stage1/101.lst 101.lst
diff /usr/local/4301/data/stage1/101.asm 101.asm
*/