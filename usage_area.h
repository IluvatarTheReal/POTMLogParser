#pragma once

#include "result_line.h"

struct usage_area {
	usage_area(result_line& line) {
		auto lbound = line._text.find_first_of("]") + 2;
		auto rbound = line._text.find_first_of(")");
		if (rbound == std::string::npos) rbound = 97;
		name = line._text.substr(lbound, rbound - lbound + 1);
		std::stringstream ss(line._text.substr(rbound + 1));
		std::string temp;
		auto convert = [&](unsigned& i) {
			std::string temp;
			ss >> temp;
			if (temp == "-") i = 0;
			else i = std::stoi(temp.c_str());
		};
		auto convert_container = [&](std::vector<unsigned>& v) {
			std::string temp;
			ss >> temp;
			if (temp == "-") v.emplace_back((unsigned)0);
			else v.emplace_back(std::stoul(temp.c_str()));
		};
		convert(entries);
		convert_container(av_xp_buf);
		convert_container(av_lev);
		convert_container(av_part);
		convert(soloers);
		convert(rp);
		convert(fighting);
		convert(inactive);
		convert(pc_deaths);
	}
	int container_average(std::vector<unsigned>& v) {
		if (!v.empty() && entries != 0) return std::accumulate(v.begin(), v.end(), 0) / entries;
		else return 0;
	}
	bool operator==(const usage_area& rhs) {
		return name == rhs.name;
	}
	bool operator<(usage_area& rhs) {
		return name < rhs.name;
	}
	std::string name;
	unsigned entries;
	std::vector<unsigned> av_xp_buf;
	std::vector<unsigned> av_lev;
	std::vector<unsigned> av_part;
	unsigned soloers;
	unsigned rp;
	unsigned fighting;
	unsigned inactive;
	unsigned pc_deaths;
};

void print_report(std::ostream& out, tm& pre_time, tm& post_time, arg_container& args, std::vector<usage_area>& compiled_areas) {
	out << "COMPILED AREA USAGE REPORTS FROM ";
	if (!args[ARGS::POST]) out << "THE EARLIEST AVAILABLE";
	else out << std::put_time(&post_time, "%a %d %b %y@%H:%M:%S");
	out << " TO ";
	if (!args[ARGS::PRE]) out << "NOW";
	else out << std::put_time(&pre_time, "%a %d %b %y@%H:%M:%S");
	out << std::endl;
	out << "[AREA] Name" << '\t' << "Entries" << '\t' << "Average XP buffer" << '\t' << "Average Level" << '\t' << "Average Party Size" << '\t' << "Soloers" << '\t' << "Roleplaying" << '\t' << "Fighting" << '\t' << "Inactive" << '\t' << "PC Deaths" << std::endl;
	for (auto& it : compiled_areas) out << it.name << '\t' << it.entries << '\t' << it.container_average(it.av_xp_buf) << '\t' << it.container_average(it.av_lev) << '\t' << it.container_average(it.av_part) << '\t' << it.soloers << '\t' << it.rp << '\t' << it.fighting << '\t' << it.inactive << '\t' << it.pc_deaths << std::endl;
}
