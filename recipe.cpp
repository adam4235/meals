#include "recipe.h"
#include "grocerylist.h"
#include <algorithm>

using namespace std;

istream& operator>>(istream& is, Recipe::Ingredient::Alternative& ingred) {
	string name, amt;
	getline(is, name);
	if (trim(name) != "") {
        if (atof(name.c_str()) == 0)
        {
            //Ingredients without amounts are considered "infinite" or "some amount"
            ingred.makeUnlimited();
            ingred.unit = "";
        }
        else
        {
            amt = strtok(name);
            ingred.unit = strtok(name);
            if (trim(name) == "")
                throw InvalidFileErr("Not a recipe: line starting with " + amt +
                                     " doesn't have enough spaces.");
            if (amt.find("/") != string::npos || amt.find("\\") != string::npos)
                throw InvalidFileErr("Not a recipe: line starting with " + amt + " contains a slash.  Use decimal numbers.");
            if (ingred.unit.find("/") != string::npos || ingred.unit.find("\\") != string::npos)
                throw InvalidFileErr("Not a recipe: line starting with " + ingred.unit + " contains a slash.  Use decimal numbers.");
            ingred.amt = atof(amt.c_str());
         }

		/*Remove things in brackets, since they probably aren't part of the name*/
		string::size_type lBracketPos = name.find("("),
		                  rBracketPos = name.find(")");
		if (lBracketPos != string::npos && rBracketPos != string::npos)
		{
			name.replace(lBracketPos, rBracketPos - lBracketPos + 1, "");
			//could compress it too but it's probably not a big enough deal.
		}
	}

	name = trim(name);  //Here's where in-place overloaded functions would be good
	ingred.name = name;
	return is;
}

istream& operator>>(istream& is, Recipe::Ingredient& ingred) {
	ingred.alternatives.clear();   //Clear the list of alternatives; otherwise it
		//will keep building up the list of ingredients for a recipe.
	Recipe::Ingredient::Alternative ingInput;
	bool choice = false;  //Whether it ended in OR, meaning we'd need
		//to keep inputting more alternatives
	do {
		is >> ingInput;
		if (is.fail() || ingInput.name == "") break;
		string& name = ingInput.name;
		if (name.substr(name.size() - 3) == " OR")
		{
			choice = true;
			name = rtrim(name.substr(0, name.size() - 3));  //remove the OR and any other trailing spaces
				//(we don't want to store it that way)
		}
		else choice = false;
		ingred.alternatives.push_back(ingInput);
	} while (choice);

	return is;
}

istream& operator>>(istream& is, Recipe& recipe) {
    /*
	string name;
	if (!getline(is, name))
		throw InvalidFileErr(recipe.name + " is not a recipe: the name couldn't be read from the first line.");
    */

	recipe.ingreds.clear();     //Clear the list of ingredients; otherwise it
		//will keep building up the list of ingredients for a recipe.

	//First get all the headers
	string input, label, value;
	while (getline(is, input) && trim(input) != "") {  //input a line
		//split the line into 2 sections
		string::size_type colonPos = input.find(":");
        if (colonPos == string::npos)
            continue;
		label = input.substr(0, colonPos);
		value = input.substr(colonPos + 1);

		//do things based on the header
		if (label == "vegetable")
			recipe.vegServings = toInt(value);
		else if (label == "meat")
			recipe.meatServings = toInt(value);
		else if (label.substr(0, 5) == "yield")
		{
			recipe.yield = toInt(value);
			//recipes specified by the user as already made should only
			//be added to the schedule once.
			if (recipe.alreadyMade()) recipe.yield = 1;
		}
	}

	if (is.eof()) throw InvalidFileErr(recipe.name + " is not a recipe: the ingredients list wasn't found.");

    try
    {
        //Read the ingredients into the recipe
        do {
            Recipe::Ingredient ingInput;
            is >> ingInput;
            if (ingInput.alternatives.size() == 0) break;  //blank line
            recipe.ingreds.push_back(ingInput);
        } while (!is.fail());
    }
    catch (InvalidFileErr const& err)
    {
        throw InvalidFileErr("Error reading " + recipe.name + ": " + err.what());
    }

	//The instructions would come last but we don't need them.

	return is;
}

Recipe& Recipe::operator*=(float multiple)
{
	for (vector<Recipe::Ingredient>::iterator ingred = ingreds.begin(); ingred != ingreds.end(); ++ingred)
	{
		for (vector<Recipe::Ingredient::Alternative>::iterator alternative = ingred->alternatives.begin();
			alternative != ingred->alternatives.end(); ++alternative)
		{
			alternative->amt *= multiple;
		}
	}
	return *this;
}

/** Determine how many ingredients the recipe calls for that I don't already own. 
 *  ownedIngredients is the list of ingredients I already own. */
int Recipe::numMissingIngredients(GroceryList const& ownedIngredients)
{
    int ret = 0;
	for (vector<Recipe::Ingredient>::const_iterator ingred = ingreds.begin(); ingred != ingreds.end(); ++ingred)
	{
        bool foundIngred = false;
		for (vector<Recipe::Ingredient::Alternative>::const_iterator alternative = ingred->alternatives.begin();
			alternative != ingred->alternatives.end(); ++alternative)
		{
            for (vector<Recipe::Ingredient::Alternative>::const_iterator ownedIngred = ownedIngredients.ingredients.begin();
                     ownedIngred != ownedIngredients.ingredients.end(); ++ownedIngred)
            {
                if (ownedIngred->sameType(*alternative) &&
                    ownedIngred->isAtLeast(*alternative))
                {
                    foundIngred = true;
                    break;
                }
            }
        }
        if (!foundIngred)
            ++ret;
    }
    return ret;
}

vector<Recipe::Ingredient> 
Recipe::getMissingIngredients(GroceryList const& ownedIngredients) 
{
    vector<Recipe::Ingredient> ret;
	for (vector<Recipe::Ingredient>::const_iterator ingred = ingreds.begin(); ingred != ingreds.end(); ++ingred)
	{
        bool foundIngred = false;
		for (vector<Recipe::Ingredient::Alternative>::const_iterator alternative = ingred->alternatives.begin();
			alternative != ingred->alternatives.end(); ++alternative)
		{
            for (vector<Recipe::Ingredient::Alternative>::const_iterator ownedIngred = ownedIngredients.ingredients.begin();
                     ownedIngred != ownedIngredients.ingredients.end(); ++ownedIngred)
            {
                if (ownedIngred->sameType(*alternative) &&
                    ownedIngred->isAtLeast(*alternative))
                {
                    foundIngred = true;
                    break;
                }
            }
        }
        if (!foundIngred)
            ret.push_back(*ingred);
    }
    return ret;
}

void Recipe::printMissingIngredients(GroceryList const& ownedIngredients)
{
    cout << "Missing ingredients for: " << name << endl;
    vector<Recipe::Ingredient> missingIngredients =
        getMissingIngredients(ownedIngredients);
    for (vector<Recipe::Ingredient>::const_iterator missingIngred = missingIngredients.begin();
            missingIngred != missingIngredients.end(); ++missingIngred)
    {
        cout << *missingIngred << endl;
    }
}

