#ifndef READ_CONFIG_H
#define READ_CONFIG_H

/**A class to hold configuration parameters, and read them from a file.*/

#include <filesystem>
#include <string>

class Config {
public:
	Config();
    void init();  //Called to initialize the config file

    std::filesystem::path rcp_path;
    std::filesystem::path rcp_groceries;
    std::filesystem::path rcp_meals;
    std::filesystem::path rcp_schedule;
    std::filesystem::path reg_ingredients;
    std::filesystem::path rcp_recipesChosen;
    std::filesystem::path rcp_ownedingredients;

private:
	/**The file in the user's home directory which might
	 * be the configuration file.*/
	const static std::filesystem::path HOME_CONFIG_FILE;
	/**The environment variable specifying the configuration file.*/
	const static std::string CONFIG_ENV_VAR;

	/** Extract the key from key-value pair input (e.g. in RECIPES=/recipes, returns "RECIPES")*/
    std::string key(const std::string& input);

	/** Extract the value from key-value pair input (e.g. in RECIPES=/recipes, returns "/recipes")*/
    std::string value(const std::string& input);

	/** Sets the appropriate variable for the key-value pair (key, value)*/
	bool setValue(const std::string& key, const std::string& value);

	/** Tries to open one of the possible locations for the config file, into strm.
	 * Returns true on success, false on failure.
	 * @param envVal an environment variable containing the config file location
	 * @param useHomeConfigFile if true, envVar is the home directory, and append the
	 * default file name for the config file to it*/
	bool openConfigFile(std::string envVal, bool useHomeConfigFile, std::ifstream& strm);

	/** Loads the configuration, given the opened file configFile. */
	void loadConfig(std::ifstream& configFile);
};


#endif

