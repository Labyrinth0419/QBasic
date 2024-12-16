#pragma once
#include "BASICTree.h"
#include <iostream>
#include "SyntaxTree.h"
#include <QObject>
#include <set>

/*
本代码用于定义程序链表，同时存储程序。
*/


class ListNode {
public:
	const int LineNum;
	const SyntaxTree* program;
	ListNode* next;
	ListNode(SyntaxTree* p = nullptr, int LineNum = -1, ListNode* n = nullptr):
		program(p), LineNum(LineNum), next(n) {};
	~ListNode() {
		if (program != nullptr) delete program;
		program = nullptr;
	};


public:
	void LETrun(VarVec& vars);	
	std::string PRINTrun(const VarVec& vars);
	bool IFrun(VarVec vars);
};

class ProgramList : public QObject
{
	Q_OBJECT
private:
	ListNode* head;
	ListNode* cur;
	VarVec vars;
	std::string res;
	bool running = false;
	bool isPause = false;
	bool debugMode = false;
	std::set<int> breakPoints;
	
public:
	ProgramList();
	~ProgramList();

private:
	void add(int LineNum, SyntaxTree* program);
	void del(int LineNum);
	void display();
	void helpDoc();
	ListNode* get(int LineNum);

public:
	void run();//从cur开始运行程序
	void pause();//暂停程序
	bool run(int LineNum);//返回是否继续执行下一行
	void stop(bool clearVars = true);//停止程序，默认清空变量
	void clear();//清空程序
	void setDebugMode(bool mode);//设置debug模式
	bool isRunning() const;//返回是否正在运行
	void step();//单步执行

public://用于处理INPUT指令
	void Input(std::string name, int value);
	bool inputSigReceived = false;
	bool inputMsgSent = false;

public://用于 std::string 类的读取
	ProgramList(std::string doc);
	void Load(std::string doc);
	void SentenseHandle(const std::string& sentence);
	bool contain(int LineNum) const;

private://用于处理输入的字符串
	void add(const std::string& sentence);
	void del(const std::string& sentence);
	void cmd(const std::string& sentence);
	

public://展示出的代码、程序树、变量表
	std::string CodeDisplay() const;
	std::string MonitorDisplay() const;
	std::string SyntaxTreeDisplay() const;
	std::string ResultDisplay() const;
	std::string BreakPointDisplay() const;

signals://信号发送，用于更新显示
	void inputMessage(const std::string& name);
	void exitSignal();
	void inputOnlyMsg(const std::string& name);
	void RenewDisplaySignal();
	void loadSignal();
	void RenewDebugDisplaySignal();
	void pauseAtLineSignal(int LineNum);
};

