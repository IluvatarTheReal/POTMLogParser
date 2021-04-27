#include "playerdata_container.h"
#include <regex>

playerdata_container::playerdata_container(const char* str, type t, std::vector<const char*>& strings_parsed, playerdata& pdata) : _strings_parsed(strings_parsed), _pdata_to_search(pdata) {
	get_field_vector(_pdata_to_search, t).emplace_back(str);
}

std::vector<playerdata_container::type> playerdata_container::types() {
	static std::vector<type> static_types_vector = { type::CDKEY, type::CHARNAME, type::ID, type::IPADDR, type::PNAME };
	return static_types_vector;
}

//OBSELETE
std::vector<const char*>& playerdata_container::get_field_vector(playerdata& data, type t) {
	switch (t) {
	case type::CDKEY: return data.cdkeys;
	case type::CHARNAME: return data.charnames;
	case type::ID: return data.ids;
	case type::IPADDR: return data.ips;
	default: return data.pnames;
	}
}

std::string playerdata_container::get_field(playerdata& data, type t) {
	switch (t) {
	case type::CDKEY: return data.cd_key;
	case type::CHARNAME: return data.character_name;
	case type::ID: return data.id;
	case type::IPADDR: return data.ip;
	default: return data.player_name;
	}
}

std::vector<playerdata_container::playerdata>& playerdata_container::data() {
	return _data;
}

void playerdata_container::handle_line(std::string line) {

	playerdata temp_data = read_data(line);

	if (!exist(temp_data)) {

		/*std::cout << "------------------------------------------------------" << std::endl;
		std::cout << "CD Key: " << temp_data.cd_key << std::endl;
		std::cout << "IP: " << temp_data.ip << std::endl;
		std::cout << "Playername: " << temp_data.player_name << std::endl;
		std::cout << "Character name: " << temp_data.character_name << std::endl;
		std::cout << "ID: " << temp_data.id << std::endl;*/



		player_lines.push_back(temp_data);
	}

}

playerdata_container::playerdata playerdata_container::read_data(std::string line) {
	std::vector<const char*> exclusions = { "<->", "<O>", "<I>", "<X>", "<!>", "Login info: ", "gp to raise", "DM " };

	//If the line is about a player login on a vault, read the line differently.
	if (line.find("Joined as Player") != std::string::npos)
		return get_joined_player_data(line);

	for (auto& it : exclusions) {
		if (line.find(it) != std::string::npos)
			return playerdata_container::playerdata();
	}

	//TODO Use regex to find information

	auto pos0 = line.find("]");
	auto pos1 = line.find("(ID:");
	if (pos1 == std::string::npos || pos0 == std::string::npos || pos0 > pos1)
		return playerdata_container::playerdata();

	auto pos2 = line.find("@", pos1);
	auto pos3 = line.find(")", pos1);
	if (pos2 == std::string::npos || pos3 == std::string::npos)
		return playerdata_container::playerdata();

	pos0++;
	line = line.substr(pos0, pos2 < pos3 ? pos2 - pos0 : pos3 - pos0);
	pos0 = line.find("(ID");


	playerdata_container::playerdata data;
	//std::cout << "------------\n" << std::endl;	

	std::string cname = line.substr(0, pos0);
	utils::trim(cname);
	data.charnames.emplace_back(cname.c_str());
	//std::cout << "Character: " << cname.c_str() << std::endl;
	data.character_name = line.substr(0, pos0).c_str();

	line = line.substr(pos0 + 4);
	pos0 = line.find("/");
	data.ids.emplace_back(line.substr(0, pos0).c_str());
	//std::cout << "ID: " << line.substr(0, pos0).c_str() << std::endl;
	data.id = line.substr(0, pos0).c_str();

	line = line.substr(pos0 + 1);
	pos0 = line.find("/");
	data.pnames.emplace_back(line.substr(0, pos0).c_str());
	//std::cout << "Playername: " << line.substr(0, pos0).c_str() << std::endl;
	data.player_name = line.substr(0, pos0).c_str();

	line = line.substr(pos0 + 1);
	pos0 = line.find("/");
	data.cdkeys.emplace_back(line.substr(0, pos0).c_str());
	//std::cout << "CD: " << line.substr(0, pos0).c_str() << std::endl;
	data.cd_key = line.substr(0, pos0).c_str();

	data.ips.emplace_back(line.substr(pos0 + 1).c_str());
	//std::cout << "IP: " << line.substr(pos0 + 1).c_str() << std::endl;
	data.ip = line.substr(pos0 + 1).c_str();

	return data;
}


playerdata_container::playerdata playerdata_container::get_joined_player_data(std::string line) {
	playerdata_container::playerdata data;

	//For CD Key in Joined as player line
	//(?<=\()([[:alnum:]]{8})(?=\) Joined as Player)
	std::regex regex_cd_key("(.{8})(?=\\) Joined as Player)", std::regex_constants::ECMAScript);
	std::regex regex_player_name("](.*)(?=\\s\\(.{8}\\))", std::regex_constants::ECMAScript);

	std::smatch match;

	std::regex_search(line, match, regex_cd_key);
	for (auto v : match)
		data.cd_key = v;

	if (std::regex_search(line, match, regex_player_name)) {
		std::string val = match[1];
		//std::cout << "Before \"" << val << "\"" << std::endl;
		val.erase(0, 1);
		//std::cout << "Between \"" << val << "\"" << std::endl;
		/*if (val.length() > 1)
			val.erase((val.length() - 1), (val.length()));*/
		data.player_name = val;
		//std::cout << "After \"" << val << "\"" << std::endl;
	}

	return data;
}


bool playerdata_container::exist(playerdata& player_data) {

	for (auto& line : player_lines) {
		int matching_fields = 0;

		for (auto t : types()) {
			std::string new_result = get_field(player_data, t);
			std::string old_result = get_field(line, t);

			if (new_result == old_result)
				matching_fields++;
		}

		if (matching_fields == 5)
			return true;
	}

	return false;
}

std::vector<std::string> playerdata_container::find_cd_key_for(std::string field_value, playerdata_container::type t_field) {
	std::vector<std::string> cd_keys;

	for (auto& line : player_lines)
		if (field_value == get_field(line, t_field) && !utils::vector_string_val_exist(cd_keys, line.cd_key))
			cd_keys.push_back(line.cd_key);

	return cd_keys;
}

std::vector<std::string> playerdata_container::find_data_type_for(std::string field_value, playerdata_container::type t_field, playerdata_container::type return_t_field) {
	std::vector<std::string> field_values;

	for (auto& line : player_lines)
		if (field_value == get_field(line, t_field) && !utils::vector_string_val_exist(field_values, get_field(line, return_t_field)))
			field_values.push_back(get_field(line, return_t_field));

	return field_values;
}






//Obselete yet still required as its used to control the amount of search loop done for some obscure reason.
//Do not remove yet.

std::vector<const char*> playerdata_container::get_remaining() {
	std::vector<const char*> ret = {};
	for (auto t : types()) for (auto& str : get_field_vector(_pdata_to_search, t)) if (!checked(str)) ret.insert(ret.end(), str);
	return ret;
}

bool playerdata_container::checked(std::string str) {
	auto where = std::find(_strings_parsed.begin(), _strings_parsed.end(), str);
	if (where == _strings_parsed.end()) return false;
	return true;
}

void playerdata_container::mark_checked() {
	for (auto t : types()) {
		for (auto& str : get_field_vector(_pdata_to_search, t)) {
			_strings_parsed.insert(_strings_parsed.end(), str);
		}
	}
}