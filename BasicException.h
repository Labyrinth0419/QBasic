#pragma once
#include <exception>
#include <string>

class ExpExcep : public std::exception
{
private:
    const std::string message = "Expression Error: "; // 错误信息:表达式错误

public:
    // 构造函数
    ExpExcep(const std::string& msg) : message(msg) {}

    // 虚析构函数
    virtual ~ExpExcep() noexcept {}

    // 错误信息获取函数
    virtual const char* what() const noexcept override {
        return message.c_str();
    }
};

class SyntaxExcep : public std::exception
{
private:
	const std::string message = "Syntax Error: "; // 错误信息:语法错误
public:
	// 构造函数
	SyntaxExcep(const std::string& msg) : message(msg) {}

	// 虚析构函数
	virtual ~SyntaxExcep() noexcept {}

	// 错误信息获取函数
	virtual const char* what() const noexcept override {
		return message.c_str();
	}
};

class VarNameExcep : public std::exception
{
private:
	const std::string message = "Variable Name Error: "; // 错误信息:变量名错误
public:
	// 构造函数
	VarNameExcep(const std::string& msg) : message(msg) {}

	// 虚析构函数
	virtual ~VarNameExcep() noexcept {}

	// 错误信息获取函数
	virtual const char* what() const noexcept override {
		return message.c_str();
	}
};

class CommandExcep : public std::exception
{
private:
	const std::string message = "Command Error: "; // 错误信息:命令错误
public:
	// 构造函数
	CommandExcep(const std::string& msg) : message(msg) {}

	// 虚析构函数
	virtual ~CommandExcep() noexcept {}

	// 错误信息获取函数
	virtual const char* what() const noexcept override {
		return message.c_str();
	}
};

class RunExcep : public std::exception
{
private:
	const std::string message = "Runtime Error: "; // 错误信息:运行时错误
public:
	// 构造函数
	RunExcep(const std::string& msg) : message(msg) {}

	// 虚析构函数
	virtual ~RunExcep() noexcept {}

	// 错误信息获取函数
	virtual const char* what() const noexcept override {
		return message.c_str();
	}
};
