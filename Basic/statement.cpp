/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include <string>
#include "statement.h"
#include "parser.h"
#include "../StanfordCPPLib/error.h"
#include "../StanfordCPPLib/tokenscanner.h"

using namespace std;
const int illegalNameCnt = 13;
const string illegalNames[illegalNameCnt] = {"REM", "LET", "PRINT", "INPUT", "END",
	"GOTO", "IF", "THEN", "RUN", "LIST", "CLEAR", "QUIT", "HELP"};

/* Implementation of the Statement class */

Statement::Statement() {
   /* Empty */
}

Statement::~Statement() {
   /* Empty */
}

void REMstate::execute(EvalState & state) {
	/* Empty */
}

void REMstate::parseState(TokenScanner & scanner, string & line) {
	whole = line;
}


void LETstate::execute(EvalState & state) {
	state.setValue(varName, exp->eval(state));
}

void LETstate::parseState(TokenScanner & scanner, string & line) {
	varName = scanner.nextToken();
	TokenType type = scanner.getTokenType(varName);
	string opt = scanner.nextToken();
	int match;
	for (match = 0; match < illegalNameCnt; match++)
		if (illegalNames[match] == varName) break;
	if (type != WORD || match != illegalNameCnt || opt != "=")
		error("SYNTAX ERROR");
	try {
		exp = parseExp(scanner);
	}
	catch (ErrorException &ex) {
		cout << "SYNTAX ERROR" << endl;
	}
	whole = line;
}

void PRINTstate::execute(EvalState & state) {
	try {
		int value = exp->eval(state);
		cout << value << endl;
	}
	catch(ErrorException &ex){
		cout << ex.getMessage() << endl;
	}
}

void PRINTstate::parseState(TokenScanner & scanner, string & line) {
	try {
		exp = parseExp(scanner);
	}
	catch (ErrorException &ex) {
		cout << "SYNTAX ERROR" << endl;
	}
	whole = line;
}

void INPUTstate::execute(EvalState & state) {
	while (1) {
		cout << " ? ";
		string str;
		getline(cin, str);
		try {
			int value = stringToInteger(str);
			state.setValue(varName, value);
			return;
		}
		catch (ErrorException &ex) {
			cout << "INVALID NUMBER" << endl;
		}
	}
}

void INPUTstate::parseState(TokenScanner & scanner, string & line) {
	varName = scanner.nextToken();
	if (scanner.hasMoreTokens()) error("SYNTAX ERROR");
	whole = line;
}

void GOTOstate::execute(EvalState & state) {
	state.setValue("RUN", lineNumber);
	state.setValue("GOTO", 1);
}

void GOTOstate::parseState(TokenScanner & scanner, string & line) {
	string num = scanner.nextToken();
	if (scanner.getTokenType(num) != NUMBER || scanner.hasMoreTokens())
		error("SYNTAX ERROR");
	try {
		lineNumber = stringToInteger(num);
	}
	catch (ErrorException) {
		cout << "INVALID NUMBER" << endl;
	}
	whole = line;
}

void IFstate::execute(EvalState & state) {
	if (cmp == "<" && lhs->eval(state) < rhs->eval(state)
		|| cmp == "=" && lhs->eval(state) == rhs->eval(state)
		|| cmp == ">" && lhs->eval(state) > rhs->eval(state)) {
		state.setValue("RUN", lineNumber);
		state.setValue("GOTO", 1);
	}
}

void IFstate::parseState(TokenScanner & scanner, string & line) {
	TokenScanner tmpScanner;
	tmpScanner.ignoreWhitespace();
	tmpScanner.scanNumbers();
	string tmp;
	int pos, _pos;

	_pos = line.find("THEN");
	if (_pos == std::string::npos) error("SYNTAX ERROR");

	if (line.find("<") != std::string::npos) pos = line.find("<");
	else if (line.find("=") != std::string::npos) pos = line.find("=");
	else if (line.find(">") != std::string::npos) pos = line.find(">");

	try {
		tmp = line.substr(line.find("IF") + 2, pos - line.find("IF") - 2);
		tmpScanner.setInput(tmp);
		lhs = parseExp(tmpScanner);

		cmp = line.substr(pos, 1);
		if (cmp != "<" && cmp != "=" && cmp != ">") error("SYNTAX ERROR");

		tmp = line.substr(pos + 1, _pos - pos - 1);
		tmpScanner.setInput(tmp);
		rhs = parseExp(tmpScanner);

		tmp = line.substr(_pos);
		tmpScanner.setInput(tmp);
		tmp = tmpScanner.nextToken();
		if (tmp != "THEN") error("SYNTAX ERROR");

		string num = tmpScanner.nextToken();
		if (tmpScanner.getTokenType(num) != NUMBER || tmpScanner.hasMoreTokens())
			error("SYNTAX ERROR");

		lineNumber = stringToInteger(num);
	}
	catch (ErrorException &ex) {
		cout << "SYNTAX ERROR" << endl;
	}

	whole = line;
}

void ENDstate::execute(EvalState & state) {
	state.setValue("END", 1);
}

void ENDstate::parseState(TokenScanner & scanner, string & line) {
	if (scanner.hasMoreTokens()) error("SYNTAX ERROR");
	whole = line;
}
