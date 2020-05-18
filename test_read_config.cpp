#include "read_config.cpp"
#include <iostream>

int main()
{
	string e;
	e = getenv("HOME");
	cout << e << "\n";
	Config config;
	cout << config.rcp_path.string() << "\n";
	cout << config.rcp_groceries.string() << "\n";
	cout << config.rcp_meals.string() << "\n";
	cout << config.rcp_schedule.string() << "\n";
	cout << config.reg_ingredients.string() << "\n";
	cout << config.rcp_subrecipes.string() << "\n";
	cout << config.rcp_recipesChosen.string() << "\n";
}

