#ifndef GROCERYLIST_H
#define GROCERYLIST_H

#include "recipe.h"
#include <vector>
#include <filesystem>
#include "mylib.h"

/** A grocery list has a list of Alternatives */
class GroceryList {
public:

	typedef Recipe::Ingredient::Alternative Alternative;

	/** Combine like ingredients into 1 ingredient */
	void merge();

	/** Add the ingredients of the recipe specified by fullPath to this grocery list */
	void addIngredientsOf(Recipe& recipe, bool allRecipes);

	void add(Recipe::Ingredient::Alternative& ingredient) { ingredients.push_back(ingredient); }

	friend std::ostream& operator<<(std::ostream& os, GroceryList const& list) {
		for (std::vector<Recipe::Ingredient::Alternative>::const_iterator i = list.ingredients.begin();
		     i < list.ingredients.end(); ++i)
		{
			if (i->amt != 0)  //If it hasn't been marked for "deletion"
            {
				os << *i;
                for (std::vector<Recipe::Ingredient::Alternative>::const_iterator itr = list.ownedIngredients.begin();
                        itr != list.ownedIngredients.end(); ++itr)
                {
                    if (itr->sameType(*i) && itr->isAtLeast(*i))
                        os << " [owned]";
                }
                os << std::endl;
            }
		}
		return os;
	}

    friend std::istream& operator>>(std::istream& is, GroceryList& list) {
		while (!is.fail() && !is.eof()) {
			Recipe::Ingredient::Alternative ingInput;
			is >> ingInput;
			if (ingInput.amt != 0)
			//if the ingredient wasn't a blank line
			{
				list.add(ingInput);
			}
		}
        return is;
    }

    void setOwnedIngreds(std::vector<Recipe::Ingredient::Alternative> const& owned)
    {
        ownedIngredients = owned;
    }

    std::vector<Recipe::Ingredient::Alternative> ingredients;
    std::vector<Recipe::Ingredient::Alternative> ownedIngredients;

};

#endif
