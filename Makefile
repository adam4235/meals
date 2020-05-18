#g++ version 7 doesn't have support for std::filesystem
CXX=g++-8
CXXFLAGS=-Wall -g2 -std=c++17 #-arch i386 -arch ppc
obj =  main.o mylib.o recipe.o grocerylist.o location.o chooseRecipes.o meal.o mealschedule.o read_config.o

#std::filesystem currently needs -lstdc++fs because it's experimental
meals: $(obj)
	$(CXX) $(CXXFLAGS) $(obj) -lstdc++fs -o meals

clean:
	rm -f *.o meals

depend:
	$(CXX) -MM $(CXXFLAGS) *.cpp > .depend

#If .depend doesn't exist, you have to comment this line out and run "make depend" to create it.
include .depend

