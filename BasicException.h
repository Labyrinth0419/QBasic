#pragma once
#include <exception>
#include <string>

class ExpExcep : public std::exception
{
private:
    const std::string message = "Expression Error: "; // ������Ϣ:���ʽ����

public:
    // ���캯��
    ExpExcep(const std::string& msg) : message(msg) {}

    // ����������
    virtual ~ExpExcep() noexcept {}

    // ������Ϣ��ȡ����
    virtual const char* what() const noexcept override {
        return message.c_str();
    }
};

class SyntaxExcep : public std::exception
{
private:
	const std::string message = "Syntax Error: "; // ������Ϣ:�﷨����
public:
	// ���캯��
	SyntaxExcep(const std::string& msg) : message(msg) {}

	// ����������
	virtual ~SyntaxExcep() noexcept {}

	// ������Ϣ��ȡ����
	virtual const char* what() const noexcept override {
		return message.c_str();
	}
};

class VarNameExcep : public std::exception
{
private:
	const std::string message = "Variable Name Error: "; // ������Ϣ:����������
public:
	// ���캯��
	VarNameExcep(const std::string& msg) : message(msg) {}

	// ����������
	virtual ~VarNameExcep() noexcept {}

	// ������Ϣ��ȡ����
	virtual const char* what() const noexcept override {
		return message.c_str();
	}
};

class CommandExcep : public std::exception
{
private:
	const std::string message = "Command Error: "; // ������Ϣ:�������
public:
	// ���캯��
	CommandExcep(const std::string& msg) : message(msg) {}

	// ����������
	virtual ~CommandExcep() noexcept {}

	// ������Ϣ��ȡ����
	virtual const char* what() const noexcept override {
		return message.c_str();
	}
};

class RunExcep : public std::exception
{
private:
	const std::string message = "Runtime Error: "; // ������Ϣ:����ʱ����
public:
	// ���캯��
	RunExcep(const std::string& msg) : message(msg) {}

	// ����������
	virtual ~RunExcep() noexcept {}

	// ������Ϣ��ȡ����
	virtual const char* what() const noexcept override {
		return message.c_str();
	}
};
