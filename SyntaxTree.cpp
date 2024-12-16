#include "SyntaxTree.h"
#include <regex>

void SyntaxTree::REMgenerate(std::string sentence)
{
	root->command = "REM"; 
	SyntaxArg* arg = new SyntaxArg(sentence);
	root->args.push_back(arg);
}

void SyntaxTree::LETgenerate(std::string sentence)
{
	root->command = "LET =";
	int pos = sentence.find("=");
	if (pos == std::string::npos) {
		throw SyntaxExcep("Syntax Error: Invalid syntax at \"" + sentence + "\"");
	}
	std::string var = sentence.substr(0, pos);
	std::string exp = sentence.substr(pos + 1);
	ExpTree* varTree = new ExpTree(var);
	if (varTree->size() > 1) {
		throw SyntaxExcep("Syntax Error: Too many varialbles at \"" + var + "\"");
	}
	SyntaxArg* arg_var = new SyntaxArg(varTree);
	root->args.push_back(arg_var);
	ExpTree* exptree = new ExpTree(exp);
	SyntaxArg* arg_exp = new SyntaxArg(exptree);
	root->args.push_back(arg_exp);
}

void SyntaxTree::PRINTgenerate(std::string sentence)
{
	root->command = "PRINT";
	ExpTree* tree = new ExpTree(sentence);
	SyntaxArg* arg = new SyntaxArg(tree);
	root->args.push_back(arg);
}

void SyntaxTree::INPUTgenerate(std::string sentence)
{
	root->command = "INPUT";
	auto isVarNameValid = [](const std::string& name) -> bool {
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
	if (isNum(sentence) || !isVarNameValid(sentence)) {
		throw ExpExcep("Expression Error: Invalid expression at \"" + sentence + "\"");
	}
	SyntaxArg* arg = new SyntaxArg(sentence);
	root->args.push_back(arg);
}

void SyntaxTree::GOTOgenerate(std::string sentence)
{
	root->command = "GOTO";
	auto match = [&](std::string str)->bool {
		std::regex pattern("^\\s*[0-9]+\\s*$");
		return std::regex_match(str, pattern);
		};
	if (!match(sentence)) {
		throw SyntaxExcep("Syntax Error: Invalid syntax at \"" + sentence + "\"");
	}
	SyntaxArg* arg = new SyntaxArg(sentence);
	root->args.push_back(arg);
}

void SyntaxTree::IFgenerate(std::string sentence)
{//sentence = "exp1 cmp_op exp2 THEN target"
	root->command = "IF THEN";
	int pos_op = -1;
	for (int i = 0; i < sentence.size(); ++i) {
		if (isCmpOp(sentence[i])) {
			pos_op = i;
			break;
		}
	}
	if (pos_op == -1) {
		throw SyntaxExcep("Syntax Error: Invalid syntax at \"IF " + sentence + "\"");
	}
	int pos_then = sentence.find(" THEN ");
	if (pos_then == std::string::npos) {
		throw SyntaxExcep("Syntax Error: Invalid syntax at \"IF " + sentence + "\"");
	}
	std::string exp1 = sentence.substr(0, pos_op);
	ExpTree* tree = new ExpTree(exp1);
	SyntaxArg* arg = new SyntaxArg(tree);
	std::string cmp_op = sentence.substr(pos_op, 1);
	SyntaxArg* cmp_op_arg = new SyntaxArg(cmp_op);
	if (pos_op + 1 >= sentence.size() || pos_then - pos_op - 1 < 0) {
		throw SyntaxExcep("Syntax Error: Invalid syntax at \"IF " + sentence + "\"");
	}
	std::string exp2 = sentence.substr(pos_op + 1, pos_then - pos_op - 1);
	ExpTree* tree2 = new ExpTree(exp2);
	SyntaxArg* arg2 = new SyntaxArg(tree2);
	if (pos_then + 6 > sentence.size()) {
		throw SyntaxExcep("Syntax Error: Invalid syntax at \"IF " + sentence + "\"");
	}
	std::string target = sentence.substr(pos_then + 6);
	SyntaxArg* target_arg = new SyntaxArg(target);
	root->args.push_back(arg);
	root->args.push_back(cmp_op_arg);
	root->args.push_back(arg2);
	root->args.push_back(target_arg);
}

void SyntaxTree::ENDgenerate(std::string sentence)
{
	root->command = "END";
	if (!sentence.empty()) {
		throw SyntaxExcep("Syntax Error: Invalid syntax at \"" + sentence + "\"");
	}
}

bool SyntaxTree::isCmpOp(char op) const
{
	for (const char cmp_op : cmp_ops) {
		if (cmp_op == op) {
			return true;
		}
	}
	return false;
}

SyntaxTree::SyntaxTree()
{
	command = NOC;
	root = nullptr;
}

SyntaxTree::SyntaxTree(std::string sentence)
{
	root = new SyntaxNode;
	size_t pos = sentence.find(" ");
	std::string cmd = sentence.substr(0,pos);
	if (pos == std::string::npos && cmd != "END" && cmd != "REM") {
		throw SyntaxExcep("Syntax Error: Invalid syntax at \"" + sentence + "\"");
	}
	
	if (cmd == "REM") command = REM;
	else if (cmd == "LET") command = LET;
	else if (cmd == "PRINT") command = PRINT;
	else if (cmd == "INPUT") command = INPUT;
	else if (cmd == "GOTO") command = GOTO;
	else if (cmd == "IF") command = IF;
	else if (cmd == "END") command = END;
	else 
		throw SyntaxExcep("Syntax Error: Unknown command \"" + cmd + "\"");
	
	if (pos == std::string::npos) {
		sentence = "";
	}
	else{
		sentence = sentence.substr(pos + 1);
	}
	switch (command)
	{
	case REM:
		REMgenerate(sentence);
		break;
	case LET:
		LETgenerate(sentence);
		break;
	case PRINT:
		PRINTgenerate(sentence);
		break;
	case INPUT:
		INPUTgenerate(sentence);
		break;
	case GOTO:
		GOTOgenerate(sentence);
		break;
	case IF:
		IFgenerate(sentence);
		break;
	case END:
		ENDgenerate(sentence);
		break;
	default:
		break;
	}
}

SyntaxTree::~SyntaxTree()
{
	delete root;
	root = nullptr;
}

void SyntaxTree::display(const std::string prefix)const
{
	std::string tab = prefix;
	const std::string space = "    ";
	std::cout << root->command << std::endl;
	root->display(space);
}

std::string SyntaxTree::getSentence() const
{
	std::string res = "";
	switch (command)
	{
	case REM:
		res += "REM ";
		res += root->args[0]->stat;
		break;
	case LET:
		res += "LET ";
		res += root->args[0]->exp->getExp();
		res += " = ";
		res += root->args[1]->exp->getExp();
		break;
	case PRINT:
		res += "PRINT ";
		res += root->args[0]->exp->getExp();
		break;
	case INPUT:
		res += "INPUT ";
		res += root->args[0]->stat;
		break;
	case GOTO:
		res += "GOTO ";
		res += root->args[0]->stat;
		break;
	case IF:
		res += "IF ";
		res += root->args[0]->exp->getExp();
		res += " ";
		res += root->args[1]->stat;
		res += " ";
		res += root->args[2]->exp->getExp();
		res += " THEN ";
		res += root->args[3]->stat;
		break;
	case END:
		res += "END";
		break;
	default:
		break;
	}
	return res;
}

std::string SyntaxTree::getSyntaxTree(const std::string prefix) const
{
	std::string tab = prefix;
	const std::string space = "    ";
	std::string res = "";
	res += root->command + "\n";
	for (SyntaxArg* arg : root->args) {
		if (arg->exp != nullptr) {
			res += tab + arg->exp->getExpTree(space) + "\n";
		}
		else {
			res += tab + space + arg->stat + "\n";
		}
	}
	return res;
}
