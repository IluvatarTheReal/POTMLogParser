#pragma once
#include <map>
#include <string>
#include <iostream>
#include <vector>

namespace ARGS {
	constexpr const char* POST = "post";
	constexpr const char* PRE = "pre";
	constexpr const char* AR = "ar";
	constexpr const char* PROF_CD = "prof_cd";
	constexpr const char* PROF_CN = "prof_cn";
	constexpr const char* PROF_PN = "prof_pn";
	constexpr const char* PROF_IP = "prof_ip";
	constexpr const char* NF = "nf";
	constexpr const char* CONF = "conf";
	constexpr const char* A = "a";
	constexpr const char* E = "e";
	constexpr const char* NOO = "noo";
	constexpr const char* ND = "nd";
	constexpr const char* P = "p";
}

class arg_container {
public:
	arg_container(int argc, char* argv[]);
	bool operator[](std::string str);
	std::string val(std::string str);
	unsigned size();
	std::vector<const char*> get_cumulative_params(std::string str);
	void print();
	std::string term();
private:
	std::map<std::string, std::string> _vals;
	bool _crawl = false;
	std::string _primary_term;
};