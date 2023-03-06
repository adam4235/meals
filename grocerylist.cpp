#include "grocerylist.h"
#include <algorithm>
#include <iostream>
#include "location.h"

using namespace std;

void GroceryList::merge() {
	stable_sort(ingredients.begin(), ingredients.end());

	for (vector<Alternative>::size_type i = 1; i < ingredients.size(); ++i)
	{
		if (ingredients[i].addableTo(ingredients[i-1]))
		{
			ingredients[i] += ingredients[i-1];
			ingredients[i-1].amt = 0;  //mark it so it will be ignored
		}
	}
}

//allRecipes: if true, always add all alternatives when there are multiple choices
void GroceryList::addIngredientsOf(Recipe& recipe, bool allRecipes)
{
	for (vector<Recipe::Ingredient>::iterator i = recipe.ingreds.begin();
	     i < recipe.ingreds.end(); ++i)
	{
		vector<Recipe::Ingredient::Alternative>::size_type choice;
		if (i->alternatives.size() > 1)
		{
            //multiple alternatives
 
            //Does the user own at least one of them?  If so, add all the owned
            //ones to the grocery list (they'll be marked as owned), and skip asking
            //the user which one they want to use.
            bool askUser = true;
   			for (vector<Recipe::Ingredient::Alternative>::iterator 
                    j = i->alternatives.begin(), 
                    end = i->alternatives.end();
                 j != end; ++j)
			{
                if (allRecipes)
                {
                    ingredients.push_back(*j);
                    askUser = false;
                }
                else
                {
                    for (vector<Recipe::Ingredient::Alternative>::iterator 
                            k = ownedIngredients.begin(), 
                            end = ownedIngredients.end();
                         k != end; ++k)
                    {
                        if (k->sameType(*j) && k->isAtLeast(*j))
                        {
                            ingredients.push_back(*j);
                            askUser = false;
                            break;
                        }
                    }
                }
			}
            if (askUser)
            {
                cout << "For recipe " << recipe.name << ", use which alternative?\n";
                for (vector<Recipe::Ingredient::Alternative>::size_type j = 0;
                     j < i->alternatives.size(); ++j)
                {
                    cout << j << ") " << i->alternatives[j] << "\n";
                }
                cin >> choice;
            } else {
                continue;
            }
		}
		else 
            choice = 0;

		Recipe::Ingredient::Alternative & altChosen = i->alternatives.at(choice);
        ingredients.push_back(altChosen);
	}
}
