
# Log Compiler
![GitHub release (latest by date)](https://img.shields.io/github/v/release/IluvatarTheReal/POTMLogParser) ![GitHub Workflow Status (branch)](https://img.shields.io/github/actions/workflow/status/IluvatarTheReal/POTMLogParser/.github/workflows/build.yml?branch=master) ![GitHub issues](https://img.shields.io/github/issues/IluvatarTheReal/POTMLogParser)   
![](https://img.shields.io/badge/platform-linux-lightgrey)  

Original design by Arawn

## I. Introduction
The Log Parser is a utility designed to work in concert with Ravenloft: Prisoners of the Mist's NWServer setup. Due to the specific configuration of directories
and log entries on PotM's server, this utility will not work out-of-the-box with any other server's backend.

## II. Usage
The core utility is run by using the command ./log_parser in the console window to open the executable provided (by default in the logs_ee directory). The 
utility will not work unless at least one search term is provided. If this is a single word, it can be provided without quotes, such as:
```bash
./log_parser bluebomber
```
If the search term contains any spaces, it must be enclosed in quotation marks, like this:
```bash
./log_parser "ate the corpse of"
```
A number of flags are available that will modify the behavior of the executable. Please note that several are experimental and may produce unexpected results. If a
flag requires an argument, simply include it after the flag, with the same caveat that it must be enclosed in quotation marks if it is more than one word.

So:
```bash
./log_parser "DM EO" -nf
```
Or:
```bash
./log_parser "DM EO" -a "DM Arawn" -p
```
The order in which flags are included does not matter; the only constant is that the first argument provided must be a search term.

Here is a (hopefully complete) list of flags and a brief description:

**-post**  
With an argument, only returns results after a given date (in the format DD MM YY). Requires an argument.

**-pre**  
With an argument, only returns results before a given date (in the format DD MM YY). Requires an argument.

**-ar**  
Switches the utility to area report mode. Note that most of this functionality is automated and will disregard most other flags (except -post and -pre).

**-prof**  
Specifies that a crawling search is to be made with the search result.

**-prof_cd (deprecated)**  
Specifies that the provided primary term is a cd key for crawling search.

**-prof_cn (deprecated)**  
Specifies that the provided primary term is a character name for crawling search.

**-prof_pn (deprecated)**  
Specifies that the provided primary term is a player name for crawling search.

**-prof_ip (deprecated)**  
Specifies that the provided primary term is an ip address for crawling search.

**-nf**  
Do not generate an output file. Print output to console.

**-conf**  
Sets the confidence level (number of steps away from origin) for crawling search.

**-a**  
Specifies an additional search term that MUST be found in a given line. Requires an argument.

**-o**  
Specifies an additional search term the parser will try to match, it act as a OR condition.

**-e**  
Exclude all lines containing this term. Requires an argument.

**-noo**  
Discard all standard output; this can be used to print only the results of crawling search.

**-nd**  
Include lines not containing a valid timestamp (Discarded by default as noise).

**-p**  
Print output to the console as well as generating an output file. Mutually exclusive with -nf.

## III. Crawling Search
Crawling search is an experimental functionality that allows this utility to compile a list of all player accounts, cd-keys, ips, and characters associated with a
given player, identified by any one of the aforementioned pieces of information. It does this by repeatedly iterating over the logs, pulling all player data from
any profile that matches at least one data point, then iterating again over any non-matches. This is experimental functionality and not guaranteed to be accurate.

## IV. Updates and setup
This shell script can be used to automatically look for the a new version of the parser and compile it or setup the parser for a first use. The easiest way is to create the shell script in the directory containing the log files (or where the log parser is to be executed) and launch the it. It will either clone or pull the latest version of the parser and compiler it. Should any compiling error happen they will be printed in the console. Please relay them directly to Iluvatar or create an issue ticket detailing the problem on the github repository of this project.

```shell
#!/bin/sh

echo "Looking for log_parser source code..."

DIR="POTMLogParser/"
if [ -d "$DIR" ]; then
    echo "\e[1;32mSource code found!\e[1;0m 
Pulling latest version from github..."
		  
	cd POTMLogParser
	git pull
	cd ..
else
	echo "\e[1;31mNo source code found.\e[1;0m
Cloning repository from github..."
		  
	git clone https://github.com/IluvatarTheReal/POTMLogParser
fi

cd POTMLogParser
echo "\e[1;36mBuilding log_parser source code...\e[1;0m"
make
cd ..


PARSER=log_parser
if test -f "$PARSER"; then
    echo "An old version of the log_parser was found.
Deleting previous log_parser..."
	rm log_parser	
fi

echo ""

cp POTMLogParser/log_parser log_parser
sudo chmod +x log_parser

echo "log_parser ready to use! Use the log_parser this way :
./log_parser <argument>"

```


## IV. Updates (obselete)
Before running the utility, it is often a good idea to check if a new version is available. A batch script to automatically update the utility is included. Simply
run ./update_parser in the same directory as the compiled executable to execute the script and automatically update the parser. Any errors in compiling will be
displayed in the console window. If this occurs, please relay them to Arawn directly.


