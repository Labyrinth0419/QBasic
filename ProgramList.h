#pragma once
#include "BASICTree.h"
#include <iostream>
#include "SyntaxTree.h"
#include <QObject>
#include <set>

/*
���������ڶ����������ͬʱ�洢����
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
	void run();//��cur��ʼ���г���
	void pause();//��ͣ����
	bool run(int LineNum);//�����Ƿ����ִ����һ��
	void stop(bool clearVars = true);//ֹͣ����Ĭ����ձ���
	void clear();//��ճ���
	void setDebugMode(bool mode);//����debugģʽ
	bool isRunning() const;//�����Ƿ���������
	void step();//����ִ��

public://���ڴ���INPUTָ��
	void Input(std::string name, int value);
	bool inputSigReceived = false;
	bool inputMsgSent = false;

public://���� std::string ��Ķ�ȡ
	ProgramList(std::string doc);
	void Load(std::string doc);
	void SentenseHandle(const std::string& sentence);
	bool contain(int LineNum) const;

private://���ڴ���������ַ���
	void add(const std::string& sentence);
	void del(const std::string& sentence);
	void cmd(const std::string& sentence);
	

public://չʾ���Ĵ��롢��������������
	std::string CodeDisplay() const;
	std::string MonitorDisplay() const;
	std::string SyntaxTreeDisplay() const;
	std::string ResultDisplay() const;
	std::string BreakPointDisplay() const;

signals://�źŷ��ͣ����ڸ�����ʾ
	void inputMessage(const std::string& name);
	void exitSignal();
	void inputOnlyMsg(const std::string& name);
	void RenewDisplaySignal();
	void loadSignal();
	void RenewDebugDisplaySignal();
	void pauseAtLineSignal(int LineNum);
};

