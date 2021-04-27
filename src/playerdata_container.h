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
		std::string player_name;
		std::string cd_key;
		std::string id;
		std::string character_name;
		std::string ip;
	};
	std::vector<playerdata> player_lines;
	playerdata_container(const char* str, type t, std::vector<const char*>& strings_parsed, playerdata& pdata);
	static std::vector<type> types();
	static std::vector<const char*>& get_field_vector(playerdata& data, type t);
	static std::string get_field(playerdata& data, type t);
	std::vector<playerdata>& data();
	void handle_line(std::string line);
	std::vector<std::string> find_cd_key_for(std::string field_value, type t_field);
	std::vector<std::string> find_data_type_for(std::string field_value, type t_field, type return_t_field);
	std::vector<const char*> get_remaining();
	void mark_checked();
private:
	playerdata read_data(std::string line);	
	bool exist(playerdata& player_data);
	bool checked(std::string str);
	playerdata get_joined_player_data(std::string line);
private:
	std::vector<playerdata> _data;
	std::vector<const char*>& _strings_parsed;
	playerdata& _pdata_to_search;
};