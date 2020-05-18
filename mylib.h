#ifndef _MYLIB_
#define _MYLIB_

/*The library functions of me, Adam Richard.
 *These help make programming in C++ considerably easier.
 */

#include <ctime>
#include <cstdlib>
#include <stdexcept>
#include <fstream>

//include some common includes that should always be there
#include <string>
#include <iostream>

typedef unsigned char byte;

/*returns the current date as a string*/
char* date();

//Get the current time as a structure
struct tm curTimeStruct();

/*returns the length of a file*/
long len(std::ifstream& file);

/*Can be called to initialize various things easily*/
void init();

/**Returns a random integer in the range 1 to max*/
int ranInt(int max);

/*Functions to trim spaces (or any character) from the start or end
 *of a string*/
std::string ltrim(std::string s, const char* chars_to_trim = " ");
std::string rtrim(std::string s, const char* chars_to_trim = " ");
std::string trim(std::string s);

//Return the path portion of a full pathname
std::string pathOf(std::string filename);

//An exception class that actually lets you pass a string to the constructor
class Exception : public std::exception {
public:
	std::string error_string;
	Exception(const std::string& error_string) {
		this->error_string = error_string;
	}

	virtual const char* what() const throw() {
		return error_string.c_str();
	}
	~Exception() throw() {}  //This is just here because the destructor of std::exception is virtual - I don't know why it is
};

/* Thrown when reading from a file, but it has the wrong format */
/* This class won't compile and I don't know why.*/
class InvalidFileErr : public Exception {
public:
  InvalidFileErr(const std::string& error_string) : Exception(error_string) {}
};


//Returns a filename without the extension
std::string dropExt(std::string filename);


/* INLINE FUNCTIONS
 * Note that inline functions only need to appear in the header file
 */

/*Returns path the application is running in.
  Unfortunately argv must be passed to appPath.*/
inline std::string appPath(char** argv) {
  return pathOf(std::string(argv[0]));
}


/*The following bunch of functions are the same as their C counterparts, but
  you can pass a string to them.
  Without these, if you had a string and wanted to execute a system command,
  you would have to use system(s.c_str()). With these functions, you can simply
  say system(s).
  I don't know why the C++ people didn't make something like this part of the
  standard.
  */

inline int system(const std::string& cmd) {
  return system(cmd.c_str());
}
//I hope to remove execute soon
inline int execute(const std::string& cmd) {
  return system(cmd);
}

inline long atol(std::string s) {
  return atol(s.c_str());
}

//Converts a string to an int without the need to call string.c_str()
inline long toInt(std::string s) {
  return atol(s);
}

inline const char* getenv(std::string name) {
  const char* retVal = getenv(name.c_str());
	if (retVal == NULL) return "";
	else return retVal;
}

std::string strtok(std::string &s, char delim = ' ');

#endif
