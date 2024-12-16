#pragma once
#include "BASICTree.h"
#include <iostream>
/*
本代码用于定义语法树类。
*/

struct SyntaxArg
{//节点参数，可能是表达式树，也可能是字符串
	std::string stat;
	ExpTree* exp;
	SyntaxArg() {
		stat = "";
		exp = nullptr;
	};
	SyntaxArg(std::string str) {
		stat = str;
		exp = nullptr;
	};
	SyntaxArg(ExpTree* tree) {
		exp = tree;
		stat = "";
	};
	~SyntaxArg() {
		if (exp != nullptr) delete exp;
		exp = nullptr;
	};
};

struct SyntaxNode {
	std::string command;
	std::vector<SyntaxArg*> args;
	void display(const std::string prefix = "") const {//用于debug
		for (SyntaxArg* arg : args) {
			if (arg->exp != nullptr) {
				arg->exp->display(prefix);
			}
			else {
				std::cout << prefix <<arg->stat << std::endl;
			}
		}
			
	};
};


class SyntaxTree
{
public:
	enum CommandT {
		REM, LET, PRINT, INPUT, GOTO, IF, END, NOC
	};
	SyntaxTree();
	SyntaxTree(std::string sentence);
	~SyntaxTree();
	void display(const std::string prefix="") const;
	CommandT getCommand() const { return command; };
	std::string getSentence() const;
	std::string getSyntaxTree(const std::string prefix = "") const;
	SyntaxNode* getRoot() const { return root; };
private:
	CommandT command;
	SyntaxNode* root;
	void REMgenerate(std::string sentence);
	void LETgenerate(std::string sentence);
	void PRINTgenerate(std::string sentence);
	void INPUTgenerate(std::string sentence);
	void GOTOgenerate(std::string sentence);
	void IFgenerate(std::string sentence);
	void ENDgenerate(std::string sentence);

	const char cmp_ops[3] = { '>', '<', '='};
	bool isCmpOp(char op) const;


};

