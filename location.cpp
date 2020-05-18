#include "location.h"
#include <filesystem>
#include "mylib.h"

using namespace std;
using namespace filesystem;

path locationOf(const string& recipeName) {
	path fname;
	path location;

	try {  //Might throw if there's an invalid path name
		//Get the real filename,
		//and make sure the recipe file exists
		fname = path(recipeName + ".rcp");
	}
	catch(filesystem_error&) {
		throw InvalidFileErr("`" + fname.string() + "' is not a valid file name.");
	}

	if (exists(config.rcp_path / (recipeName + ".rcp"))) {
		location = config.rcp_path / (recipeName + ".rcp");
	} else if (exists(config.rcp_path / (recipeName + ".txt"))) {
        location = config.rcp_path / (recipeName + ".txt");
    }
    else {
		throw InvalidFileErr("The file for " + recipeName + " was not found.");
	}

	return location;
}

void readInto(Recipe& recipe)
{
	path filename = locationOf(recipe.name);
	ifstream recipeFile(filename);
	recipeFile >> recipe;
}

Config config;
