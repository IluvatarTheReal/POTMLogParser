#include "log_file_fetcher.h"

log_file_fetcher::log_file_fetcher(environment_type _environment) {
	environment = _environment;
}

log_file_fetcher::~log_file_fetcher() { }


log_file_fetcher::environment_type log_file_fetcher::get_environement() const {
	return environment;
}

std::vector<fs::path> log_file_fetcher::get_log_files_path() const {
	return log_files_path;
}

int log_file_fetcher::get_files_count() {
	return log_files_path.size();
}

void log_file_fetcher::find_files() {
	for (const auto& it : fs::recursive_directory_iterator(fs::current_path())) {

		auto filepath = it.path().filename().string();
		auto pos = filepath.find_last_of(".");
		if (pos == std::string::npos || filepath.substr(pos) != ".txt" || filepath.find(log_file_base_name) == std::string::npos)
			continue;
		else
			log_files_path.push_back(it.path());
	}
}