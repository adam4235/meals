#ifndef MEALSCHEDULE_H
#define MEALSCHEDULE_H

#include "meal.hpp"
#include <vector>

/** A meal schedule is an array of a certain number of meals,
	normally 1 per day of the week. */
class MealSchedule {
public:
	const static unsigned NUM_MEALS = 7, MAX_MEALS_PER_RECIPE = 3;
	const static std::string days[];

	MealSchedule();
	
	/** Returns whether the schedule is complete (has all meals with enough food). */
	bool isDone() { return numLeftToFill == 0; }
	
	/** Check whether recipe can be added to this schedule
	  (based on its meat & veg servings, etc.) 
	  @param addToDay If not -1, check whether recipe can be added to a specific day in the schedule.*/
	bool canBeAdded(Recipe& recipe, short addToDay = -1);

	/** Add a recipe to the schedule.  Adds it to as many meals as possible, up to a maximum. */
	void add(Recipe& recipe, short addToDay = -1);

	/** Allows iterating through each *recipe* (not meal) in the schedule */
	Recipe& eachRecipe() const;

	/** Sets day as "out" - meaning I don't have to cook anything that day. */
	void out(short day);

	friend std::ostream& operator<<(std::ostream& os, MealSchedule& schedule);
private:
    std::vector<Meal> meals;
	int numLeftToFill;  //Number of meals to "fill" before the schedule is complete
};

#endif
