//============================================================================
// Name        : DB.cpp
// Author      : Salman Marvasti
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
//#include "stdafx.h
#include "FxDisplayManager.h"
#define CHECK(x) if(x) cout<<"Success";
#define MYSLEEP(y) usleep(y*1000);
using namespace std;

/*void * print_message_function(void *in){
	char * p = static_cast<char*> (in);


	std::cout<<p<<endl;

}
*/

bool testBasics();
void testFile(FxMarketManager & mManager);
FxMarketManager mManager;

int main(int argc, char* argv[])
{
	//CsvFileParser csvfile(argv[1], argv[2];
	CsvFileParser csvfile("d:/market.csv", "c:/OUTPUT.csv");

	FxCsvParser<1,FxCsvParser<>::END> fx(csvfile, &mManager);
	//a = fx.Read(-1);
//	testBasics();
	std::thread t(testBasics);
	std::thread t1(std::bind(testFile, std::ref(mManager)));
	//fx.Read(-1);
	t.join();
	t1.join();
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
	PriceStruct bidprice; bidprice.price=9.9; bidprice.size=10000;
	CCYBIDASK ccyprice=CCYBIDASK("GBPUSD", bidprice,bidprice);
	mManager.UpdateLadder("FxMart", ccyprice);
	mManager.UpdateLadder("AutoFx", ccyprice);

	FxDisplayManager dispMan(&mManager);
	// main loop of thread
	for(int i=0;i<1000;++i)
	{
		MYSLEEP(100)
		std::cout<<"thread 1";
		dispMan.DisplayAllCcyAlt();
	}

	global::CCYIDX idx=global::g_ccyIndex.GetCcyIndex("GBPUSD");
	FxDisplayManager::PRICESIZEMAP pMap=FxDisplayManager::PRICESIZEMAP(new FxDisplayManager::PRICESIZEMAP::element_type);
	FxDisplayManager::PRICESIZEMAP pMapAsk=FxDisplayManager::PRICESIZEMAP(new FxDisplayManager::PRICESIZEMAP::element_type);

	dispMan.CalculatePriceSize(idx, pMap, pMapAsk);

	const PriceStruct & retPriceSize = (*pMap->begin());
	CHECK(retPriceSize.price==bidprice.price && retPriceSize.size==(bidprice.size*2));


	return true;
}
void testFile(FxMarketManager & mManager){

	FxDisplayManager dispMan(&mManager);
//	dispMan.DisplayAllCcy();
	MYSLEEP(1000);
	std::cout<<"Final Success"<<std::flush;
	MYSLEEP(2000);
	PriceStruct bidprice; bidprice.price=9; bidprice.size=100000000;
	PriceStruct ask=bidprice;
	ask.price=11;
	CCYBIDASK ccyprice=CCYBIDASK("GBPUSD", bidprice,ask);
	mManager.UpdateLadder("FxMart", ccyprice);
	MYSLEEP(5000);
	mManager.UpdateLadder("FxFart", ccyprice);

	dispMan.DisplayAllCcy();

	// return true
}