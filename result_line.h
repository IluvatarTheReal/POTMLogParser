#pragma once
#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <iomanip>
#include <sstream>
#include <vector>

struct result_line {
	std::istream& advance(std::istream& f) {
		auto& proc = std::getline(f, _text);
		extract_time();
		return proc;
	}
	void extract_time() {
		std::tm t = {};
		auto pos1 = _text.find_first_of("[");
		auto pos2 = _text.find_first_of("]");
		if (pos1 != std::string::npos && pos2 != std::string::npos) {
			std::string datestring = _text.substr(pos1 + 1, pos2 - pos1 - 1);
			auto pos = datestring.find("  ");
			if (pos != std::string::npos) datestring.replace(pos, 2, " 0");
			std::istringstream ss(datestring);
			ss >> std::get_time(&t, "%a %b %d %H:%M:%S");
			if (!ss.fail()) time_good = true;
			else time_good = false;
		}
		else time_good = false;
		_time = t;
	}
	bool operator<(result_line& rhs) {
		return mktime(&_time) < mktime(&(rhs._time));
	}
	bool check_line(std::vector<const char*>& search_terms, std::vector<const char*> exclusions) {
		for (auto& it : exclusions) if (_text.find(it) != std::string::npos) return false;
		if (search_terms.size() > 0) {
			bool found = true;
			for (auto& it : search_terms) if (_text.find(it) == std::string::npos) found = false;
			return found;
		}
		return false;
	}

	std::string _text = "";
	std::tm _time;
	bool time_good = false;
};