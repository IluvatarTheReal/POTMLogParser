#pragma once
#include "utils.h"

class playerdata_container {
public:
	enum class type { ID, PNAME, CDKEY, IPADDR, CHARNAME };
	struct playerdata {
		std::vector<const char*> cdkeys = { };
		std::vector<const char*> charnames = { };
		std::vector<const char* > ids = { };
		std::vector<const char*> ips = { };
		std::vector<const char*> pnames = { };
	};
	playerdata_container(const char* str, type t, std::vector<const char*>& strings_parsed, playerdata& pdata);
	static std::vector<type> types();
	static std::vector<const char*>& get_field_vector(playerdata& data, type t);
	std::vector<playerdata>& data();
	void handle_line(std::string line);
	std::vector<const char*> get_remaining();
	void mark_checked();
private:
	playerdata read_data(std::string line);
	bool checked(std::string str);
	bool present(playerdata& pdata);
private:
	std::vector<playerdata> _data;
	std::vector<const char*>& _strings_parsed;
	playerdata& _pdata_to_search;
};