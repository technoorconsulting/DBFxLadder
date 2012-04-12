//#include "StdAfx.h"
#include "CsvParser.h"
using namespace std;

CsvFileParser::CsvFileParser(char * filename, char * outname):sFilename(filename)
{
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
