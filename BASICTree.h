#pragma once
#include "VarVec.h"
#include "BasicException.h"


/*
本代码用于定义表达式树类。
*/

struct TreeNode {
private:
	const std::string operators[6] = {"+", "-", "*", "/", "MOD", "**"};
	bool isOperator(std::string op) {
		for (const std::string o : operators) {
			if (op == o) {
				return true;
			}
		}
		return false;
	}
public:
	enum Type {
		VAR, OP
	};
	int level;
	Type type;
	std::string val;
	TreeNode* L = nullptr;
	TreeNode* R = nullptr;
	TreeNode(std::string data, TreeNode* L=nullptr, TreeNode* R=nullptr, int l=-1) {
		if (isOperator(data)) {
			this->type = Type::OP;
			this->val = data;
		}
		else {
			this->type = Type::VAR;
			this->val = data;
		}
		this->L = L;
		this->R = R;
		this->level = l;
	}
	void setLevel(int l) {
		this->level = l;
	}
};

class ExpTree
{
private:
	TreeNode* root;
	std::vector<std::string> token;
	int getPriority(std::string op) const;
	bool isOperator(std::string op) const;
	void setlevel();
	void setlevel(TreeNode* node, int l);

private:
	void isBracketValid(std::string exp) const;
	void isOperatorValid() const;
	void isOperatorValid(TreeNode* node) const;
	void isTokenValid() const;
	int mod(int a, int b) const;

public:
	ExpTree();
	ExpTree(std::string exp);
	~ExpTree();
	TreeNode* generate(int st, int ed);
	int size() const;
	void deleteTree(TreeNode* node);
	int calculate(const VarVec& vars, TreeNode* node) const;
	int calculate(const VarVec& vars) const;
	void tokenize(std::string exp);
	void display(const std::string prefix = "") const;//用于debug

public:
	std::string getExp() const;
	std::string getExpTree(const std::string prefix = "") const;
	
};

