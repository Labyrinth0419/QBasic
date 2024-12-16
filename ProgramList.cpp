#include "ProgramList.h"
#include <sstream>

ProgramList::ProgramList()
{
	head = nullptr;
	cur = nullptr;
}

ProgramList::~ProgramList()
{
	ListNode* temp = head;
	while (temp != nullptr) {
		ListNode* next = temp->next;
		delete temp;
		temp = next;
	}
	head = nullptr;
	cur = nullptr;
}

void ProgramList::add(int LineNum, SyntaxTree* program)
{//按照行号排序插入
	if (contain(LineNum)) {
		del(LineNum);
	}
	ListNode* node = new ListNode(program, LineNum);
	if (head == nullptr) {
		head = node;
		cur = node;
	}
	else {
		ListNode* temp = head;
		ListNode* prev = nullptr;
		while (temp != nullptr && temp->LineNum < LineNum) {
			prev = temp;
			temp = temp->next;
		}
		if (prev == nullptr) {
			node->next = head;
			head = node;
		}
		else {
			prev->next = node;
			node->next = temp;
		}
	}

}

void ProgramList::del(int LineNum)
{
	ListNode* temp = head;
	ListNode* prev = nullptr;
	while (temp != nullptr && temp->LineNum != LineNum) {
		prev = temp;
		temp = temp->next;
	}
	if (temp == nullptr) return;
	if (prev == nullptr) {
		head = temp->next;
	}
	else {
		prev->next = temp->next;
	}
	delete temp;
	temp = nullptr;
}

void ProgramList::display()
{//用于debug
	ListNode* temp = head;
	while (temp != nullptr) {
		std::cout << temp->LineNum << " ";
		temp->program->display();
		temp = temp->next;
	}
}

void ProgramList::helpDoc()
{
	res = std::string("Welcome to the help document of the BASIC Interpreter.\n") +
		"Here are some instructions for you:\n" +
		"1. You can input your code in the cmdLine area.\n" +
		"2. You can run your code by clicking the Run button.\n" +
		"3. You can clear all the code by clicking the Clear button.\n" +
		"4. You can load a file by clicking the Load button.\n" +
		"5. You can enter debug mode by clicking the Debug Mode button.\n" +
		"6. You can exit debug mode by clicking the Exit Debug Mode button.\n" +
		"7. You can resume the code execution by clicking the Resume button.\n" +
		"8. You can set breakpoints by entering ADD LINENUM in the Code Display area.\n" +
		"9. You can clear breakpoints by entering DELETE LINENUM in cmdLine.\n";
}

ListNode* ProgramList::get(int LineNum)
{
	ListNode* temp = head;
	while (temp != nullptr && temp->LineNum != LineNum) {
		temp = temp->next;
	}
	if (temp == nullptr) return nullptr;
	return temp;
}

void ProgramList::run()
{
	if (!running) {
		running = true;
		isPause = false;
		res = "";
		cur = head;
	}
	if (isPause && running) {
		isPause = false;
	}
	while (cur != nullptr && running && !isPause) {
		int LineNum = cur->LineNum;
		if (debugMode && breakPoints.find(LineNum) != breakPoints.end()) {
			pause();
			emit RenewDebugDisplaySignal();
			emit RenewDisplaySignal();
			emit pauseAtLineSignal(LineNum);
			break;
		}
		if (run(LineNum))
			cur = cur->next;
		//emit RenewDisplaySignal(); //发送信号会使运行速度大大降低
	}
	if (cur == nullptr) {
		running = false;
		isPause = false;
		vars.clear();
		cur = head;
		emit RenewDisplaySignal();
		emit RenewDebugDisplaySignal();
		res = "";
	}
	
}

void ProgramList::pause()
{
	isPause = true;
}

bool ProgramList::run(int LineNum)
{
	bool result = true;
	cur = get(LineNum);
	if (cur == nullptr) return true;
	switch (cur->program->getCommand())
	{
	case SyntaxTree::REM:
		result = true;
		break;
	case SyntaxTree::LET:
		cur->LETrun(vars);
		result = true;
		break;
	case SyntaxTree::PRINT:
		res += cur->PRINTrun(vars) + "\n";
		result = true;
		break;
	case SyntaxTree::INPUT:
		emit inputMessage(cur->program->getRoot()->args[0]->stat);
		break;
	case SyntaxTree::GOTO:
		cur = get(std::stoi(cur->program->getRoot()->args[0]->stat));
		result = false;
		break;
	case SyntaxTree::IF:
		if (cur->IFrun(vars)) {
			cur = get(std::stoi(cur->program->getRoot()->args[3]->stat));
			result = false;
		}
		else{
			result = true;
		}
		break;
	case SyntaxTree::END:
		cur = nullptr;
		result = false;
		break;
	}
	/*emit RenewDisplaySignal();
	if(debugMode){
		emit RenewDebugDisplaySignal();
	}*/
	return result;
}

void ProgramList::stop(bool vc)
{
	running = false;
	isPause = false;
	cur = nullptr;
	res = "";
	if(vc)
		vars.clear();
}

void ProgramList::clear()
{
	ListNode* temp = head;
	while (temp != nullptr) {
		ListNode* next = temp->next;
		delete temp;
		temp = next;
	}
	vars.clear();
	breakPoints.clear();
	head = nullptr;
	cur = nullptr;
	res = "";
}

void ProgramList::setDebugMode(bool mode)
{
	debugMode = mode;
	stop();
}

bool ProgramList::isRunning() const
{
	return running;
}

void ProgramList::step()
{
	if (run(cur->LineNum))
		cur = cur->next;
}

void ProgramList::Input(std::string name, int value)
{
	if (vars.isExist(name)) {
		vars.set(name, value);
	}
	else {
		vars.add(name, value);
	}
}


void ListNode::LETrun(VarVec& vars)
{
	std::string name = program->getRoot()->args[0]->exp->getExp();
	int value = program->getRoot()->args[1]->exp->calculate(vars);
	vars.set(name, value);
}

std::string ListNode::PRINTrun(const VarVec& vars)
{
	return std::to_string(program->getRoot()->args[0]->exp->calculate(vars));
}

bool ListNode::IFrun(VarVec vars)
{
	int res1 = program->getRoot()->args[0]->exp->calculate(vars);
	int res2 = program->getRoot()->args[2]->exp->calculate(vars);
	std::string op = program->getRoot()->args[1]->stat;
	if (op == ">") {
		return res1 > res2;
	}
	else if (op == "<") {
		return res1 < res2;
	}
	else if (op == "=") {
		return res1 == res2;
	}
	throw SyntaxExcep("Syntax Error: Unknown syntax at \"" + op + "\"");
}

ProgramList::ProgramList(std::string doc)
{
	Load(doc);
}

void ProgramList::Load(std::string doc)
{
	std::istringstream iss(doc);
	std::string line;
	while (std::getline(iss, line)) {
		if (line == "") continue;
		size_t pos = line.find(" ");
		if (pos == std::string::npos) {
			del(line);
			continue;
		}
		for (int i = 0; i < pos; i++) {
			if (!isdigit(line[i])) {
				throw SyntaxExcep("Line Number Error: " + line.substr(0, pos));
			}
		}
		std::istringstream iss2(line);
		int LineNum;
		LineNum = (iss2 >> LineNum) ? LineNum : -1;
		if (LineNum <= 0 || LineNum > 1E6) {
			throw SyntaxExcep("Line Number Error: " + std::to_string(LineNum) + " range exceeded");
		}
		std::string sentence = line.substr(pos + 1);
		SyntaxTree* program = new SyntaxTree(sentence);
		add(LineNum, program);
	}
}

void ProgramList::SentenseHandle(const std::string& sentence)
{
	if (sentence.empty()) return;
	auto match = [&](std::string str)->bool {
		size_t start = str.find_first_not_of(" \t\n\r\f\v");
		if (start == std::string::npos) {
			return false;
		}
		size_t end = str.find_last_not_of(" \t\n\r\f\v");
		for (size_t i = start; i <= end; ++i) {
			if (!std::isdigit(str[i])) {
				return false;
			}
		}
		return true;
		};
	if (match(sentence)) {
		del(sentence);
	}
	else {
		std::string cmd = sentence.substr(sentence.find_first_not_of(" "));
		size_t pos = (cmd.find(" ") != std::string::npos) ? (cmd.find(" ")) : (cmd.size());
		cmd = cmd.substr(0, pos);
		const std::string cmdList[] = { "LET", "PRINT", "INPUT", "RUN", "CLEAR", "LOAD", "LIST", "HELP", "QUIT", "ADD", "DELETE"};
		for (const std::string c: cmdList)
		{
			if (cmd == c) {
				this->cmd(sentence);
				return;
			}
		}
		add(sentence);
		return;
	}
}

void ProgramList::add(const std::string& sentence)
{
	if (sentence.empty()) return;
	bool flag = false;
	for (const char n : sentence) {
		if (!isspace(n)) {
			flag = true;
			break;
		}
	}
	if (!flag) return;

	int pos = sentence.find(" ");
	if (pos == std::string::npos) {
		throw SyntaxExcep("Syntax Error: Unknown syntax at \"" + sentence + "\"");
	}
	for (int i = 0; i < pos; i++) {
		if (!isdigit(sentence[i])) {
			throw SyntaxExcep("Line Number Error: " + sentence.substr(0, pos));
		}
	}
	std::istringstream iss(sentence);
	int LineNum;
	LineNum = (iss >> LineNum) ? LineNum : -1;
	if (LineNum <= 0 || LineNum > 1E6) {
		throw SyntaxExcep("Line Number Error: " + std::to_string(LineNum) + " range exceeded");
	}
	if (pos + 1 >= sentence.size()) {
		throw SyntaxExcep("Syntax Error: Unknown syntax at \"" + sentence + "\"");
	}
	std::string sentence2 = sentence.substr(pos + 1);
	SyntaxTree* program = new SyntaxTree(sentence2);
	add(LineNum, program);
}

void ProgramList::del(const std::string& sentence)
{
	auto match = [&](std::string str)->bool {
		size_t start = str.find_first_not_of(" \t\n\r\f\v");
		if (start == std::string::npos) {
			return false;
		}
		size_t end = str.find_last_not_of(" \t\n\r\f\v");
		for (size_t i = start; i <= end; ++i) {
			if (!std::isdigit(str[i])) {
				return false;
			}
		}
		return true;
		};
	if (!match(sentence)) {
		throw SyntaxExcep("Syntax Error: \"" + sentence + "\" is not an invalid number.");
	}
	std::istringstream iss(sentence);
	int LineNum;
	LineNum = (iss >> LineNum) ? LineNum : -1;
	if (LineNum <= 0 || LineNum > 1E6) {
		throw SyntaxExcep("Line Number Error: " + std::to_string(LineNum) + " range exceeded.");
	}
	del(LineNum);
}

void ProgramList::cmd(const std::string& sentence)
{//"INPUT m", "PRINT m"
	if (sentence.empty()) return;
	std::string cmd = sentence.substr(sentence.find_first_not_of(" "));
	size_t pos = (cmd.find(" ") != std::string::npos) ? (cmd.find(" ")) : (cmd.size());
	cmd = cmd.substr(0, pos);
	auto match = [](std::string str)->bool {
		size_t start = str.find_first_not_of(" \t\n\r\f\v");
		if (start == std::string::npos) {
			return false;
		}
		size_t end = str.find_last_not_of(" \t\n\r\f\v");
		for (size_t i = start; i <= end; ++i) {
			if (!std::isdigit(str[i])) {
				return false;
			}
		}
		return true;
		};
	if (cmd == "LET" || cmd == "PRINT" || cmd == "INPUT") {//三项后接program
		SyntaxTree* program = new SyntaxTree(sentence);
		ListNode temp(program);
		try{
			switch (program->getCommand())
			{
			case SyntaxTree::LET:
				temp.LETrun(vars);
				break;
			case SyntaxTree::PRINT:
				res += temp.PRINTrun(vars) + "\n";
				break;
			case SyntaxTree::INPUT:
				emit inputOnlyMsg(program->getRoot()->args[0]->stat);
				break;
			}
		}
		catch (std::exception& e) {
			throw CommandExcep(e.what());
		}
		emit RenewDisplaySignal();
		return;
	}
	if (cmd == "ADD") {
		int LineNum;
		if (!match(sentence.substr(3))) {
			throw CommandExcep("Syntax Error: Invalid line number at \"" + sentence + "\"");
		}
		LineNum = std::stoi(sentence.substr(3));
		if (LineNum <= 0 || LineNum > 1E6) {
			throw CommandExcep("Line Number Error: " + std::to_string(LineNum) + " range exceeded.");
		}
		if (!contain(LineNum)) {
			throw CommandExcep("Line Number Error: " + std::to_string(LineNum) + " doesn't exist.");
		}
		breakPoints.insert(LineNum);
		emit RenewDebugDisplaySignal();
		return;
	}
	else if (cmd == "DELETE") {
		int LineNum;
		if (!match(sentence.substr(6))) {
			throw CommandExcep("Syntax Error: Invalid line number at \"" + sentence + "\"");
		}
		LineNum = std::stoi(sentence.substr(6));
		if (LineNum <= 0 || LineNum > 1E6) {
			throw CommandExcep("Line Number Error: " + std::to_string(LineNum) + " range exceeded.");
		}
		if (!contain(LineNum)) {
			throw CommandExcep("Line Number Error: " + std::to_string(LineNum) + " doesn't exist.");
		}
		breakPoints.erase(LineNum);
		emit RenewDebugDisplaySignal();
		return;
	}
	if (cmd == "RUN") {
		run();
	}
	else if (cmd == "CLEAR") {
		clear();
		emit RenewDisplaySignal();
	}
	else if (cmd == "LOAD") {
		emit loadSignal();
	}
	else if (cmd == "HELP") {
		helpDoc();
		emit RenewDisplaySignal();
	}
	else if (cmd == "QUIT") {
		emit exitSignal();
	}
	else if (cmd == "LIST") {
		return;
	}
	else {
		throw CommandExcep("Syntax Error: Unknown syntax at \"" + sentence + "\"");
	}
}

bool ProgramList::contain(int LineNum) const
{
	ListNode* temp = head;
	while (temp != nullptr) {
		if (temp->LineNum == LineNum) return true;
		temp = temp->next;
	}
	return false;
}

std::string ProgramList::CodeDisplay() const
{
	std::string res;
	ListNode* temp = head;
	while (temp != nullptr) {
		res += std::to_string(temp->LineNum) + " " + temp->program->getSentence() + "\n";
		temp = temp->next;
	}
	return res;
}

std::string ProgramList::MonitorDisplay() const
{
	std::string res;
	for (int i = 0; i < vars.size(); i++) {
		res += vars[i].first + "\t" + std::to_string(vars[i].second) + "\n";
	}
	return res;
}

std::string ProgramList::SyntaxTreeDisplay() const
{
	std::string res;
	ListNode* temp = head;
	while (temp != nullptr) {
		res += std::to_string(temp->LineNum) + " ";
		res += temp->program->getSyntaxTree();
		temp = temp->next;
	}
	return res;
}

std::string ProgramList::ResultDisplay() const
{
	return res;
}

std::string ProgramList::BreakPointDisplay() const
{
	std::string res = "";
	for (int i : breakPoints) {
		res += std::to_string(i) + "\n";
	}
	return res;
}
