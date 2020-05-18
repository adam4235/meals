#ifndef LOCATION_H
#define LOCATION_H

#include <filesystem>
#include <string>
#include "recipe.h"
#include "read_config.h"

//Initialize the config file
extern Config config;

/** Return the directory that recipeName is found in.  Throws.*/
std::filesystem::path locationOf(const std::string& recipeName);

/** Read the recipe data into recipe from the appropriate file. */
void readInto(Recipe& recipe);

#endif
