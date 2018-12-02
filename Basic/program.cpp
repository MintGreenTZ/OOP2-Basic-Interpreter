/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include <string>
#include <vector>
#include <map>
#include "program.h"
#include "parser.h"
#include "statement.h"
#include "../StanfordCPPLib/tokenscanner.h"
#include "../StanfordCPPLib/error.h"
using namespace std;

Program::Program() {
}

Program::~Program() {
}

void Program::clear(EvalState & state) {
	for (map<int, Statement *>::iterator it = code.begin(); it != code.end(); it++)
		delete (*it).second;
	code.clear();
	state.clear();
}

void Program::addSourceLine(int lineNumber, TokenScanner & scanner,  string & line) {
	Statement *stmt = nullptr;
	string token = scanner.nextToken();
	try {
		if (token == "REM") stmt = new REMstate;
		else if (token == "LET") stmt = new LETstate;
		else if (token == "PRINT") stmt = new PRINTstate;
		else if (token == "INPUT") stmt = new INPUTstate;
		else if (token == "GOTO") stmt = new GOTOstate;
		else if (token == "IF") stmt = new IFstate;
		else if (token == "END") stmt = new ENDstate;
		else { cout << "SYNTAX ERROR" << endl; return; }
		stmt->parseState(scanner, line);
		code[lineNumber] = stmt;
	}
	catch (ErrorException &ex) {
		if (stmt) delete stmt;
		cout << ex.getMessage() << endl;
	}
}

void Program::removeSourceLine(int lineNumber) {
	code.erase(lineNumber);
}

string Program::getSourceLine(int lineNumber) {
	if (code.find(lineNumber) == code.end()) return "";
	return code[lineNumber]->whole;
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
   /* Empty */
}

Statement *Program::getParsedStatement(int lineNumber) {
   return NULL;
}

int Program::getFirstLineNumber() {
   return code.begin() -> first;
}

int Program::getNextLineNumber(int lineNumber) {
	if (code.upper_bound(lineNumber) == code.end()) return -1;
	return code.upper_bound(lineNumber) -> first;
}

void Program::list() {
	for (map<int, Statement *>::iterator it = code.begin(); it != code.end(); it++)
		cout << (it->second)->whole << endl;
}

// RUN--curLineNumber; END--bool about whether end; GOTO--bool about whether goto
void Program::run(EvalState & state) {
	state.setValue("RUN", getFirstLineNumber());
	state.setValue("END", 0);
	state.setValue("GOTO", 0);
	while (!state.getValue("END") && state.getValue("RUN") != -1) {
		try {
			if (code.find(state.getValue("RUN")) == code.end())
				error("LINE NUMBER ERROR");
			code[state.getValue("RUN")]->execute(state);
			if (state.getValue("GOTO"))
				state.setValue("GOTO", 0);
			else
				state.setValue("RUN", getNextLineNumber(state.getValue("RUN")));
		}
		catch (ErrorException &ex) {
			cout << ex.getMessage() << endl;
			break;
		}
	}
}
