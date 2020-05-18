#include "mylib.h"

/*returns the current date as a string*/
char* date() {
  time_t aclock;

  time(&aclock);   /* Get time in seconds */
  return asctime(localtime(&aclock));
}

//Get the current time as a structure
struct tm curTimeStruct() {
  time_t curTime;
  time(&curTime);
  return *localtime(&curTime);
}

/*returns the length of a file*/
long len(std::ifstream & file) {
  long oldPos = file.tellg();
  file.seekg(0, std::ios::end);
  long length = file.tellg();
  file.seekg(oldPos, std::ios::beg);  //don't change the position of the file
  return length;
}

/*Can be called to initialize various things easily*/
void init() {
  /*Initialize random numbers
  Note that this should only be initialized once in the program,
  not once per call
  of rand().  This is because if 2 srands are called in a row, and the
  processor is so fast that the time doesn't change in that interval, the
  second random # will be the same as the first.*/
  srand((unsigned)time(NULL));
}

/**Returns a random integer in the range 1 to max*/
int ranInt(int max) {
  int ranNum, maxRanNum;
  do {
    ranNum = rand();
    maxRanNum = RAND_MAX / max * max;  /*this avoids the first few #s having a
                                        higher probability of being chosen and
    ensures that all #s from 1 to max have an equal probability of being chosen*/
  } while (ranNum >= maxRanNum);
  ranNum %= max;  //gives a # in 0..max-1
  ranNum++;  //make it a # in 1..max
  return (int)ranNum;
}

std::string ltrim(std::string s, const char* chars_to_trim) {
  std::string::size_type pos = s.find_first_not_of(chars_to_trim);
  if (pos == std::string::npos) return "";    //if the string only consists of chars_to_trim
  return s.substr(pos);
}

std::string rtrim(std::string s, const char* chars_to_trim) {
  std::string::size_type pos = s.find_last_not_of(chars_to_trim);
  if (pos == std::string::npos) return "";    //if the string only consists of chars_to_trim
  return s.substr(0, pos + 1);
}

std::string trim(std::string s) {
  return rtrim(ltrim(s));
}

//Return the path portion of a full pathname
std::string pathOf(std::string filename) {
  std::string::size_type slashPos = filename.find_last_of("\\");
  if (slashPos == std::string::npos) return "";
  return filename.substr(0, slashPos);
}

//Returns a filename without the extension
std::string dropExt(std::string filename) {
  std::string::size_type dotPos = filename.find_last_of(".");
  if (dotPos == std::string::npos) return filename;
  return filename.substr(0, dotPos);
}

std::string strtok(std::string &s, char delim) {
	std::string::size_type delim_pos = s.find(delim);
	std::string retVal = s.substr(0, delim_pos);
	if (delim_pos == std::string::npos)
		s = "";
	else
		s = s.substr(delim_pos + 1);
	return retVal;
}
