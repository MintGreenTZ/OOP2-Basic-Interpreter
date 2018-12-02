/*
 * File: Basic.cpp
 * ---------------
 * Name: Tang Ze
 * Section: Kind assistants
 * This file is the starter project for the BASIC interpreter from
 * Assignment #6.
 * [TODO: extend and correct the documentation]
 */

#include <cctype>
#include <iostream>
#include <string>
#include "exp.h"
#include "parser.h"
#include "program.h"
#include "../StanfordCPPLib/error.h"
#include "../StanfordCPPLib/tokenscanner.h"

#include "../StanfordCPPLib/simpio.h"
#include "../StanfordCPPLib/strlib.h"
using namespace std;

const int optNum = 5;
const string opt[optNum] = {"RUN", "LIST", "CLEAR", "QUIT", "HELP"};

/* Function prototypes */

void processLine(string line, Program & program, EvalState & state);

/* Main program */

int main() {
   EvalState state;
   Program program;
   while (true) {
      try {
         processLine(getLine(), program, state);
      } catch (ErrorException & ex) {
         cerr << "Error: " << ex.getMessage() << endl;
      }
   }
   return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version,
 * the implementation does exactly what the interpreter program
 * does in Chapter 19: read a line, parse it as an expression,
 * and then print the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(string line, Program & program, EvalState & state) {
   TokenScanner scanner;
   scanner.ignoreWhitespace();
   scanner.scanNumbers();
   scanner.setInput(line); 
   while (scanner.hasMoreTokens()) {
	   string token = scanner.nextToken();
	   if (scanner.getTokenType(token) == NUMBER) {
		   if (scanner.hasMoreTokens())
			   program.addSourceLine(stringToInteger(token), scanner, line);
		   else
			   program.removeSourceLine(stringToInteger(token));
		   return;
	   }
	   int match;
	   for (match = 0; match < optNum; match++)
		   if (token == opt[match]) break;
	   if (match == optNum) {
		   Statement *stmt = nullptr;
		   if (token == "LET") stmt = new LETstate;
		   else if (token == "PRINT") stmt = new PRINTstate;
		   else if (token == "INPUT") stmt = new INPUTstate;
		   else { cout << "SYNTAX ERROR" << endl; return; }
		   try {
				stmt->parseState(scanner, line);
				stmt->execute(state);
		   }
		   catch (ErrorException &ex) {
			   cout << ex.getMessage() << endl;
		   }
		   return;
	   }
	   if (scanner.hasMoreTokens()) {
		   cout << "SYNTAX ERROR" << endl;
		   return;
	   }
	   try {
		   switch (match) {
			   case 0:program.run(state); break;
			   case 1:program.list(); break;
			   case 2:program.clear(state); break;
			   case 3:exit(0); break;
			   case 4:cout << "The editor needs help too. qwq" << endl; break;
			   default:break;
		   }
	   }
	   catch (ErrorException &ex) {
		   cout << ex.getMessage() << endl;
	   }
   };
}
