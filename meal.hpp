#ifndef MEAL_HPP
#define MEAL_HPP

#include "recipe.h"
#include <vector>

/** A meal is a list of recipes which are eaten in one meal.
 It should have 2 vegetable servings and 1 meat serving. */
class Meal {
public:
	friend class MealSchedule;

	const static unsigned MEAT_SERVINGS = 1;
	const static unsigned VEG_SERVINGS = 2;

	Meal() : meat(0), veg(0), _out(0) {}

	/** Returns whether it has the right amount of food for a meal */
	bool isFull();

	/** Check whether a recipe will "fit" in this meal */
	bool canBeAdded(Recipe& recipe);

	/** Tries to add a recipe to the meal.  If it won't "fit", returns false;
	  otherwise, returns true. */
	bool add(Recipe& recipe);

	friend std::ostream& operator<<(std::ostream& os, Meal& meal)
	{
		if (meal.isOut()) os << "OUT";
		else
		{
            std::vector<Recipe>& r = meal.recipes;
			for (std::vector<Recipe>::iterator recipe = r.begin(); recipe != r.end(); )
			{
				os << *recipe;
				if (++recipe != r.end()) os << ", ";
			}
		}
		return os;
	}

	void out() { _out = true; }

	bool isOut() const { return _out; }

private:
    std::vector<Recipe> recipes;    //A list might be better, but it won't let me
		//get the next element given a pointer to an element (which is strange
		//because that's what lists are supposed to do)
	unsigned meat, veg;
	bool _out;  //Stores whether the meal is "out" - i.e. if true means I'm going to a
		//restaurant or otherwise don't have to cook
};

#endif
