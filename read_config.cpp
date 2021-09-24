#include "read_config.h"
#include "mylib.h"
#include <cctype>
#include <iostream>
#include <filesystem>
#include <cassert>

using namespace std;
using namespace filesystem;

const path Config::HOME_CONFIG_FILE = path(".recipes");
const string Config::CONFIG_ENV_VAR = "RECIPE_CONFIG";

/*Replace environment variables with their values
 * in value*/
string repEnvVars(string value)
{
	string::size_type dollarPos;
	while ((dollarPos = value.find("$")) != string::npos)
	//while there's an env variable in value
	{
		//env vars only allowed to contain
		//numbers, letters, and _.  Find the end of the env variable.
		string::size_type slashPos = dollarPos + 1;  //will eventually contain
			//	character after env variable
		while (slashPos != string::npos)
		{
			char c = value[slashPos];
			if (!isalnum(c) && c != '_') break;
			++slashPos;
		}
		string env_var = (slashPos == string::npos)?
			value.substr(dollarPos+1) :
			value.substr(dollarPos+1, slashPos - (dollarPos + 1));
		string env_var_value = getenv(env_var);
		value.replace(dollarPos, slashPos, env_var_value);
	}
	return value;
}

bool Config::openConfigFile(string envVal, bool useHomeConfigFile, ifstream& strm)
{
	if (envVal == "") return false;
	else
	{
		path envPath;  //A path for the env variable's value
		try{
			envPath = path(envVal);
		}
		catch(filesystem_error &){
			//envVal was not a valid path name
			return false;
		}
		
		if (useHomeConfigFile)
		//if we're to append the file name to the home directory
		//specified in envPath
			envPath /= Config::HOME_CONFIG_FILE;
		strm.open(envPath);
		if (!strm) return false;  //if it didn't open
		else return true;
	}
}

//Load the configuration from a configuration file
Config::Config()
{
}

void Config::init()
{
	char* homeEnv = getenv("HOME");
	if (homeEnv == NULL) {
		homeEnv = getenv("USERPROFILE");
	}
	ifstream homeFile, envFile;
	//if $HOME is defined, first check for $HOME/.recipes
	if (homeEnv != NULL && openConfigFile(homeEnv, true, homeFile)) {
		loadConfig(homeFile);
		return;
	}
	//otherwise, check for $RECIPE_CONFIG
	else if (char* configEnv = getenv("RECIPE_CONFIG")) {
		if (openConfigFile(configEnv, false, envFile)) {
			loadConfig(envFile);
			return;
		}
	}
	//We don't throw here because you can't catch an exception if a static
	//initialization throws it
	cerr << "Couldn't find configuration file.\n" 
		"It must be in either $HOME/.recipes or $" << CONFIG_ENV_VAR << ".\nExiting.\n";
	exit(1);
}

void Config::loadConfig(ifstream& configFile)
{
	string ln;
	while (getline(configFile, ln))
	//for each line ln in the file
	{
		if (ltrim(ln).substr(0, 1) == "#") continue;  //ignore comments
		//split ln into key and value
		string k = key(ln);
		if (k != "") 
		//if the line actually contains a key-value pair (ignore it if it doesn't)
		{
			string val = value(ln);
			val = repEnvVars(val);
			if (!setValue(k, val))
			{
				cout << "Warning: " + k + 
					" not a valid configuration parameter, ignoring.\n";
			}
		}
	}
	if (!exists(rcp_path) || !is_directory(rcp_path))
	{
		cout << "Directory of recipes not found.  You have to edit the configuration file"
			" and set RECIPES to a directory containing some recipes to use this program.\n"
			"Exiting\n";
		exit(1);
	}
}

string Config::key(const string& input)
{
	string::size_type equalsPos = input.find("=");
	if (equalsPos == string::npos)
		return "";
	else
	{
		return input.substr(0, equalsPos);
	}
}

string Config::value(const string& input)
{
	string::size_type equalsPos = input.find("=");
	assert(equalsPos != string::npos);
	return input.substr(equalsPos + 1);
}

bool Config::setValue(const string& key, const string& value)
{
	path valueAsPath(value);

	//Here's where we need a switch/select statement for values 
	//other than integers(or pattern matching)
	if (key == "RECIPES") {
        if (!exists(valueAsPath)) {
            cout << "Warning: `" << value << "' is not a valid filename.  Ignoring.\n";
            return false;  //if the input was invalid
        }
		rcp_path = valueAsPath;
    } else if (key == "GROCERY_LIST") {
		rcp_groceries = valueAsPath;
    } else if (key == "INPUT") {
        if (!exists(valueAsPath)) {
            cout << "Warning: `" << value << "' is not a valid filename.  Ignoring.\n";
            return false;  //if the input was invalid
        }
		rcp_meals = valueAsPath;
    } else if (key == "SCHEDULE") {
		rcp_schedule = valueAsPath;
    } else if (key == "REG_INGREDIENTS") {
        if (!exists(valueAsPath)) {
            cout << "Warning: `" << value << "' is not a valid filename.  Ignoring.\n";
            return false;  //if the input was invalid
        }
		reg_ingredients = valueAsPath;
    } else if (key == "RECIPE_LIST") {
		rcp_recipesChosen = valueAsPath;
    } else if (key =="OWNED_INGREDIENTS") {
        if (!exists(valueAsPath)) {
            cout << "Warning: `" << value << "' is not a valid filename.  Ignoring.\n";
            return false;  //if the input was invalid
        }
        rcp_ownedingredients = valueAsPath;
    } else {
		return false;
    }
	return true;
}

