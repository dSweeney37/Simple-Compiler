/*
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 */
#ifndef __PARSER_H__
#define __PARSER_H__
#define declarations 0
#define evaluations 1
#define variables 0
#define inputs 1

#include <math.h>
#include <string>
#include <vector>
#include "lexer.h"




class Input {
	public:
		Input(int);
		Input(std::string, int);
		Input(int, std::string, int);

		int lineNum, value;
		std::string name;
		std::vector<int> intputRefVector;
};



class Monomial {
	public:
		Monomial(int, std::string);
		Monomial(int, std::string, TokenType);

		bool isInitialized = false;
		int coefficient = 1, exponent = 1, inputIndex = 0, lineNum;
		std::string name, sign;
		TokenType tokenType;
};



class Polynomial {
	public:
		Polynomial(int, std::string);

		bool isDupe = false;
		int lineNum;
		std::string name;
		std::vector<Monomial> body, parameters;
};



class Parser {
	public:
		void ParseInput();


	private:
		int inputPos = -1, evalPos = 0;
		LexicalAnalyzer lexer;
		std::string error3Str;
		std::vector<std::vector<Input>> inputMatrix;
		// polyMatrix[0][*] = polynomial declarations, polyMatrix[1][*] = polynomial evaluations
		std::vector<std::vector<Polynomial>> polyMatrix;

		void AsignVarToVal(std::vector<Monomial>&, std::vector<Input>&);
		void Code1Errors();
		void Code2Errors();
		void Code3Errors();
		void Code4Errors();
		void Code5Errors();
		Token expect(TokenType);
		void FindPolyMatch(int, std::string);
		double GetOperand(std::vector<Input>&, Monomial*);
		void GetParameterValue(std::vector<Input>&, Monomial*);
		void ParseProgram();
		void ParsePolyDeclSec();
		void ParsePolyDecl();
		void ParsePolyHeader();
		void ParseIdList();
		void ParsePolyName();
		void ParsePolyBody();
		void ParseTermList();
		void ParseTerm();
		void ParseMonoList(int);
		void ParseMono(int);
		void ParseExp();
		std::string ParseAddOp();
		Monomial ParseCoef();
		void ParseStart();
		void ParseInputs();
		void ParseStatementList();
		void ParseStatement();
		void ParsePolyEvalStatement();
		void ParseInputStatement();
		void ParsePolyEval(int);
		void ParseArgList(int);
		void ParseArg(int);
		void PrintError(int, std::string);
		int ProcessDeclaration(std::string, std::vector<Input>&);
		int ProcessEvaluation();
		void ProcessPolynomials();
		void SetInputValues(Monomial*, Token*);
		void syntax_error();

		void p1();
		void p2();
		void p3();
};

#endif
