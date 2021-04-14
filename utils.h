#pragma once
#include <string>
#include <chrono>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cstdlib>
#include <iomanip>


//This is a kludge to work around the inability to test the standard of C++ supported on the server; otherwise I'd have to make manual changes every time I uploaded it.
#ifdef _WIN32
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

//constant definitions
constexpr const char* OUTFILE_STRING = "synthetic_log";
constexpr int BARS = 50;
constexpr float VER_NUM = 1.6f;
constexpr const char* ws = " \t\n\r\f\v";

namespace utils {
	inline void rtrim(std::string& s, const char* t = ws) {
		s.erase(s.find_last_not_of(t) + 1);
	}

	inline void ltrim(std::string& s, const char* t = ws) {
		s.erase(0, s.find_first_not_of(t));
	}

	// trim from both ends of string (right then left)
	inline void trim(std::string& s, const char* t = ws) {
		ltrim(s);
		rtrim(s);
	}

	inline std::string get_outfile_name() {
		std::string name = OUTFILE_STRING;
		int parse = 0;
		while (fs::exists(name + "_" + std::to_string(parse) + ".txt")) parse++;
		return name + "_" + std::to_string(parse) + ".txt";
	}

	inline std::tm get_time_from_directory_name(std::string str) {
		std::tm t = {};
		str = str.substr(0, str.find_first_of('@'));
		std::istringstream ss(str);
		ss >> std::get_time(&t, "%d-%m-%y");
		if (ss.fail()) std::cout << "Failed to extract time from directory name." << std::endl;
		return t;
	}

	inline std::tm get_time_from_file_path(const fs::path& p) {
		std::string s = p.string();
		auto pos2 = s.find_last_of('\\');
		if (pos2 == std::string::npos) pos2 = s.find_last_of('/');
		auto pos1 = s.find_last_of('\\', pos2 - 1);
		if (pos1 == std::string::npos) pos1 = s.find_last_of('/', pos2 - 1);
		s = s.substr(pos1 + 1, pos2 - pos1 - 1);
		return get_time_from_directory_name(s);
	}

	inline std::string extract_player_name(std::string str) {
		auto pos1 = str.find_first_of("]");
		auto pos2 = str.find_first_of("(");
		return str.substr(pos1 + 2, pos2 - pos1 - 3);
	}

	inline std::string progress_bar(int percent) {
		std::string ret = "[";
		int num_equals = (int)((float)BARS * (percent / 100.0f));
		if (num_equals < 0) num_equals = 0;
		for (int i = 0; i < num_equals; ++i) ret += "=";
		ret += ">";
		for (int i = 0; i < (BARS - num_equals); ++i) ret += " ";
		ret += "]";
		return ret;
	}

	enum class relative_time {before, after};

	inline std::tm get_arg_time(std::string str, relative_time rt) {
		std::tm t = {};
		std::istringstream ss(str);

		if (str.find('@') != std::string::npos) ss >> std::get_time(&t, "%d-%m-%y@%H:%M:%S");
		else {
			ss >> std::get_time(&t, "%d-%m-%y");
			if (rt == relative_time::before) {
				t.tm_hour = 23;
				t.tm_min = 59;
				t.tm_sec = 59;
			}
		}
		if (ss.fail()) std::cout << "Invalid date entered." << std::endl;
		return t;
	}


}