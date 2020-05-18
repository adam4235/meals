#include "chooseRecipes.hpp"
#include "mylib.h"
#include <iostream>
#include "location.h"
#include "grocerylist.h"
#include <fstream>
#include "mealschedule.hpp"
#include <filesystem>
#include <algorithm>

using namespace std;

void addUserRecipes(vector<string>& recipeList, vector<Recipe>& recipesChosen, MealSchedule& schedule, bool noSchedule)
{
	if (!exists(config.rcp_meals)) return;   //user specified no recipes

	ifstream meals(config.rcp_meals);

	string recipeName;
	while (getline(meals, recipeName))
	{
		if (trim(recipeName) == "") continue;

		//parse the current line of the input file
		bool out = false, alreadyMade = false;
		signed char addToDay = -1;
		if (recipeName.substr(3, 1) == ":" && !noSchedule)
		{
			string day = recipeName.substr(0, 3);
			for (int i = 0; i < 7; ++i)
			{
				if (day == MealSchedule::days[i])  //if the day the user specified for the recipe is the ith day of the week
				{
					recipeName = trim(recipeName.substr(4));
					if (recipeName == "OUT") out = true;
					addToDay = i;
					break;
				}
			}
		}
		if (recipeName.substr(0, 1) == "(" &&
		    recipeName.substr(recipeName.size() - 1) == ")")
		//if it's surrounded in brackets, which means that recipe is leftovers
		{
			alreadyMade = true;
			recipeName = recipeName.substr(1, recipeName.size() - 2);  //trim the brackets
		}

		if (find(recipeList.begin(), recipeList.end(), recipeName) == recipeList.end())
			throw InvalidFileErr("`" + recipeName + "' is not a valid recipe.");

		if (out)
		{
			schedule.out(addToDay);
		}
		else
		{
			Recipe recipe(recipeName, alreadyMade);
			readInto(recipe);
			if (schedule.canBeAdded(recipe, addToDay))
			{
				if (!noSchedule) schedule.add(recipe, addToDay);
				recipesChosen.push_back(recipe);
			}
		}
	}
}

bool chooseRecipes(vector<string>& recipeList, vector<Recipe>& recipesChosen, MealSchedule& schedule, bool allRecipes, bool noSchedule)
{
	vector< vector<string>::size_type > recipesConsidered;   //indices of the
		//recipes already considered
	vector<string>::size_type numRecipesLeft = recipeList.size();  //Keep track
		// of how many recipes we have left to consider; when it reaches 0, we're done

	//keep looping until 1) they enter s, 2) all recipes have been presented, or
	//3) the schedule is complete.
	while(numRecipesLeft-- > 0 && (!schedule.isDone() || noSchedule)) {
		vector<string>::size_type recipeChosen;  //the index in recipeList of the
			//recipe chosen randomly
		string recipeName;  //the name of the recipe chosen randomly
		string ans;  //the user's response

		do {
			recipeChosen = ranInt(recipeList.size()) - 1;  //pick a random next recipe
			recipeName = recipeList[recipeChosen];
			
		} while (find(recipesConsidered.begin(),
									recipesConsidered.end(),
									recipeChosen) != recipesConsidered.end()); /*ensure that
										the same recipe is not considered twice. */

		Recipe recipe(recipeName);
		readInto(recipe);
        if (config.rcp_ownedingredients != "")
        {
            GroceryList ownedIngredients;
            {
                ifstream fowned(config.rcp_ownedingredients);
                fowned >> ownedIngredients;
            }
            recipe.printMissingIngredients(ownedIngredients);
        }

		if (allRecipes)
			ans = "Y";
		else if (!noSchedule && !schedule.canBeAdded(recipe))
			//Don't ask the user for recipes that couldn't "fit" in the schedule anyway
			ans = "N";
		else {
			bool alreadyChosen = false;  //True if the random recipe has already been chosen for cooking
			//Some recipes were already chosen by the user in the input file.  Don't ask for those ones again.
			for (vector<Recipe>::iterator r = recipesChosen.begin(); r != recipesChosen.end(); ++r)
			{
				if (r->name == recipeName) 
				{
					alreadyChosen = true;
					break;
				}
			}
			if (alreadyChosen) ans = "N";
			else
			{
				cout << "Use " << recipeName << "? (y/n, s to stop, recipe name to use that recipe) ";
				cin >> ans;
			}
		}

		if (ans == "S" || ans == "s" || cin.fail())
            return true;

		bool yes = (ans == "Y" || ans == "y"), no = (ans == "N" || ans == "n");
		if (yes || no) recipesConsidered.push_back(recipeChosen);
		if (no) {}
		else
		{
			if (!yes)  //entered either a specific recipe name, or something invalid
			{
				recipeName = "";  //Assume it's an error until we find otherwise
				vector<string>::iterator pos;
				if ((pos = find(recipeList.begin(), recipeList.end(), ans)) != recipeList.end())
				{
					vector<string>::size_type index = distance(recipeList.begin(), pos);
					recipesConsidered.push_back(index);
					recipeName = ans;
					cout << "Adding " << recipeList[index] << ".\n";  //extra check that "index" is correct
					//Have to actually use the new recipe rather than the randomly chosen one
					recipe = Recipe(recipeName);
					readInto(recipe);
				}
				else
				{
					cout << "Error:" << ans << " is not a recipe or a valid command.\n";
					continue;  //Error occurred; don't add the recipe.
				}
			}
			recipesChosen.push_back(recipe);
			if (!noSchedule)
			{
				schedule.add(recipe);  //can't fail, because of the canBeAdded check above
				cout << schedule;
			}
		}
	}
    return false;
}
