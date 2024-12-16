#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include "BasicException.h"

struct Var {
	int value;
	std::string name;
	Var():name("") {
		value = 0;
	};
	Var(std::string n, int v):name(n) {
		value = v;
	};
	int getVar() const{
		return value;
	};
	//重载赋值运算符
	Var& operator=(const Var& v) {
		name = v.name;
		value = v.value;
		return *this;
	};
};

struct VarVec {//用于存储变量
	std::vector<Var> vec;
	VarVec() {
		vec.clear();
	};
	~VarVec() {
		vec.clear();
	};
	void add(std::string name, int value) {
		vec.push_back(Var(name, value));
		sort();
	};
	void sort() {
		auto cmp = [](Var a, Var b) -> bool {
			return a.name < b.name;
			};
		std::sort(vec.begin(), vec.end(), cmp);
	};
	int get(const std::string& name) const {
		for (int i = 0; i < vec.size(); i++) {
			if (vec[i].name == name) {
				return vec[i].value;
			}
		}
		throw VarNameExcep("Error: Variable \"" + name + "\" is not defined");
	};
	void set(const std::string name, int value) {
		for (Var& v : vec) {
			if (v.name == name) {
				v.value = value;
				return;
			}
		}
		add(name, value);
	};
	bool isExist(std::string name) const {
		for (Var v : vec) {
			if (v.name == name) {
				return true;
			}
		}
		return false;
	};
	void clear() {
		vec.clear();
	};
	int size() const {
		return vec.size();
	};
	std::pair<std::string, int> operator[](int index) const {
		return std::make_pair(vec[index].name, vec[index].value);
	};
};
