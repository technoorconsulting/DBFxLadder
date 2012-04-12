// zapoutlier.cpp : Defines the entry point for the console application.
//
#include <string>
#include "stdafx.h"
#include "FxDisplayManager.h"
#define CHECK(x) if(x) cout<<"Success";
/*void * print_message_function(void *in){
	char * p = static_cast<char*> (in);


	std::cout<<p<<endl;

}
*/

bool testBasics();
bool testFile(const FxMarketManager & mManager);

int _tmain(int argc, _TCHAR* argv[])
{
	//CsvFileParser csvfile(argv[1], argv[2];
	// StringMatrix::iterator a;
	CsvFileParser csvfile("d:/market.csv", "c:/OUTPUT.csv");
	FxMarketManager mManager;

	FxCsvParser<1,FxCsvParser<>::END> fx(csvfile, &mManager);
	//a = fx.Read(-1);
	testBasics();
	fx.Read(-1);
	testFile(mManager);
	
	/*
	pthread_t thread1, thread2;
	char *message1 = "Thread 1";
	char *message2 = "Thread 2";
	int  iret1, iret2;



	iret1 = pthread_create( &thread1, NULL, print_message_function, (void*) message1);
	iret2 = pthread_create( &thread2, NULL, print_message_function, (void*) message2);
	pthread_join( thread1, NULL);
	pthread_join( thread2, NULL); 
	*/


	return 0;
}



bool testBasics(){
	FxMarketManager mManager;
	PriceStruct bidprice; bidprice.price=99; bidprice.size=1;
	CCYBIDASK ccyprice=CCYBIDASK("GBPUSD", bidprice,bidprice);
	mManager.UpdateLadder("FxMart", ccyprice);
	mManager.UpdateLadder("AutoFx", ccyprice);

	FxDisplayManager dispMan(&mManager);
	dispMan.DisplayAllCcy();

	global::CCYIDX idx=global::g_ccyIndex.GetCcyIndex("GBPUSD");
	FxDisplayManager::PRICESIZEMAP pMap=FxDisplayManager::PRICESIZEMAP(new FxDisplayManager::PRICESIZEMAP::element_type);
	FxDisplayManager::PRICESIZEMAP pMapAsk=FxDisplayManager::PRICESIZEMAP(new FxDisplayManager::PRICESIZEMAP::element_type);

	dispMan.CalculatePriceSize(idx, pMap, pMapAsk);

	const PriceStruct & retPriceSize = (*pMap->begin());
	CHECK(retPriceSize.price==bidprice.price && retPriceSize.size==(bidprice.size*2));


	return true;
}

bool testFile(const FxMarketManager & mManager){

	FxDisplayManager dispMan(&mManager);
	dispMan.DisplayAllCcy();

	return true;

}