#include "playerdata_container.h"

playerdata_container::playerdata_container(const char* str, type t, std::vector<const char*>& strings_parsed, playerdata& pdata) : _strings_parsed(strings_parsed), _pdata_to_search(pdata) {
	get_field_vector(_pdata_to_search, t).emplace_back(str);
}

std::vector<playerdata_container::type> playerdata_container::types() {
	static std::vector<type> static_types_vector = { type::CDKEY, type::CHARNAME, type::ID, type::IPADDR, type::PNAME };
	return static_types_vector;
}

std::vector<const char*>& playerdata_container::get_field_vector(playerdata& data, type t) {
	switch (t) {
	case type::CDKEY: return data.cdkeys;
	case type::CHARNAME: return data.charnames;
	case type::ID: return data.ids;
	case type::IPADDR: return data.ips;
	default: return data.pnames;
	}
}

std::vector<playerdata_container::playerdata>& playerdata_container::data() {
	return _data;
}

void playerdata_container::handle_line(std::string line) {
	auto temp_data = read_data(line);

	if (!present(temp_data)) {
		_data.insert(_data.end(), temp_data);

		for (auto t : types()) {
			auto& vec_to_check = get_field_vector(_pdata_to_search, t);
			for (auto& str : get_field_vector(temp_data, t)) {
				if (!checked(str) && std::find(vec_to_check.begin(), vec_to_check.end(), str) == vec_to_check.end()) {
					vec_to_check.insert(vec_to_check.end(), str);
				}
			}
		}
	}
}

std::vector<const char*> playerdata_container::get_remaining() {
	std::vector<const char*> ret = {};
	for (auto t : types()) for (auto& str : get_field_vector(_pdata_to_search, t)) if (!checked(str)) ret.insert(ret.end(), str);
	return ret;
}

void playerdata_container::mark_checked() {
	for (auto t : types()) {
		for (auto& str : get_field_vector(_pdata_to_search, t)) {
			_strings_parsed.insert(_strings_parsed.end(), str);
		}
	}
}

playerdata_container::playerdata playerdata_container::read_data(std::string line) {
	std::vector<const char*> exclusions = { "<->", "<O>", "<->", "<X>", "<!>", "Login info: ", "gp to raise", "DM " };

	for (auto& it : exclusions) {
		if (line.find(it) != std::string::npos)
			return playerdata();
	}

	auto pos0 = line.find("]");
	auto pos1 = line.find("(ID:");
	if (pos1 == std::string::npos || pos0 == std::string::npos || pos0 > pos1) return playerdata();
	auto pos2 = line.find("@", pos1);
	auto pos3 = line.find(")", pos1);
	if (pos2 == std::string::npos || pos3 == std::string::npos) return playerdata();
	pos0++;
	line = line.substr(pos0, pos2 < pos3 ? pos2 - pos0 : pos3 - pos0);
	pos0 = line.find("(ID");

	//std::cout << "------------\n" << std::endl;

	playerdata data;
	std::string cname = line.substr(0, pos0);
	utils::trim(cname);
	data.charnames.emplace_back(cname.c_str());
	//std::cout << "Character: " << cname.c_str() << std::endl;

	line = line.substr(pos0 + 4);
	pos0 = line.find("/");
	data.ids.emplace_back(line.substr(0, pos0).c_str());
	//std::cout << "ID: " << line.substr(0, pos0).c_str() << std::endl;

	line = line.substr(pos0 + 1);
	pos0 = line.find("/");
	data.pnames.emplace_back(line.substr(0, pos0).c_str());
	//std::cout << "Playername: " << line.substr(0, pos0).c_str() << std::endl;

	line = line.substr(pos0 + 1);
	pos0 = line.find("/");
	data.cdkeys.emplace_back(line.substr(0, pos0).c_str());
	//std::cout << "CD: " << line.substr(0, pos0).c_str() << std::endl;

	data.ips.emplace_back(line.substr(pos0 + 1).c_str());
	//std::cout << "IP: " << line.substr(pos0 + 1).c_str() << std::endl;

	return data;
}

bool playerdata_container::checked(std::string str) {
	auto where = std::find(_strings_parsed.begin(), _strings_parsed.end(), str);
	if (where == _strings_parsed.end()) return false;
	return true;
}

bool playerdata_container::present(playerdata& pdata) {
	for (auto t : types()) {
		for (auto& result : get_field_vector(_pdata_to_search, t)) {
			for (auto& new_text : get_field_vector(pdata, t))
			{
				if (result == new_text)	
					return true;									
			}
		}
	}
	
	return false;
}