/** This program manages my cooking and meal preparation.
	* It looks at all the files in my directory of recipies and randomly chooses some
	* for me to make.  Then it generates a grocery list based on the ingredients listed in the recipe files.
	*
	* Author: Adam Richard
	* ftgog
	*
	* You may do anything you want with this code except call it your own.
	*/

#include "mylib.h"
#include <algorithm>
#include <vector>
#include <filesystem>
#include "recipe.h"
#include "grocerylist.h"
#include "location.h"
#include "chooseRecipes.hpp"
#include "mealschedule.hpp"

using namespace std;

void fileErr(const std::filesystem::path & file, const InvalidFileErr &e) {
	cerr << "The file \"" << file
	     << "\" is not a valid recipe.  The error was:\n" << e.what() << "\nFix it then try again.\n";
}

/*Produce a grocery list from the ingredients of all the recipes selected
	and from a file of ingredients that are always added*/
void groceryList(vector<Recipe> & recipesChosen, bool allRecipes)
{
	GroceryList list;

    if (config.rcp_ownedingredients != "")
    {
		ifstream ownedIngreds(config.rcp_ownedingredients);
        GroceryList owned;
        ownedIngreds >> owned;
        list.setOwnedIngreds(owned.ingredients);
    }

	//fill up ingredients with the ingredients of all recipes chosen
	for (vector<Recipe>::iterator recipe = recipesChosen.begin();
		recipe < recipesChosen.end(); ++recipe)
	{
		if (!recipe->alreadyMade()) list.addIngredientsOf(*recipe, allRecipes);
	}

	/*Add the regular ingredients - ingredients I eat for a snack, for example,
		that I always want on my grocery list*/
	if (exists(config.reg_ingredients))
	//if the user has specified a file of regular ingredients
	{
		ifstream regIngreds(config.reg_ingredients);
        regIngreds >> list;
	}

	list.merge();

	//output to a file
	if (config.rcp_groceries != "")
	//if the user has specified a file for the grocery list
	{
		ofstream ingredients_txt(config.rcp_groceries);
		ingredients_txt << "Grocery list" << endl << list;
		cout << "File " << config.rcp_groceries << " written successfully.\n";
	}
}

void saveRecipes(vector<Recipe> & recipes) {
	if (config.rcp_recipesChosen == "") return;  //if the user hasn't specified a directory to
		//save the list of recipes (script)
	ofstream out(config.rcp_recipesChosen);
	if (out.fail()) {
		cerr << "Couldn't open the file " << config.rcp_recipesChosen
					<< " to write recipes to.";
		exit(1);
	}
	out << "install-memo ";
	for (vector<Recipe>::iterator recipe = recipes.begin(); recipe < recipes.end(); ++recipe)
	{
		if (!recipe->alreadyMade())
			out << "/home/adam/recipes/supper/" << *recipe << ".rcp ";
	}
	if (config.rcp_groceries != "")
		out << config.rcp_groceries;  //grocery list needs syncing too
	cout << "File " << config.rcp_recipesChosen << " written successfully.\n";
}

vector<string> recipesWithNToBuy(int i, vector<string> const& recipeList)
{
    vector<string> ret;
    GroceryList ownedIngredients;
    {
        ifstream fowned(config.rcp_ownedingredients);
        fowned >> ownedIngredients;
    }
    for (vector<string>::const_iterator recipeName = recipeList.begin(); recipeName != recipeList.end(); ++recipeName)
    {
        Recipe recipe(*recipeName);
        readInto(recipe);
        int numMissing = recipe.numMissingIngredients(ownedIngredients);
#if 0
        cout << *recipeName << " has " << numMissing << " missing" << endl;
#endif
        if (numMissing == i)
            ret.push_back(*recipeName);
    }
    return ret;
}

int main(int argc, char* argv[]) {

    //Initialize the configuration file
    config.init();

	/* Handle the arguments */
	bool allRecipes = false;
	bool noSchedule = false;
    bool onhand = false;
	if (argc > 1) {
		string arg(argv[1]);
		if (arg == "all")
			allRecipes = noSchedule = true;
		else if (arg == "noschedule")
			noSchedule = true;
        else if (arg == "onhand")
            onhand = true;
		else {
			cout << "Usage: " << argv[0] << " [noschedule|all]\n"
				<< "\"noschedule\" makes " << argv[0] << " output a simple list of "
					"recipes rather than a schedule.\n"
				<< "\"all\", which implies noschedule, generates the grocery list from all recipes.\n"
                << "\"onhand\" takes the list of owned ingredients and suggests recipes that require buying the fewest ingredients.\n";
			return 1;
		}
	}

	init();  //need random numbers

	//Get the list of recipes on my computer
	vector<string> recipeList;
	for (std::filesystem::directory_iterator i(config.rcp_path); i != std::filesystem::directory_iterator(); i++)
		if (!std::filesystem::is_directory(*i))
			recipeList.push_back(i->path().stem().native());

	if (recipeList.size() == 0) {
		cerr << "Error: no recipe files were found.";
		return 4;
	}

	try
	{
		MealSchedule schedule;
		vector<Recipe> recipesChosen;  //can't be extracted from the meal schedule, because
			//I might want to just get a list of recipes, and a meal schedule sometimes can't
			//fit them all.
		addUserRecipes(recipeList, recipesChosen, schedule, noSchedule);
        if (onhand)
        {
            for (int i = 0; i <= 5; ++i)
            {
                cout << "Suggesting recipes with " << i << " ingredients to buy." << endl;
                vector<string> onHandRecipes = recipesWithNToBuy(i, recipeList); 
#if 0
                for (vector<string>::iterator itr = onHandRecipes.begin(); itr != onHandRecipes.end(); ++itr)
                    cout << *itr << endl;
                exit(0);
#endif
                if (chooseRecipes(onHandRecipes, recipesChosen, schedule, allRecipes, noSchedule))
                    break;
            }
        }
        else
        {
		    chooseRecipes(recipeList, recipesChosen, schedule, allRecipes, noSchedule);
        }
        if (!noSchedule && config.rcp_schedule != "")
		{
			ofstream fschedule(config.rcp_schedule);
			fschedule << schedule;
			cout << schedule;
		}
		saveRecipes(recipesChosen);
		groceryList(recipesChosen, allRecipes);
	}
	catch (InvalidFileErr const& err)
	{
		cerr << err.what() << "\n";
	}
	catch (std::filesystem::filesystem_error const& err)
	{
		cerr << err.what() << "\n";
	}

	return 0;
}
