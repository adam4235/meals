#include "meal.hpp"

bool Meal::canBeAdded(Recipe& recipe)
{
	if (isOut()) return false;
	else return !(recipe.meatServings + meat > MEAT_SERVINGS ||
	       recipe.vegServings + veg > VEG_SERVINGS);
}

bool Meal::add(Recipe& recipe) {
	if (canBeAdded(recipe))
	{
		recipes.push_back(recipe);
		meat += recipe.meatServings;
		veg += recipe.vegServings;
		return true;
	}
	else return false;
}

bool Meal::isFull() {
	return isOut() || (meat == MEAT_SERVINGS && veg == VEG_SERVINGS);
}
