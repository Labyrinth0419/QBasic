#include "BASICTree.h"
#include <stack>
#include <queue>
#include <iostream>



void ExpTree::isBracketValid(std::string exp) const
{
	std::stack<std::string> s;
	for (char c : exp) {
		if (c == '(') {
			s.push("(");
		}
		else if (c == ')') {
			if (s.empty()) {
				throw ExpExcep("Expression Error: Bracket is not matched at " + exp);
			}
			s.pop();
		}
	}
	bool res = s.empty();
	if (!res) {
		throw ExpExcep("Expression Error: Bracket is not matched at " + exp);
	}
}

void ExpTree::isOperatorValid() const
{
	auto isOP = [this](const std::string& name) -> bool {
		if (isOperator(name)) {
			return true;
		}
		else if (name == "(" || name == ")") {
			return true;
		}
		return false;
		};
	for (int i = 0; i < token.size() - 1; i++) {
		if (!isOP(token[i]) && !isOP(token[i + 1])) {
			throw ExpExcep("Expression Error: Varialble " + token[i] + " and " + token[i + 1] + " are adjacent");
		}
	}
	isOperatorValid(root);
}

void ExpTree::isOperatorValid(TreeNode* node) const
{
	if (!node) return;
	if (node->type == TreeNode::OP) {
		if (node->val == "**") {
			if (node->R == nullptr) {
				throw ExpExcep("Expression Error: Operator ** need two operands");
			}
		}
		else if (node->val == "-") {
			if (node->R == nullptr) {
				throw ExpExcep("Expression Error: Operator - need at least one operand");
			}
		}
		else {
			if (node->L == nullptr || node->R == nullptr) {
				throw ExpExcep("Expression Error: Operator " + node->val + " need two operands");
			}
		}
	}
	isOperatorValid(node->L);
	isOperatorValid(node->R);
}

void ExpTree::isTokenValid() const
{
	auto isVarNameValid = [](const std::string & name) -> bool{
		const std::string reserved[7] = { "REM", "LET", "PRINT", "INPUT", "GOTO", "IF", "END" };
		if (name.empty()) {
			return false;
		}
		if (name[0] >= '0' && name[0] <= '9') {
			return false;
		}
		for (char c : name) {
			if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))) {
				return false;
			}
		}
		for (const std::string& res : reserved) {
			if (name == res) {
				return false;
			}
		}
		return true;
	};
	auto isNum = [](const std::string& name) -> bool {
		for (char c : name) {
			if (!isdigit(c)) {
				return false;
			}
		}
		return true;
		};
	auto isOP = [this](const std::string& name) -> bool {
		if (isOperator(name)) {
			return true;
		}
		else if (name == "(" || name == ")"){
			return true;
		}
		return false;
		};
	for (const std::string tok : token) {
		if (!isVarNameValid(tok) && !isNum(tok) && !isOP(tok)) {
			throw ExpExcep("Expression Error: Invalid variable name: " + tok);
		}
	}
}

int ExpTree::mod(int a, int b) const
{
	int res = a % b;
	if ((b > 0 && res < 0) || (b < 0 && res >0)) {
		res += b;
	}
	return res;
}

void ExpTree::tokenize(std::string exp)
{
	isBracketValid(exp);
	std::string temp = "";
	for (int i = 0; i < exp.size(); i++) {
		if (exp[i] == ' ') {
			if (!temp.empty()) {
				token.push_back(temp);
				temp.clear();
			}
			continue;
		}
		if (exp[i] == '+' || exp[i] == '-' || exp[i] == '/') {
			if (!temp.empty()) {
				token.push_back(temp);
				temp.clear();
			}
			token.push_back(std::string(1, exp[i]));
			continue;
		}
		if (exp[i] == '*') {
			if (temp == "*") {//operator **
				temp += exp[i];
				token.push_back(temp);
				temp.clear();
			}
			else if (temp.empty()) {//operator *
				temp += exp[i];
			}
			else {//number
				token.push_back(temp);
				temp.clear();
				temp += exp[i];
			}
			continue;
		}
		if (exp[i] == '(' || exp[i] == ')') {
			if (!temp.empty()) {
				token.push_back(temp);
				temp.clear();
			}
			token.push_back(std::string(1, exp[i]));
			continue;
		}
		if (temp == "*") {
			token.push_back(temp);
			temp.clear();
		}
		temp += exp[i];
	}
	if (!temp.empty()) {
		token.push_back(temp);
		temp.clear();
	}
	isTokenValid();
}

void ExpTree::display(const std::string prefix) const
{
	std::queue <TreeNode*> q;
	root->setLevel(0);
	q.push(root);
	int cur_level = 0;
	const std::string space = "    ";//4 spaces
	std::string tab = prefix;
	while (!q.empty())
	{
		TreeNode* node = q.front();
		q.pop();
		if (node->level > cur_level) {
			tab += space;
			cur_level++;
		}
		std::cout << tab << node->val << std::endl;
		if (node->L != nullptr) {
			q.push(node->L);
		}
		if (node->R != nullptr) {
			q.push(node->R);
		}
		
		
	}
}

std::string ExpTree::getExp() const
{
	std::string res = "";
	for (const std::string tok : token) {
		res += tok + " ";
	}
	//去掉最后一个空格
	if (!res.empty()) {
		res.pop_back();
	}
	return res;
}

std::string ExpTree::getExpTree(const std::string prefix) const
{
	std::queue <TreeNode*> q;
	root->setLevel(0);
	q.push(root);
	int cur_level = 0;
	const std::string space = "    ";//4 spaces
	std::string tab = prefix;
	std::string res = "";
	while (!q.empty())
	{
		TreeNode* node = q.front();
		q.pop();
		if (node->level > cur_level) {
			tab += space;
			cur_level++;
		}
		res += tab + node->val + "\n";
		if (node->L != nullptr) {
			q.push(node->L);
		}
		if (node->R != nullptr) {
			q.push(node->R);
		}
		
	}
	//去掉最后一个换行符
	if (!res.empty()) {
		res.pop_back();
	}
	return res;
}

int ExpTree::getPriority(std::string op) const
{
	if (op == "+" || op == "-") {
		return 1;
	}
	if (op == "*" || op == "/" || op == "MOD") {
		return 2;
	}
	if (op == "**") {
		return 3;
	}
	return 0;
}

bool ExpTree::isOperator(std::string op) const
{
	const std::string operators[6] = { "+", "-", "*", "/", "MOD", "**" };
	for (const std::string o : operators) {
		if (op == o) {
			return true;
		}
	}
	return false;
}

void ExpTree::setlevel()
{
	setlevel(root, 0);
}

void ExpTree::setlevel(TreeNode* node, int l)
{
	if (!node) return;
	node->level = l;
	setlevel(node->L, l + 1);
	setlevel(node->R, l + 1);
}

ExpTree::ExpTree()
{
	root = nullptr;
}

ExpTree::ExpTree(std::string exp)
{
	tokenize(exp);
	root = generate(0,token.size());
	isOperatorValid();//检查表达式是否合法
	setlevel();
}

ExpTree::~ExpTree()
{
	deleteTree(root);
}

TreeNode* ExpTree::generate(int st, int ed)
{//处理表达式字符串
	if (st >= ed) {
		return nullptr;
	}
	std::stack<std::string> s;
	if (token[st] == "(" && token[ed - 1] == ")") {//前后匹配括号，去掉括号
		bool isBracketed = true;
		for (int i = st + 1; i < ed - 1; i++) {
			if (token[i] == "(") {
				s.push("(");
			}
			if (token[i] == ")") {
				if (s.empty()) {
					isBracketed = false;
					break;
				}
				s.pop();
			}
		}
		if (isBracketed) {
			return generate(st + 1, ed - 1);
		}
		while (!s.empty()) {
			s.pop();
		}
	}
	TreeNode* node = nullptr;
	int min_pr = 0x3f3f3f3f;
	int pos = -1;
	for (int i = st; i < ed; i++) {
		while (!s.empty() && i < ed) {//跳过括号
			if (token[i] == "(") {
				s.push("(");
			}
			else if (token[i] == ")") {
				s.pop();
			}
			i++;
		}
		if (i >= ed) break;
		if (token[i] == "(") {
			s.push("(");
		}
		if (isOperator(token[i])) {
			int pr = getPriority(token[i]);
			if (pr <= min_pr) {
				min_pr = pr;
				pos = i;
			}
		}
	}
	
	if (min_pr == 3) {
		for (int flag = 0, i = st; i < ed; i++){
			if(token[i] == "(")
				flag++;
			if (token[i] == ")")
				flag--;
			if (flag == 0 && token[i] == "**") {
				pos = i;
				break;
			}
		}
	}
	if (pos == -1) {
		node = new TreeNode(token[st]);
	}
	else {
		node = new TreeNode(token[pos], generate(st, pos), generate(pos + 1, ed));
	}
	return node;
}

int ExpTree::size() const
{
	return token.size();
}


void ExpTree::deleteTree(TreeNode* node)
{
	if (node == nullptr) {
		return;
	}
	deleteTree(node->L);
	deleteTree(node->R);
	delete node;
	node = nullptr;
}

int ExpTree::calculate(const VarVec& vars,TreeNode* node) const
{
	if (!node) return 0;//应对一元运算符 "-"
	auto isNum = [&](std::string str) -> bool{
		for (char c : str) {
			if (!isdigit(c)) {
				return false;
			}
		}
		return true;
	};
	if (node->type == TreeNode::OP) {
		std::string op = node->val;
		if (op == "+") {
			return calculate(vars, node->L) + calculate(vars, node->R);
		}
		if (op == "-") {
			return calculate(vars, node->L) - calculate(vars, node->R);
		}
		if (op == "*") {
			return calculate(vars, node->L) * calculate(vars, node->R);
		}
		if (op == "/") {
			if (calculate(vars, node->R) == 0) {
				throw ExpExcep("Expression Error: Division by zero");
			}
			return calculate(vars, node->L) / calculate(vars, node->R);
		}
		if (op == "MOD") {
			if (calculate(vars, node->R) == 0) {
				throw ExpExcep("Expression Error: Division by zero");
			}
			return mod(calculate(vars,node->L), calculate(vars,node->R));
		}
		if (op == "**") {
			return pow(calculate(vars, node->L), calculate(vars, node->R));
		}
	}
	else {
		std::string var = node->val;
		if (isNum(var)) {//数字
			return std::stoi(var);
		}
		else {//变量名
			return vars.get(var);
		}
	}
	return 0;
}

int ExpTree::calculate(const VarVec& vars) const
{
	return calculate(vars, root);
}



