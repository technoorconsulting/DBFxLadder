#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iosfwd>

// not generally a good idea.
// Usually I would use fully qualified name but this
// makes code easier to read
using namespace std;


class CsvFileParser
{
protected:
    // Keep the file stream pointer's in case we want to read only parts of a file
    ifstream * f_in;
    long tokenRow;
    long tokenCol; 

    //ofstream * f_out;

public:
    static const long ALL = 0;
    typedef std::vector<string> csvline;
    typedef size_t indextype;
	std::string sFilename;
	std::string m_logFileName;
	istream * GetStream();
    CsvFileParser(char * filename, char * outputname);
    virtual ~CsvFileParser(void);

public:

};


