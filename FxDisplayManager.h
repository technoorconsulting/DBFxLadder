#pragma once
#include "FxParser.h"
#include <numeric>
#include <ctime>
#include <set>

class Logger{


	void log(std::string & log)
	{
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    cout << (now->tm_year + 1900) << '-' 
         << (now->tm_mon + 1) << '-'
         <<  now->tm_mday
         << log<<endl;


	}

};

// ****************************************************************************
// FxDisplayManager
// used by FxDisplayManager
// ****************************************************************************

struct merge_price_funct
{
	bool operator()(const PriceStruct &  l, const PriceStruct &  r) const
	{
		if(l.price<r.price){
			return true;
		}
		return false;
	}
};

class FxDisplayManager{

public:

	typedef std::shared_ptr<std::set<PriceStruct>> PRICESIZEMAP;
	void AddChangedCcy(unsigned long ccyidx)
	{

	}


	
	void CalculatePriceSize(const unsigned long ccyid , PRICESIZEMAP bids, PRICESIZEMAP asks){

			std::vector<PriceStruct> bidPrices;
			std::vector<PriceStruct> askPrices;
			fxMarket->GetBidAskPrices(ccyid,bidPrices, askPrices);
			AccumulateSizeInPriceQuotes(*bids.get(), bidPrices);
			AccumulateSizeInPriceQuotes(*asks.get(), askPrices);

	}
	
	void DisplayAllCcy()
	{
		for(unsigned long id=0;id<g_ccyIndex.size();++id)
		{
			std::stringstream strout;
			const std::string ccyStr =global::g_ccyIndex.GetCcyPair(id);
			std::vector<PriceStruct> bidPrices;
			std::vector<PriceStruct> askPrices;
			fxMarket->GetBidAskPrices(id,bidPrices, askPrices);
			std::sort(bidPrices.begin(), bidPrices.end());
			std::sort(askPrices.begin(), askPrices.end());
			std::shared_ptr<std::map<PriceStruct,vector<unsigned long>>> pVec;
			if(!bidPrices.empty()||!askPrices.empty())
			{
				strout<<ccyStr<<" - BID:";
				DisplayVector(strout, bidPrices);
				strout<<" ASK:";
				DisplayVector(strout, askPrices);
				strout<<std::endl<<std::endl;
				std::cout<<strout.str(); // todo remove

			}
		}
	}

	FxDisplayManager(const IMarketManager * fxMark, int maxNumberPrice=3):N(maxNumberPrice),fxMarket(fxMark)
	{
		if(fxMark==0) throw exception(std::exception("Invalid initialistion"));
	}

	~FxDisplayManager(){

	}
private:
	template<char eC=','>
	struct display_price_func
{
	std::stringstream & str;
	display_price_func(std::stringstream & str):str(str)
	{
	}
    void operator()(const PriceStruct & cei) const
    {
        if (cei.size)
			str<<" "<<cei.size<<"@"<<cei.price<<",";   
	}
};
	void DisplayVector (std::stringstream & str, PRICESIZEMAP vec)
	{
		display_price_func<> dFunc(str);
	
		std::for_each (vec->begin(), --vec->end(), dFunc);
		display_price_func<' '> dFuncEnd(str);
		dFuncEnd(*(--vec->end()));
	}
	void AccumulateSizeInPriceQuotes(PRICESIZEMAP::element_type & mapOut, vector<PriceStruct> & inVec){

			PRICESIZEMAP::element_type::const_iterator mend = mapOut.end();
			for(std::vector<PriceStruct>::iterator it=inVec.begin();it!=inVec.end();++it)
			{
				PRICESIZEMAP::element_type::iterator prevEntry;
				// accumulate the sizes
				if((prevEntry=mapOut.find(*it))!=mend)
				{
					(*it).size+=(*prevEntry).size;
					mapOut.erase(prevEntry);
				}

				mapOut.insert(*it);
			}
	}
	void DisplayVector (std::stringstream & str, const std::vector<PriceStruct> &vec)
	{
		if(vec.empty()) return;
		double prev=vec[0].price;
		int prevstartidx=0;
		unsigned int totalSize=vec[0].size;
		
		// group together sizes
		for(std::size_t i=1;i<vec.size();++i){
			if(prev==vec[i].price)
				totalSize += vec[i].size;
			else{
				log(str, totalSize, prev);
				totalSize = vec[i].size;
				prev = vec[i].price;
			}

		}
		log(str, totalSize, prev);
				
	}

	void log(std::stringstream & str, unsigned long totalSize, double price){
		str<<" "<<(double)totalSize/1000000<<"@"<<price<<",";

	}
private:
	vector<std::string> lastUpdate;
	vector<unsigned int> updatedCcyIdx;
	const IMarketManager * fxMarket;
	const int N;// number of prices to display per currency pair
};
