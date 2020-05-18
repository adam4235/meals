#ifndef RECIPE_H
#define RECIPE_H

#include "mylib.h"
#include <vector>
#include <cassert>

class GroceryList;

/** A recipe consists of a list of ingredients plus some other information*/
class Recipe {
public:

	/** An ingredient is a list of alternatives for the ingredient */
	class Ingredient {
	public:
		/** An alternative holds the data for each ingredient, which includes an amount, the units
		* for the amount, and a name which included explanatory information */
		class Alternative {
		public:
			float amt;
            std::string unit;
            std::string name;

			Alternative() : amt(0), unit(""), name("") {}

            Alternative(float amt, std::string unit, std::string name) 
                : amt(amt), unit(unit), name(name) {}

            friend std::ostream& operator<<(std::ostream& os, const Alternative& ingred) {
                if (!ingred.isUnlimited())
                    os << ingred.amt << " " << ingred.unit << " ";
                os << ingred.name;
                return os;
            }
            friend std::istream& operator>>(std::istream& is, Alternative& ingred);

            bool isUnlimited() const
            {
                return amt == -1;
            }
            void makeUnlimited()
            {
                amt = -1;
                unit = "";
            }

            bool sameUnits(Alternative const& other) const
            {
                // Don't consider an "s" on the end to matter (e.g. onion and onions are the same)
                std::string unit1 = rtrim(unit, "s"), unit2 = rtrim(other.unit, "s");
                return unit1 == unit2;
            }

            // Whether an ingredient is the same as another (with possibly different units)
            bool sameType(Alternative const& other) const
            {
                // Don't consider an "s" on the end to matter (e.g. onion and onions are the same)
                std::string name1 = rtrim(name, "s"), name2 = rtrim(other.name, "s");
                return name1 == name2;
            }

            // Check if we have enough of an ingredient
            bool isAtLeast(Alternative const& needed) const
            {
                assert(sameType(needed));
                if (isUnlimited())
                    return true;
                else if (needed.isUnlimited())
                    return false;
                else
                    return sameUnits(needed) && amt >= needed.amt;
            }

            /** Compare 2 alternatives for the purpose of sorting them to remove duplicates.
              Sort first by name, then by unit. */
            bool operator<(const Alternative& other) const
            {
                if (name == other.name)
                    return unit < other.unit;
                else
                    return name < other.name;
            }

            /** Add like alternatives by adding their amounts */
            Alternative& operator+=(const Alternative& other)
            {
                assert(addableTo(other));
                if (isUnlimited())
                {
                }
                else if (other.isUnlimited())
                {
                    makeUnlimited();
                }
                else
                {
                    amt += other.amt;
                }
                return *this;
            }

            const Alternative operator+(const Alternative& rhs) {
                return Alternative(*this) += rhs;
            }

            /** Whether ingredients are compatable */
            bool addableTo(const Alternative& other)
            {
                if (!sameType(other))
                    return false;

                if (sameUnits(other))
                    return true;
                else if (isUnlimited() || other.isUnlimited())
                    return true;
                else
                    return false;
            }
		};

        std::vector<Alternative> alternatives;

		friend std::ostream& operator<<(std::ostream& os, const Ingredient& ingred) {
			const std::vector<Alternative> & a = ingred.alternatives;
			for (std::vector<Alternative>::size_type i = 0; i < a.size(); ++i) {
				os << a[i];
				if (i < a.size() - 1) os << " ";  //separate alternatives with spaces
			}
			return os;
		}

		friend std::istream& operator>>(std::istream& is, Ingredient& ingred);
	};

    std::vector<Ingredient> ingreds;
	unsigned vegServings, meatServings, yield;
    std::string name;  //The name of the recipe (filename without the .rcp extension)

	/*You can't directly assign ingredients; they must be read in from a file*/
	explicit Recipe(std::string filename, bool alreadyMade = false) {
		ingreds = std::vector<Ingredient>(0);
		vegServings = 0;
		meatServings = 0;
		name = dropExt(filename);
		yield = 0;
		_alreadyMade = alreadyMade;
	}

	friend std::ostream& operator<<(std::ostream& os, const Recipe& recipe) {
		os << recipe.name;
		return os;
	}

	/** Input a recipe (almost certainly from a file)
		recipe files have the following format:

		header1
		header2
		... (until a blank line)
		ingredient1
		ingredient2
		... (until a blank line)
		instructions (as one or more paragraphs)

		The headers are of the form label:value (maybe with spaces around the colon).
		The ingredients have the form amount unit name, where amount is a floating point
		value and unit and name are any string.  Alternatively, they can be just a name.
	*/
	friend std::istream& operator>>(std::istream& is, Recipe& recipe);

	/** Multiply a recipe by multiple.  E.g. if multiple is 2, double the recipe. */
	Recipe& operator*=(float multiple);

	/** Returns true iff the recipe was already made (i.e. is leftovers) */
	bool alreadyMade() const { return _alreadyMade; }

    std::vector<Recipe::Ingredient> getMissingIngredients(GroceryList const& ownedIngredients);
    int numMissingIngredients(GroceryList const& ownedIngredients);
    void printMissingIngredients(GroceryList const& ownedIngredients);

private:
	bool _alreadyMade;
};

#endif
