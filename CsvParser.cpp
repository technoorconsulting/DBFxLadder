//#include "stdafx.h"
#include "CsvParser.h"
using namespace std;

CsvFileParser::CsvFileParser(const char * filename, const char * outname):sFilename(filename)
{
google::InitGoogleLogging("test.log");
f_in = new ifstream(filename);
}
istream * CsvFileParser:: GetStream()
{
	return f_in;
}

CsvFileParser::~CsvFileParser(void)
{
    f_in->close();
  //  f_out->close();
    delete f_in;
    //delete [] f_out;
}
