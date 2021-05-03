#pragma once
#include "utils.h"

#ifndef LOG_FILE_FETCHER_H
#define LOG_FILE_FETCHER_H

class log_file_fetcher {
public:
	enum class environment_type { SERVER, LOCAL };
	log_file_fetcher(environment_type _environment);
	~log_file_fetcher();

	environment_type get_environement() const;
	std::vector<fs::path> get_log_files_path() const;

	int get_files_count();
	void find_files();
private:
	std::string const log_file_base_name = "nwserverLog";
	environment_type environment;
	std::vector<fs::path> log_files_path;
};

#endif