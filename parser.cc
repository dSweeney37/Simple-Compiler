/*
 * Copyright (C) Rida Bazzi, 2020
 *
 * Do not share this file with anyone
 *
 * Do not post this file or derivatives of
 * of this file online
 *
 */
#include <iostream>
#include <cstdlib>
#include "parser.h"

using namespace std;




int main() {
	Parser input;


	input.ParseInput();
}



void Parser::AsignVarToVal(vector<Monomial>& pPolyParameters, vector<Input>& pInputTable) {
	for (int i = 0; i < pInputTable.size(); ++i) {
		if (pPolyParameters.size() == 0) { pInputTable[i].name = "x"; }
		else { pInputTable[i].name = pPolyParameters[i].name; }
	}
}



void Parser::Code1Errors() {
	string errorStr;


	for (int i = 0; i < polyMatrix[declarations].size(); ++i) {
		for (int j = i + 1; j < polyMatrix[declarations].size(); ++j) {
			if (polyMatrix[declarations][i].name == polyMatrix[declarations][j].name) {
				polyMatrix[declarations][i].isDupe = true;
				polyMatrix[declarations][j].isDupe = true;
			}
		}
		if (polyMatrix[declarations][i].isDupe == true) {
			errorStr = errorStr + " " + std::to_string(polyMatrix[declarations][i].lineNum);
		}
	}

	if (errorStr != "") {
		PrintError(1, errorStr);
		exit(1);
	}
}



void Parser::Code2Errors() {
	string errorStr;


	for (int i = 0; i < polyMatrix[declarations].size(); ++i) {
		for (int j = 0; j < polyMatrix[declarations][i].body.size(); ++j) {
			bool found = false;


			if (polyMatrix[declarations][i].body[j].tokenType == NUM) { found = true; }

			for (int k = 0; k < polyMatrix[declarations][i].parameters.size(); ++k) {
				if (polyMatrix[declarations][i].body[j].name == polyMatrix[declarations][i].parameters[k].name) {
					found = true;
				}
			}

			if (found == false) {
				if (polyMatrix[declarations][i].body[j].name == "x") {
					if (polyMatrix[declarations][i].parameters.size() != 0) {
						errorStr = errorStr + " " + std::to_string(polyMatrix[declarations][i].body[j].lineNum);
					}
				}
				else { errorStr = errorStr + " " + std::to_string(polyMatrix[declarations][i].body[j].lineNum); }
			}
		}
	}

	if (errorStr != "") {
		PrintError(2, errorStr);
		exit(1);
	}
}



void Parser::Code3Errors() {
	if (error3Str != "") {
		PrintError(3, error3Str);
		exit(1);
	}
}



void Parser::Code4Errors() {
	string errorStr;


	for (int i = 0; i < polyMatrix[evaluations].size(); ++i) {
		for (int j = 0; j < polyMatrix[declarations].size(); ++j) {
			if (polyMatrix[evaluations][i].name == polyMatrix[declarations][j].name) {
				if (polyMatrix[declarations][j].parameters.size() == 0) {
					if (polyMatrix[evaluations][i].parameters.size() != 1) {
						errorStr = errorStr + " " + std::to_string(polyMatrix[evaluations][i].lineNum);
					}
				}
				else if (polyMatrix[evaluations][i].parameters.size() != polyMatrix[declarations][j].parameters.size()) {
					errorStr = errorStr + " " + std::to_string(polyMatrix[evaluations][i].lineNum);
				}
			}
		}
	}

	if (errorStr != "") {
		PrintError(4, errorStr);
		exit(1);
	}
}



void Parser::Code5Errors() {
	bool repeat;
	const int MAX_VAL_ROW = 1, NUM_OF_COL = 2, CURRENT_POS_ROW = 0;
	int min;
	int* paramValues[NUM_OF_COL];
	string errorStr;


	// Creates "numOfCol" columns.
	for (int i = 0; i < NUM_OF_COL; ++i) { paramValues[i] = new int[polyMatrix[evaluations].size()]; }

	// Adds the max number of rows per column in the "polyMatrix" to "paramValues"
	// as well as the starting position "pos" = 0.
	for (int i = 0; i < polyMatrix[evaluations].size(); ++i) {
		paramValues[CURRENT_POS_ROW][i] = 0;
		paramValues[MAX_VAL_ROW][i] = polyMatrix[evaluations][i].parameters.size();
	}

	do {
		repeat = false;
		min = -1;


		for (int i = 0; i < polyMatrix[evaluations].size(); ++i) {
			if (repeat == false && paramValues[CURRENT_POS_ROW][i] < paramValues[MAX_VAL_ROW][i]) {
				min = i;
				repeat = true;
			}

			else if (repeat == true && paramValues[CURRENT_POS_ROW][i] < paramValues[MAX_VAL_ROW][i]) {
				if (polyMatrix[evaluations][i].parameters[paramValues[CURRENT_POS_ROW][i]].lineNum <
					polyMatrix[evaluations][min].parameters[paramValues[CURRENT_POS_ROW][min]].lineNum) {
					min = i;
				}
			}
		}

		if (repeat == true && polyMatrix[evaluations][min].parameters[paramValues[CURRENT_POS_ROW][min]].isInitialized == false) {
			errorStr = errorStr + " " + std::to_string(polyMatrix[evaluations][min].parameters[paramValues[CURRENT_POS_ROW][min]].lineNum);
		}
		++paramValues[CURRENT_POS_ROW][min];

	} while (repeat == true);

	if (errorStr != "") {
		PrintError(5, errorStr);
		exit(1);
	}
}



// this function gets a token and checks if it is
// of the expected type. If it is, the token is
// returned, otherwise, synatx_error() is generated
// this function is particularly useful to match
// terminals in a right hand side of a rule.
// Written by Mohsen Zohrevandi
Token Parser::expect(TokenType expected_type) {
	Token t = lexer.GetToken();
	if (t.token_type != expected_type)
		syntax_error();
	return t;
}



// Looks through the polynomial vector for a match to the passed-in
// argument. If a match is not located within the vector then, the  
// argument is appended to the "error3Str". 
void Parser::FindPolyMatch(int pLineNum, string pLexeme) {
	bool found = false;


	for (int i = 0; i < polyMatrix[declarations].size(); ++i) {
		if (polyMatrix[declarations][i].name == pLexeme) { found = true; }
	}

	if (found == false) { error3Str = error3Str + " " + to_string(pLineNum); }
}



double Parser::GetOperand(vector<Input>& pInputTable, Monomial* pMonomial) {
	double value = 0;


	if (pMonomial->tokenType == NUM) { value = std::stoi(pMonomial->name); }
	else {
		for (int i = 0; i < pInputTable.size(); ++i) {
			if (pInputTable[i].name == pMonomial->name) { value = pInputTable[i].value; }
		}
	}

	return value;
}



void Parser::GetParameterValue(vector<Input>& pInputTable, Monomial* pParameter) {
	bool found = false;


	if (pParameter->tokenType == NUM) {
		pInputTable.push_back(Input("", std::stoi(pParameter->name)));
	}

	else {
		for (int i = 0; i < inputMatrix[variables].size(); ++i) {
			if (inputMatrix[variables][i].name == pParameter->name) {
				int pos = inputMatrix[variables][i].intputRefVector[pParameter->inputIndex];
				int value = inputMatrix[inputs][pos].value;


				pInputTable.push_back(Input(pParameter->name, value));
				found = true;
			}
		}

		// Creates a placeholder input and appends it to the "inputTable".
		if (found == false) {
			++evalPos;
			pInputTable.push_back(Input("", ProcessEvaluation()));
		}
	}
}



void Parser::ParseInput() {
	// --> program & inputs
	int n = 2;
	Token t;


	// Makes the "polyMatrix" 2 dimensional with "n" number of columns.
	inputMatrix.resize(n);
	polyMatrix.resize(n);

	ParseProgram();
	ParseInputs();
	t = expect(END_OF_FILE);

	//p1();     // PRINTS OUT ALL OF THE POLYNOMIALS, THEIR PARAMETERS, AND THEIR BODIES IN THE POLYNOMIAL MATRIX.
	//p2();     // PRINTS OUT ALL EVALUATIONS AND THEIR PARAMETERS/INPUT_REF.
	//p3();     // PRINTS OUT ALL OF THE INPUTS AND ALL OF THE VARIABLES IN THE VARIABLE MATRIX.

	Code1Errors();
	Code2Errors();
	Code3Errors();
	Code4Errors();
	Code5Errors();
	ProcessPolynomials();
}



void Parser::ParseProgram() {
	// --> poly_decl_section & start


	ParsePolyDeclSec();
	ParseStart();
}



void Parser::ParsePolyDeclSec() {
	// --> poly_decl || poly_decl & poly_decl_section
	Token p;


	ParsePolyDecl();
	p = lexer.peek(1);
	if (p.token_type == POLY) { ParsePolyDeclSec(); }
}



void Parser::ParsePolyDecl() {
	// --> POLY & polynomial_header & EQUAL & polynomial_body & SEMICOLON
	Token t;


	// Verifies and consumes "POLY".
	t = expect(POLY);
	ParsePolyHeader();
	// Verifies and consumes "EQUAL".
	t = expect(EQUAL);
	ParsePolyBody();
	// Verifies and consumes "SEMICOLON".
	t = expect(SEMICOLON);
}



void Parser::ParsePolyHeader() {
	// --> polynomial_name || polynomial_name & LPAREN & id_list & RPAREN
	Token t, p;


	p = lexer.peek(1);
	// Appends a new polynomial with the corresponding token info
	// into the declaration column of the polynomial vector.
	polyMatrix[declarations].push_back(Polynomial(p.line_no, p.lexeme));

	ParsePolyName();

	t = lexer.GetToken();
	// Checks whether or not to continue parsing this block.
	if (t.token_type == LPAREN) {
		ParseIdList();
		// Verifies and consumes "RPAREN".
		t = expect(RPAREN);
	}
	// Returns a "Token" if parsing was discontinued.
	else { lexer.UngetToken(1); }
}



void Parser::ParseIdList() {
	// --> ID || ID & COMMA & id_list
	Token t;


	// Verifies and consumes "ID".
	t = expect(ID);

	// Adds a new monomial to the most recently added declaration polynomial"s "monomials" vector.
	polyMatrix[declarations][polyMatrix[declarations].size() - 1].parameters.push_back(Monomial(t.line_no, t.lexeme));

	t = lexer.GetToken();
	// Checks whether or not to continue parsing this block.
	if (t.token_type == COMMA) { ParseIdList(); }
	// Returns a "Token" if parsing was discontinued.
	else { lexer.UngetToken(1); }
}



void Parser::ParsePolyName() {
	// --> ID
	Token t;


	// Verifies and consumes "ID".
	t = expect(ID);
}



void Parser::ParsePolyBody() {
	// --> term_list


	ParseTermList();
}



void Parser::ParseTermList() {
	// --> term || term & add_operator & term_list
	string sign;
	Token p;


	ParseTerm();

	p = lexer.peek(1);
	// Checks whether or not to continue parsing this block.
	if (p.token_type == PLUS || p.token_type == MINUS) {
		int i = polyMatrix[declarations].size() - 1, j = polyMatrix[declarations][i].body.size() - 1;


		sign = ParseAddOp();
		polyMatrix[declarations][i].body[j].sign = sign;
		ParseTermList();
	}
}



void Parser::ParseTerm() {
	// --> monomial_list || coefficient || coefficient & monomial_list
	int i = polyMatrix[declarations].size() - 1;
	Token p;


	p = lexer.peek(1);
	if (p.token_type == ID) { ParseMonoList(1); }
	else if (p.token_type == NUM) {
		Monomial temp = ParseCoef();
		temp.tokenType = NUM;

		p = lexer.peek(1);
		if (p.token_type == ID) { ParseMonoList(stoi(temp.name)); }
		else { polyMatrix[declarations][i].body.push_back(temp); }
	}
	else { syntax_error(); }
}



void Parser::ParseMonoList(int pCoef) {
	// --> monomial || monomial & monomial_list
	Token p;


	ParseMono(pCoef);

	p = lexer.peek(1);
	if (p.token_type == ID) {
		int i = polyMatrix[declarations].size() - 1, j = polyMatrix[declarations][i].body.size() - 1;

		polyMatrix[declarations][i].body[j].sign = "*";
		ParseMonoList(1);
	}
}



void Parser::ParseMono(int pCoef) {
	// --> ID || ID & exponent
	int i = polyMatrix[declarations].size() - 1, j;
	Token t, p;


	// Verifies and consumes "ID".
	t = expect(ID);

	// Adds a new monomial to the most recently added declaration polynomial"s "monomials" vector.
	polyMatrix[declarations][i].body.push_back(Monomial(t.line_no, t.lexeme));

	j = polyMatrix[declarations][i].body.size() - 1;
	polyMatrix[declarations][i].body[j].coefficient = pCoef;

	p = lexer.peek(1);
	if (p.token_type == POWER) { ParseExp(); }
}



void Parser::ParseExp() {
	// --> POWER & NUM
	int i = polyMatrix[declarations].size() - 1, j = polyMatrix[declarations][i].body.size() - 1;
	Token t;


	// Verifies and consumes "POWER".
	t = expect(POWER);
	// Verifies and consumes "NUM".
	t = expect(NUM);
	polyMatrix[declarations][i].body[j].exponent = stoi(t.lexeme);
}



string Parser::ParseAddOp() {
	// --> PLUS || MINUS
	Token t;


	t = lexer.GetToken();
	// Verifies and consumes "PLUS" or "MINUS", otherwise throws a syntax error.
	if (t.token_type != PLUS && t.token_type != MINUS) { syntax_error(); }

	if (t.token_type == PLUS) { return "+"; }
	else { return "-"; }
}



Monomial Parser::ParseCoef() {
	// --> NUM
	Token t;


	// Verifies and consumes "NUM".
	t = expect(NUM);

	return Monomial(t.line_no, t.lexeme);

}



void Parser::ParseStart() {
	// --> START & statement_list
	Token t;


	// Verifies and consumes "START".
	t = expect(START);
	ParseStatementList();
}



void Parser::ParseInputs() {
	// --> NUM || NUM & inputs
	Token t, p;


	// Verifies and consumes "NUM".
	t = expect(NUM);

	inputMatrix[inputs].push_back(Input(std::stoi(t.lexeme)));

	p = lexer.peek(1);
	if (p.token_type == NUM) { ParseInputs(); }
}



void Parser::ParseStatementList() {
	// --> statement || statement & statement_list
	Token p;


	ParseStatement();

	p = lexer.peek(1);
	if (p.token_type == INPUT || p.token_type == ID) { ParseStatementList(); }
}



void Parser::ParseStatement() {
	// --> input_statement || poly_evaluation_statement
	Token p;


	p = lexer.peek(1);
	if (p.token_type == INPUT) { ParseInputStatement(); }
	else if (p.token_type == ID) { ParsePolyEvalStatement(); }
	else { syntax_error(); }
}



void Parser::ParsePolyEvalStatement() {
	// --> polynomial_evaluation & SEMICOLON
	Token t;


	ParsePolyEval(polyMatrix[evaluations].size());
	// Verifies and consumes "SEMICOLON".
	t = expect(SEMICOLON);
}



void Parser::ParseInputStatement() {
	// --> INPUT & ID & SEMICOLON
	bool addIt = true;
	Token t;


	// Verifies and consumes "INPUT".
	t = expect(INPUT);
	// Verifies and consumes "ID".
	t = expect(ID);

	for (int i = 0; i < inputMatrix[variables].size(); ++i) {
		if (inputMatrix[variables][i].name == t.lexeme) {
			addIt = false;
			inputMatrix[variables][i].intputRefVector.push_back(++inputPos);
		}
	}

	// Appends a new "Input" object with the corresponding token info
	// into the "inputMatrix" if the "addIt" variable equals true.

	if (addIt == true) { inputMatrix[variables].push_back(Input(t.line_no, t.lexeme, ++inputPos)); }

	// Verifies and consumes "SEMICOLON".
	t = expect(SEMICOLON);
}



void Parser::ParsePolyEval(int pCounter) {
	// --> polynomial_name & LPAREN & argument_list & RPAREN
	Token t, p;


	p = lexer.peek(1);
	FindPolyMatch(p.line_no, p.lexeme);

	// Appends a new polynomial with the corresponding token info
	// into the evaluation column of the polynomial vector.
	polyMatrix[evaluations].push_back(Polynomial(p.line_no, p.lexeme));

	ParsePolyName();

	// Verifies and consumes "LPAREN".
	t = expect(LPAREN);

	ParseArgList(pCounter);

	// Verifies and consumes "RPAREN".
	t = expect(RPAREN);
}



void Parser::ParseArgList(int pCounter) {
	// --> argument || argument & COMMA & argument_list
	Token t;


	ParseArg(pCounter);

	t = lexer.GetToken();
	// Checks whether or not to continue parsing this block.
	if (t.token_type == COMMA) { ParseArgList(pCounter); }
	// Returns a "Token" if parsing was discontinued.
	else { lexer.UngetToken(1); }
}



void Parser::ParseArg(int pCounter) {
	// --> ID || NUM || polynomial_evaluation
	int pos;
	Token t, p;


	// Consumes "ID" or "NUM".
	t = lexer.GetToken();

	// Verifies that the type is either "ID" or "NUM" and throws a syntax error otherwise.
	if (t.token_type != ID && t.token_type != NUM) { syntax_error(); }

	// Appends a new monomial to the "parameters" vector at the "pCounter" index.
	polyMatrix[evaluations][pCounter].parameters.push_back(Monomial(t.line_no, t.lexeme, t.token_type));
	pos = polyMatrix[evaluations][pCounter].parameters.size() - 1;



	// Peeks at the next token to determine whether or not to "ParsePolyEval".
	p = lexer.peek(1);

	if (p.token_type == LPAREN) {
		polyMatrix[evaluations][pCounter].parameters[pos].isInitialized = true;
		lexer.UngetToken(1);
		ParsePolyEval(polyMatrix[evaluations].size());
	}
	else if (t.token_type == ID) { SetInputValues(&polyMatrix[evaluations][pCounter].parameters[pos], &t); }
	else { polyMatrix[evaluations][pCounter].parameters[pos].isInitialized = true; }
}



// Accepts an error code number and string of lines on which said errors occured
// and then outputs the concatenated message.
void Parser::PrintError(int pErrorNum, string pErrorList) {
	cout << "Error Code " << pErrorNum << ":" << pErrorList << endl;
}



int Parser::ProcessDeclaration(string pName, std::vector<Input>& pInputTable) {
	int sum = 0, multiplier = 1;
	string sign;


	for (int i = 0; i < polyMatrix[declarations].size(); ++i) {
		if (polyMatrix[declarations][i].name == pName) {
			Polynomial declaration = polyMatrix[declarations][i];


			AsignVarToVal(declaration.parameters, pInputTable);

			for (int j = 0; j < declaration.body.size(); ++j) {
				double exponent = declaration.body[j].exponent;
				double operand = GetOperand(pInputTable, &declaration.body[j]);
				int coefficient = declaration.body[j].coefficient;


				if (declaration.body[j].sign == "*") { multiplier *= coefficient * pow(operand, exponent); }
				else if (sign == "-") {
					sum -= multiplier * (coefficient * pow(operand, exponent));
					multiplier = 1;
					sign = declaration.body[j].sign;
				}
				else {
					sum += multiplier * (coefficient * pow(operand, exponent));
					multiplier = 1;
					sign = declaration.body[j].sign;
				}
			}
		}
	}
	return sum;
}



int Parser::ProcessEvaluation() {
	int pos = evalPos;
	vector<Input> inputTable;


	for (int i = 0; i < polyMatrix[evaluations][pos].parameters.size(); ++i) {
		GetParameterValue(inputTable, &polyMatrix[evaluations][pos].parameters[i]);
	}
	return ProcessDeclaration(polyMatrix[evaluations][pos].name, inputTable);
}



void Parser::ProcessPolynomials() {
	while (evalPos < polyMatrix[evaluations].size()) {
		cout << ProcessEvaluation() << endl;
		++evalPos;
	}
}



// Sets the initialized status and input reference of the parameter monomial.
void Parser::SetInputValues(Monomial* pParameter, Token* pToken) {
	bool initializedInput = false;


	for (int i = 0; i < inputMatrix[variables].size(); ++i) {
		if (inputMatrix[variables][i].name == pToken->lexeme) {
			if (inputMatrix[variables][i].lineNum < pToken->line_no) { initializedInput = true; }
			pParameter->inputIndex = inputMatrix[variables][i].intputRefVector.size() - 1;
		}
	}
	pParameter->isInitialized = initializedInput;
}



void Parser::syntax_error() {
	cout << "SYNTAX ERROR !&%!\n";
	exit(1);
}



// Constructor for the "Input" class.
Input::Input(int pValue) { value = pValue; }



// Constructor for the "Input" class.
Input::Input(string pName, int pValue) {
	name = pName;
	value = pValue;
}



// Constructor for the "Input" class.
Input::Input(int pLineNum, string pName, int pPos) {
	intputRefVector.push_back(pPos);
	lineNum = pLineNum;
	name = pName;
}



// Constructor for the "Polynomial" class.
Polynomial::Polynomial(int pLineNum, string pName) {
	lineNum = pLineNum;
	name = pName;
}



// Constructor for the Monomial class.
Monomial::Monomial(int pLineNum, string pName) {
	lineNum = pLineNum;
	name = pName;
}



// Constructor for the Monomial class.
Monomial::Monomial(int pLineNum, string pName, TokenType pTokenType) {
	lineNum = pLineNum;
	name = pName;
	tokenType = pTokenType;
}














void Parser::p1() {
	for (int i = 0; i < polyMatrix[declarations].size(); ++i) {
		cout
			<< polyMatrix[declarations][i].lineNum
			<< ": "
			<< polyMatrix[declarations][i].name
			<< "(";


		for (int j = 0; j < polyMatrix[declarations][i].parameters.size(); ++j) {
			if (j != 0) { cout << ", "; }
			cout
				<< polyMatrix[declarations][i].parameters[j].name;
		}
		cout << ") = {";

		for (int j = 0; j < polyMatrix[declarations][i].body.size(); ++j) {
			if (j != 0) { cout << ", "; }
			cout
				<< "("
				<< polyMatrix[declarations][i].body[j].coefficient
				<< "*"
				<< polyMatrix[declarations][i].body[j].name
				<< "^"
				<< polyMatrix[declarations][i].body[j].exponent
				<< " "
				<< polyMatrix[declarations][i].body[j].sign
				<< ")";
		}
		cout << "}\n";
	}
	cout << endl;
}



void Parser::p2() {
	for (int i = 0; i < polyMatrix[evaluations].size(); ++i) {
		cout
			<< polyMatrix[evaluations][i].lineNum
			<< ": "
			<< polyMatrix[evaluations][i].name
			<< "{";


		for (int j = 0; j < polyMatrix[evaluations][i].parameters.size(); ++j) {
			if (j != 0) { cout << ", "; }
			cout
				<< "("
				<< polyMatrix[evaluations][i].parameters[j].name
				<< ", "
				<< polyMatrix[evaluations][i].parameters[j].inputIndex
				<< ")";
		}
		cout << "}\n";
	}
	cout << endl;
}



void Parser::p3() {
	cout << "\t\t\tInputs" << endl;
	for (int i = 0; i < inputMatrix[inputs].size(); ++i) {
		if (i != 0) { cout << ", "; }

		cout << inputMatrix[inputs][i].value;
	}
	cout << endl;

	cout << "\n\n\n\t\t\t\tVariables" << endl;
	for (int i = 0; i < inputMatrix[variables].size(); ++i) {

		cout << inputMatrix[variables][i].name << endl;

		for (int j = 0; j < inputMatrix[variables][i].intputRefVector.size(); ++j) {
			cout << "     #" << j << ") " << inputMatrix[variables][i].intputRefVector[j] << endl;
		}

		cout << endl;
	}

	cout << endl;
}
