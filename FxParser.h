#pragma once
#include "CsvParser.h"
#include <map>
//#include "maphelper.h"
#include <algorithm>
#include <vector>
#include <iterator>
#include "globals.h"
#include <memory>
#include <thread>

#define MYSLEEP(y) usleep(y*1000);
//#include <windows.h>

using namespace global;
class FxDataSink{
public:
	FxDataSink()
	{
		ccyCodePrice.resize(10); // 10 currency pairs
	}

	~FxDataSink(){}

	bool AddToBidAsk(const string & ccypair, const PriceStruct & bid, const PriceStruct& ask, unsigned long & ccyIndx )
	{
		ccyIndx = g_ccyIndex.GetCcyIndex(ccypair);
		bool updated =  PriceStruct::assign(ccyCodePrice[ccyIndx].first, bid);		
		return  PriceStruct::assign(ccyCodePrice[ccyIndx].first, bid)||updated;
	}
	
	void AddToBid(const string & ccypair, double price)
	{
		// todo 
	}

	void AddToAsk(const string & ccypair, double price)
	{
		// todo 
	}


	bool Update( CCYBIDASK & ccyprice)
	{
		AddToBid(ccyprice.ccy, ccyprice.bid.price);
		return false;
	}


private:
	
	vector<BIDASK>  ccyCodePrice;

};

// can use virtual interface to reduce coupling between classes to allow for extensibility at small performance cost
class IMarketManager{
public:
	IMarketManager(){};

	virtual ~IMarketManager(){}

	virtual void GetBidAskPrices(const unsigned int ccyInd, vector<PriceStruct> & bid, vector<PriceStruct> & ask) const = 0;
};

struct functorBID
{

	 const PriceStruct operator () (const std::pair<std::string, BIDASK> ipair){

		return ipair.second.first;
	}
};

struct functorASK
{

	 const PriceStruct operator () (const std::pair<std::string, BIDASK> ipair){

		return ipair.second.second;
	}
};
class FxMarketManager: public IMarketManager
{
	typedef std::map<string, std::shared_ptr<FxDataSink>> MARKETMAP;
	typedef std::map<string,BIDASK> MARKETTOBIDASK;
	typedef std::vector<MARKETTOBIDASK> CCYTOMARKETBIDASK; // ccy pairs are indexed by number see GLOBAL


public:
	FxMarketManager()
	{
		ccyLadderBidPrice.resize(global::g_ccyIndex.size());
	}
	// currently supports any number of markets but this can be made more effecient if number of markets is constant
	
	void UpdateLadder(const std::string & market, const CCYBIDASK & ccyprice)
	{
		unsigned long ccyidx = g_ccyIndex.GetCcyIndex(ccyprice.ccy);
		std::lock_guard<std::mutex> lk(m); // alternative is to lock per index

		if(UpdateLadder(ccyidx, market, ccyprice))
		{
			// todo notify DisplayManager which currencies are updated
			//updatedCcyPairs.push_back(ccyidx);
		}
	}


	// todo initialise markets

	bool UpdateLadder(const unsigned long idx, const string & market, const CCYBIDASK & ccyprice ){

		// find currency pair index for all market providers and update current market price
		MARKETTOBIDASK & marketPrice =  ccyLadderBidPrice[idx];
		CCYTOMARKETBIDASK::value_type::iterator marketIt = marketPrice.find(market);
		bool updated = false;
		if(marketIt==marketPrice.end()){
			marketPrice.insert(std::make_pair(market, BIDASK(ccyprice.bid,ccyprice.ask)));
			return true;
		}
		if(ccyprice.bid.size!=0)
		{
			updated = true;
			(*marketIt).second.first = ccyprice.bid;
		}
		if(ccyprice.ask.size!=0)
		{
			updated = true;
			(*marketIt).second.second = ccyprice.ask;
		}
		return updated;
	}

	void GetBidAskPrices(const unsigned int ccyInd, vector<PriceStruct> & bidV, vector<PriceStruct> & askV) const
	{
		// use transform to populate vectors and shield user from internal handling of the market prices in case it is changed
		const MARKETTOBIDASK & marketPrices =  ccyLadderBidPrice[ccyInd];
		
		std::transform  ( marketPrices.begin(), marketPrices.end(),
			std::back_inserter(bidV),
			functorBID() );

		std::transform  ( marketPrices.begin(), marketPrices.end(),
			std::back_inserter(askV),
			functorASK() );
	}

private:
	  CCYTOMARKETBIDASK ccyLadderBidPrice;
	  MARKETMAP marketToSink;
	  std::mutex m;
};

/*
	bool Update(const std::string & market, const CCYBIDASK & ccyprice){
		MARKETMAP::iterator it = marketToSink.find(market);
		FxDataSink * dataSink;
		if(it==marketToSink.end()){
			dataSink = new FxDataSink;
			marketToSink.insert(make_pair(market, std::make_shared<FxDataSink>(*dataSink)));
		}
		else{
			dataSink = (*it).second.get();
		}
		{
			unsigned long ccyIdx=0;
			if(dataSink->AddToBidAsk(ccyprice.ccy, ccyprice.bid, ccyprice.ask, ccyIdx))
			{
			}
		}

	}

*/


template < int startingRow=1, int colToRead=6>
class FxCsvParser
{

private :	

	// this class does not take even partial ownership only maintains weak references
	istream * instream;
	FxMarketManager* fxData;


public:

	enum CsvFormat
	{
		TIME, MARKET, CCY, BID, BSIZE, ASK, ASIZE, END=ASIZE
	};

	FxCsvParser(CsvFileParser & a, FxMarketManager* pfxData):instream(a.GetStream()), fxData(pfxData){


	}
	template<typename T>
	static T & ConvertToNum(const std::string & a, T & onevalue)
	{

		istringstream sst(a);
		//sst.clear(); // in case there is a parse error in one of the rows
		//sst.str(a);
		sst >> onevalue;
		return onevalue;
	}

	void Read(CsvFileParser::indextype linesToRead = 0)
        
    {
        
  CsvFileParser::indextype linesRead = 0;
    if (linesToRead == 0){
        cout << "CSVPARSER: Reading Entire File in One Go.";
    }
    char a[200];
    string line;

    bool readMore = true;
    static double pTimeStamp =0;
    try
    {

        while( instream->getline(a, 200,'\n') && readMore)
        {
			++linesRead;
			if(linesRead<=startingRow){
				// skip headers
				continue;
			}
			if (linesRead >= linesToRead && linesToRead > 0){ 
				readMore = false;
			}

            line = a;
            // now bust the line up into individual words
			unsigned int numTokensPerLine = 0;
			string read[END+1];
			while(line != "")
			{   
				size_t pos = line.find(',');
				if(pos ==string::npos){
					pos = line.size()-1;
				}

				if (numTokensPerLine <= colToRead){
					read[numTokensPerLine]=line.substr(0,pos);
					cout<<read[numTokensPerLine];
				}else{
					cout<<endl;
				}
					line = line.substr(pos+1);
					++numTokensPerLine;                
			}
			CCYBIDASK ccyprice;
			ccyprice.ccy = read[CCY];
			ConvertToNum(read[BID], ccyprice.bid.price);
			ConvertToNum(read[BSIZE],ccyprice.bid.size);
			ConvertToNum(read[ASK], ccyprice.ask.price);
			ConvertToNum(read[ASIZE], ccyprice.ask.size);
			// Purpose of following is to delay the update to simulate real time arrival of quotes
			// Sleep is not most accurate but sufficient for demonstration of principle.
			// More accurate os timers are available in windows (signal handlers in unix)
			double timestamp = 0;
			ConvertToNum(read[TIME], timestamp);

			MYSLEEP((unsigned int)(timestamp-pTimeStamp));
			pTimeStamp = timestamp;
			
			fxData->UpdateLadder(read[MARKET], ccyprice);
        
        }
    }
    catch (exception& e) // TODO specialise exception handling
    {
        cerr << "Something Went Wrong. Cannot Parse the CSV file."<< e.what();
    }
    //return 2.0;
    
    }


};

