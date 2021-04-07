#include "arg_container.h"

arg_container::arg_container(int argc, char* argv[]) {
	if (argc < 2) {
		std::cout << "No arguments supplied." << std::endl;
		return;
	}
	unsigned count = 1;
	if (std::string(argv[1]).substr(0, 1) != "-") {
		_primary_term = std::string(argv[1]);
		++count;
	}
	std::string last_flag = "";
	for (; count < argc; ++count) {
		std::string temp = argv[count];
		if (temp.substr(0, 1) == "-") {
			if (temp.length() < 2) {
				std::cout << "Ill-formed flag detected." << std::endl;
				return;
			}
			temp = temp.substr(1);
			if (last_flag != "") _vals.emplace(last_flag, "");
			last_flag = temp;
		}
		else if (last_flag == "") {
			std::cout << "Ill-formed flag detected." << std::endl;
			return;
		}
		else {
			_vals.emplace(last_flag, temp);
			last_flag = "";
		}
	}
	if (last_flag != "") _vals.emplace(last_flag, "");
}

bool arg_container::operator[](std::string str) {
	return _vals.find(str) != _vals.end();
}

std::string arg_container::val(std::string str) {
	return ((*this)[str])? _vals.at(str) : "";
}

unsigned arg_container::size() {
	return _vals.size();
}

std::vector<const char*> arg_container::get_cumulative_params(std::string str) {
	if (!(*this)[str]) return {};
	std::vector<const char*> ret = {};
	for (auto& item : _vals) if (item.first == str) ret.insert(ret.end(), item.second.c_str());
	return ret;
}

void arg_container::print() {
	std::cout << "\nArgs Dump:\n";
	std::cout << "Primary Term: " << term() << "\n";
	for (auto& ref : _vals) {
		std::cout << "\n" << ref.first;
		if (ref.second == "") std::cout << " without a param";
		else std::cout << " with param " << ref.second;
	}
	std::cout << std::endl;
}

std::string arg_container::term() {
	return _primary_term;
}
