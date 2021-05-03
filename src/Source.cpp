#include <algorithm>
#include "playerdata_container.h"
#include "arg_container.h"
#include "usage_area.h"
#include "log_file_fetcher.h"

void print_parser_header();
void print_line(std::string line);
void print_crawling_results(playerdata_container& pc_data);
//bool vector_string_val_exist(std::vector<std::string>& the_vector, std::string the_value);

bool print_console = false;
bool print_file = true;

std::fstream outfile;


int main(int argc, char* argv[]) {
	//Load configuration
	arg_container args(argc, argv);

	print_parser_header();

	//Set up basic search containers.
	std::vector<const char*> parse_strings = {};
	//First argument is assumed to be a search string if present.
	std::string term = args.term();

	print_file = !args[ARGS::NF];
	print_console = args[ARGS::P] || args[ARGS::NF];

	if (term != "")
		parse_strings.emplace_back(term.c_str());

	//Initialize vector to hold strings we've already searched to prevent duplication
	std::vector<const char*> strings_parsed = {};

	//Can't run without args.
	if ((parse_strings.size() == 0 || parse_strings.at(0) == "") && !args[ARGS::AR]) {
		std::cout << "\nNo valid argument supplied, closing." << std::endl;
		return 0;
	}

	//Set up globals
	std::tm post_time;
	std::tm pre_time;

	if (args[ARGS::POST])
		post_time = utils::get_arg_time(args.val(ARGS::POST), utils::relative_time::after);

	if (args[ARGS::PRE])
		pre_time = get_arg_time(args.val(ARGS::PRE), utils::relative_time::before);

	//set up containers
	std::vector<result_line> results;
	std::vector<usage_area> usage_areas;

	//Check if we're crawling and set up appropriately.
	bool crawling_search = false;
	playerdata_container::type temp_type = playerdata_container::type::CDKEY;
	if (args[ARGS::PROF])
		crawling_search = true;
	else if (args[ARGS::PROF_CD]) { //Deprecated
		crawling_search = true;
	}
	else if (args[ARGS::PROF_CN]) { //Deprecated
		temp_type = playerdata_container::type::CHARNAME;
		crawling_search = true;
	}
	else if (args[ARGS::PROF_PN]) { //Deprecated
		temp_type = playerdata_container::type::PNAME;
		crawling_search = true;
	}
	else if (args[ARGS::PROF_IP]) { //Deprecated
		temp_type = playerdata_container::type::IPADDR;
		crawling_search = true;
	}
	playerdata_container::playerdata profiles_to_search;
	playerdata_container pc(parse_strings.size() > 0 ? parse_strings.at(0) : "", temp_type, strings_parsed, profiles_to_search);

	//open output file
	std::string name = utils::get_outfile_name();
	//std::fstream outfile;
	if (print_file) {
		outfile.open(name, std::fstream::out);
		if (outfile)
			std::cout << "\nOutput file \"" + name + "\" opened successfully." << std::endl;
		else
			std::cout << "\nOutput file \"" + name + "\" not opened." << std::endl;
	}
	else std::cout << "\nOutput redirected to browser, no file created." << std::endl;

	for (auto addition : args.get_cumulative_params(ARGS::A))
		parse_strings.insert(parse_strings.end(), addition);

	//Any term here will be validated along with the primary term, one of these or the primary term must be present during the search.	
	std::vector<const char*> orTerm = args.get_cumulative_params(ARGS::O);

	std::vector<const char*> exclusions = args.get_cumulative_params(ARGS::E);

	//Iluvatar : Doing this prevent any kind of research for the crawling, why was it added?
	//we've consumed the search term, now discard it. 
	/*if (crawling_search)
		parse_strings.clear();*/

	int files_processed = 0;
	int file_count = 0;

	log_file_fetcher files_fetcher(args[ARGS::LOCAL] ? log_file_fetcher::environment_type::LOCAL : log_file_fetcher::environment_type::SERVER);
	files_fetcher.find_files();

	file_count = files_fetcher.get_files_count();
	std::cout << "\n" << file_count << " files found.\n" << std::endl;


	//Initialize number of search terms
	std::vector<const char*> remaining = {};
	std::vector<const char*> current_pass;
	int loopsize = parse_strings.size() + remaining.size();

	if (args[ARGS::AR])
		loopsize = 1;

	//Print information about search if this isn't an area report.
	if (!args[ARGS::AR]) {
		std::vector<const char*> temp_strings = {};

		for (auto& it : parse_strings)
			temp_strings.insert(temp_strings.end(), it);
		for (auto& it : remaining)
			temp_strings.insert(temp_strings.end(), it);

		int size = temp_strings.size();
		std::cout << (crawling_search ? "Crawling " : "Parsing ") << " files for " << (size > 1 ? "strings " : "string ");
		for (int x = 0; x < size; ++x)
			std::cout << "\"" << temp_strings[x] << (size > (x + 1) ? "\", " : "\".\n");

		if (orTerm.size() > 0) {
			std::cout << "OR ";
			for (int x = 0; x < orTerm.size(); ++x)
				std::cout << "\"" << orTerm[x] << (orTerm.size() > (x + 1) ? "\", " : "\".\n");
		}

		for (auto& it : parse_strings)
			current_pass.insert(current_pass.end(), it);

		std::cout << std::endl;
	}
	else
		std::cout << "Parsing files for area usage reports.\n" << std::endl;

	//Main parse loop.
	//for (const auto& it : fs::recursive_directory_iterator(Path)) {
	for (const auto& it : files_fetcher.get_log_files_path()) { //This way we avoid looking every single file path again and only check the one we know are the log files we want.
		//files_fetcher

		//Grab first file.
		std::string filepath = it.filename().string();
		//Make sure we're not trying to parse a parsed log. That way lies insanity. If we are, skip!
		if (filepath.find(OUTFILE_STRING) != std::string::npos) continue;

		//Identify the target file and make sure it's the right kind of file.
		auto pos = filepath.find_last_of(".");
		if (pos == std::string::npos || filepath.substr(pos) != ".txt" || filepath.find("nwserverLog") == std::string::npos) continue;

		//Open the file and skip if the file doesn't open.
		std::fstream file_to_parse;
		if (!file_to_parse.is_open())
			file_to_parse.open(it.string());
		if (!file_to_parse) {
			std::cout << "File failed to open: " << it.string() << std::endl;
			continue;
		}

		//Print update for user for current search.
		files_processed++;
		auto num = (int)(100 * (float)files_processed / (float)file_count);
		std::cout << "Parsing file: " << it.string() << ". " << num << "% complete. " << utils::progress_bar(num) << '\r' << std::flush;

		//Get some file information
		std::tm log_created_time = utils::get_time_from_file_path(it);
		int highest_month = 0;

		//Advances as long as a valid line can be read in.
		for (result_line line; line.advance(file_to_parse); ) {
			//If text searching and the line doesn't conform, skip.
			if (!line.check_line(current_pass, exclusions))
			{
				bool found_match = false;
				//Test the or term ONE BY ONE (Multiple OR cannot be supplied through command line yet, but this code will support it in the future)
				for (int i = 0; i < orTerm.size(); i++)
				{
					std::vector<const char*> current_orTerm = { orTerm[i] };
					if (line.check_line(current_orTerm, exclusions)) {
						found_match = true;
					}
				}

				if (!found_match)
					continue;
			}

			line._time.tm_year = log_created_time.tm_year;
			int mon = line._time.tm_mon;
			if (mon < highest_month) {
				line._time.tm_year++;
				highest_month = mon;
			}
			else if (mon > highest_month) highest_month = mon;
			bool line_good = true;

			//If not an area report, process line for search
			if (!args[ARGS::AR]) {
				if (line.time_good || args[ARGS::ND]) {
					if (args[ARGS::POST] && std::mktime(&line._time) < std::mktime(&post_time))
						line_good = false;

					if (args[ARGS::PRE] && std::mktime(&line._time) > std::mktime(&pre_time))
						line_good = false;

					//If line matches parameters, deal with it
					if (line_good) {
						//If no output is not selected, add the line to Results to print later.
						if (!args[ARGS::NOO]) results.emplace_back(line);
						//If crawling search is indicated, handle it.
						if (crawling_search) {
							pc.handle_line(line._text);
						}

					}
				}
			}
			//If area report, process data separately
			else {
				if (line._text.substr(0, 6) == "[AREA]" && line._text.substr(0, 11) != "[AREA] Name") {
					if (args[ARGS::POST] && std::mktime(&log_created_time) < std::mktime(&post_time)) line_good = false;
					if (args[ARGS::PRE] && std::mktime(&log_created_time) > std::mktime(&pre_time)) line_good = false;
					if (line_good) usage_areas.emplace_back(line);
				}
			}
		}
		//Close file. Technically should happen automatically when it goes out of scope, but we take no risks here.
		file_to_parse.close();
	}

	//Process results
	if (!args[ARGS::AR]) {
		std::cout << "\nSorting results..." << std::endl;
		if (results.empty() && !args[ARGS::NOO])
			outfile << "No valid results found." << std::endl;
		else {
			std::sort(results.begin(), results.end(), [](result_line& lhs, result_line& rhs) {	return lhs < rhs;	});
			if (print_console && (results.empty() && !args[ARGS::AR]))
				std::cout << "No valid results found." << std::endl;
			else {
				for (auto& it : results) {
					if (it._text.substr(0, 1) == ".")
						it._text = it._text.substr(1);
					else {
						if (!args[ARGS::NOO]) {
							print_line(it._text);
						}
					}
				}
			}

			if (crawling_search) {
				print_crawling_results(pc);
			}
		}
	}

	if (args[ARGS::AR]) {
		std::cout << "\nProcessing area reports...\n";
		std::vector<usage_area> compiled_areas;

		for (auto& it : usage_areas) {
			auto f = std::find(compiled_areas.begin(), compiled_areas.end(), it);
			if (f != compiled_areas.end()) {
				if (f->name == it.name) {
					f->av_lev.emplace_back(it.av_lev.at(0) * it.entries);
					f->av_part.emplace_back(it.av_part.at(0) * it.entries);
					f->av_xp_buf.emplace_back(it.av_xp_buf.at(0) * it.entries);
					f->entries += it.entries;
					f->fighting += it.fighting;
					f->inactive += it.inactive;
					f->pc_deaths += it.pc_deaths;
					f->rp += it.rp;
					f->soloers += it.soloers;
				}
			}
			else compiled_areas.emplace_back(it);
		}
		if (usage_areas.empty()) std::cout << "No results found." << std::endl;
		else
		{
			std::cout << "Sorting results..." << std::endl;
			std::sort(compiled_areas.begin(), compiled_areas.end(), [](usage_area& lhs, usage_area& rhs) {
				return (lhs < rhs);
				});
			if (!args[ARGS::NF]) print_report(outfile, pre_time, post_time, args, compiled_areas);
			if (args[ARGS::P] || args[ARGS::NF]) print_report(std::cout, pre_time, post_time, args, compiled_areas);
		}
	}

	if (!args[ARGS::NF])
		std::cout << "Operation complete (see file " << name << ")." << std::endl;
	else
		std::cout << "Operation complete." << std::endl;
}


void print_line(std::string line) {

	if (print_console)
		std::cout << line << std::endl;
	if (print_file)
		outfile << line << "\r\n";
	//Handle adding line to console output/file

}

void print_crawling_results(playerdata_container& pc_data) {
	print_line("------------------------------------------");
	print_line("\nPlayerdata Report:");
	print_line("\nBy player names:");

	std::vector<std::string> printed_player_name;
	for (auto& i : pc_data.player_lines)
	{
		//find_cd_key_for

		if (i.character_name != "" && !utils::vector_string_val_exist(printed_player_name, i.player_name)) {

			std::string cd_keys_string = "";
			std::vector<std::string> playername_cd_keys = pc_data.find_cd_key_for(i.player_name, playerdata_container::type::PNAME);
			for (auto& j : playername_cd_keys) {
				if (cd_keys_string == "")
					cd_keys_string += j;
				else
					cd_keys_string += ", " + j;
			}

			print_line("    " + i.player_name + "(" + cd_keys_string + ")" + "");
			printed_player_name.push_back(i.player_name);


			std::vector<std::string> playername_characters = pc_data.find_data_type_for(i.player_name, playerdata_container::type::PNAME, playerdata_container::type::CHARNAME);
			for (auto& character : playername_characters)
			{
				if (character.length() > 1) {
					cd_keys_string = "";
					playername_cd_keys = pc_data.find_cd_key_for(character, playerdata_container::type::CHARNAME);
					for (auto& j : playername_cd_keys) {
						if (cd_keys_string == "")
							cd_keys_string += j;
						else
							cd_keys_string += ", " + j;
					}

					print_line("        " + character + "(" + cd_keys_string + ")" + "");
				}
			}
		}
	}

	//Playerdata report

	//  PlayerName 1 (CD_Key 1, CD_Key 2)  //Here we could guess that CD_Key 2 was use to peek at the player's vault
	//		Char 1 (CD_Key 1)
	//		Char 2 (CD_Key 1)
	//  PlayerName 2(CD_Key 3, CD_Key 4) //Here we could guess that CD_Key 4 was use to peek at the player's vault and to log on character 5
	//		Char 3 (CD_Key 3 )
	//		Char 4 (CD_Key 3)
	//		Char 5 (CD_Key 3, CD_Key 4)


	print_line("\n------------------------------------------");
	print_line("\nData lines:");

	for (auto& i : pc_data.player_lines) {
		print_line("\n");
		print_line("CD Key: " + i.cd_key);
		print_line("IP: " + i.ip);
		print_line("Playername: " + i.player_name);
		print_line("Character name: " + i.character_name);
		print_line("ID: " + i.id);
	}

	print_line("------------------------------------------");
}

void print_parser_header() {

	std::cout << "\n       **************************\n";
	std::cout << "       *     LOG PARSER V" << VER_NUM << "    *\n";
	std::cout << "       **************************\n" << std::endl;
	std::cout << "                by Arawn\n" << std::endl;

}


//bool vector_string_val_exist(std::vector<std::string>& the_vector, std::string the_value) {
//
//	if (std::find(the_vector.begin(), the_vector.end(), the_value) != the_vector.end())
//		return true;
//
//	return false;
//}

