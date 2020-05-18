#include "mealschedule.hpp"
#include <cmath>
#include <algorithm>

using namespace std;

const string MealSchedule::days[] = {"Sat", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri"};

MealSchedule::MealSchedule() {
	meals = vector<Meal>(NUM_MEALS);
	numLeftToFill = NUM_MEALS;
}

bool MealSchedule::canBeAdded(Recipe& recipe, short addToDay)
{
	//If it can be added to any meal, return true; otherwise false
	short i = 0;
	for ( vector<Meal>::iterator meal = meals.begin();
	     meal < meals.end(); ++meal, ++i)
	{
		if (addToDay != -1 && addToDay != i) continue;  //if it's not a day that the meal 
			//can be added to
		if (meal->canBeAdded(recipe)) return true;
	}
	return false;
}

void MealSchedule::add(Recipe& recipe, short addToDay) {
	assert(!isDone());

	unsigned mealsLeftToEatIt = recipe.yield;
	if (mealsLeftToEatIt > MAX_MEALS_PER_RECIPE)
		mealsLeftToEatIt = MAX_MEALS_PER_RECIPE;
	if (mealsLeftToEatIt == 0)  //unknown yield according to the recipe
		//(I leave it blank if I don't know how many it yields)
		mealsLeftToEatIt = MAX_MEALS_PER_RECIPE;

	//We want to add them in a random order, so I don't have the same thing many days in a row
	
	int numLeft = NUM_MEALS;  //Number of meals left to try
	vector<int> mealsDone;
	while (numLeft-- > 0)
	{
		int i;
		do {
			i = ranInt(NUM_MEALS) - 1;
		} while (find(mealsDone.begin(), mealsDone.end(), i) != mealsDone.end());
			
		mealsDone.push_back(i);

		if (addToDay != -1 && addToDay != i) continue;
		if (!meals[i].isFull())   //Without this, things like rice
		//would keep getting added to the first meal.
		{
			if (meals[i].add(recipe))
			{
				if (meals[i].isFull()) --numLeftToFill;
				if (--mealsLeftToEatIt == 0)
					return;
			}
		}
	}
}

void MealSchedule::out(short day)
{
	short i = 0;
	for (vector<Meal>::iterator meal = meals.begin();
	     meal < meals.end(); ++meal, ++i)
	{
		if (i == day)
		{
			meal->out();
			--numLeftToFill;

			//Take all the recipes already in the meal and put them elsewhere in the schedule
			vector<Recipe>& r = meal->recipes;
			for (vector<Recipe>::iterator recipe = r.begin(); recipe < r.end(); ++recipe)
			{
				add(*recipe);
			}

			return;
		}
	}
}

ostream& operator<<(ostream& os, MealSchedule& schedule)
{
	for (unsigned i = 0; i < MealSchedule::NUM_MEALS; ++i)
	{
		//output the day of the week, with 0 -> Sat and 6 -> Fri
		os << MealSchedule::days[i] << ": " << schedule.meals[i] << "\n";
	}
	return os;
}

