#ifndef CHOOSERECIPES_H
#define CHOOSERECIPES_H

#include <string>
#include <vector>
#include "recipe.h"
#include "mealschedule.hpp"

/** Add the recipes specified by the user from the input file (e.g. meals.txt) */
void addUserRecipes(std::vector<std::string>& recipeList, std::vector<Recipe>& recipesChosen, MealSchedule& schedule, bool noSchedule);

/** Choose the recipes, randomly selecting recipes and asking the user whether they
  want to make it.
    Returns true if the user chose to stop, false if it finished by going through all the recipes.
	@param allRecipes If true, say "yes" to all recipes automatically for the purpose
		of getting a grocery list of all ingredients and a list of all recipes
*/
bool chooseRecipes(std::vector<std::string>& recipeList, std::vector<Recipe>& recipesChosen, MealSchedule& schedule, bool allRecipes, bool noSchedule);

#endif
