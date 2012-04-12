#pragma once
#include <exception>
#include <stdexcept>


struct PriceStruct{
	double price;
	unsigned long size;

	// allow correct sorting in stl containers using default operator<
	bool operator<(const PriceStruct & r) const{
		return price<r.price;
	}
	static bool assign(PriceStruct & out, const PriceStruct & in)
	{
		if(in.size!=0)
		{
			out.price = in.price;
			out.size = in.size;
			return true;
		}

		return false;
	}
};

struct CcyStruct
{
	std::string ccy;
	PriceStruct bid;
	PriceStruct ask;
	CcyStruct():ccy(""),bid(),ask(){}
	CcyStruct(const std::string & ccy, const PriceStruct & bid, const PriceStruct & ask):ccy(ccy),bid(bid), ask(ask){}
};

typedef std::pair<PriceStruct, PriceStruct> BIDASK; // used where currency index is not needed
typedef CcyStruct CCYBIDASK;
// Global static declarations
namespace global{
	typedef unsigned long CCYIDX;
	class GlobalCCYIndex{
	public:
		typedef std::map< string,unsigned long> CCYINDEXMAP;
		CCYINDEXMAP g_ccypairToCod;
		string * pKeys;
		GlobalCCYIndex(){
			static  unsigned long values[] = { 0,1,2,3,4,5,6,7,8,9 };
			static  std::string keys[] = { "EURUSD",
				"GBPUSD",
				"EURGBP",
				"USDCAD",
				"AUDUSD",
				"EURCHF",
				"EURJPY",
				"NZDUSD",
				"USDCHF",
				"USDJPY"};
			pKeys = keys;
			g_ccypairToCod.insert(make_pair(keys[0], values[0]));
			g_ccypairToCod.insert(make_pair(keys[1], values[1]));
//		    g_ccypairToCod = helper::make_map<std::string, unsigned long, 10>(keys,values);

		}

		std::size_t size() const
		{
			return g_ccypairToCod.size();
		}
		unsigned long GetCcyIndex(const std::string & s) const // throw specification not supported by windows
		{
			CCYINDEXMAP::const_iterator it=g_ccypairToCod.find(s);
			if(it!=g_ccypairToCod.end()){
				return (*it).second;
			}
			throw std::runtime_error("Error CCYPair not found in index. Please alert support.");
		}
		const std::string & GetCcyPair(unsigned long index) const
		{
			return *(pKeys+index);
		}
	};
	// End Global
	static GlobalCCYIndex g_ccyIndex;
}
