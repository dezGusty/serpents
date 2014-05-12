
#include "../model/ScoringTable.h"

using namespace std;
using namespace Serpents;


void displayScoreTable(ScoringTable & table)
{
	cout<<"--------- displaying scoring table ------------"<<endl;
	for( int i=0; i<table.getNumberOfEntries(); i++ )
	{
		cout<<table.getEntryPtrAt(i)->toString()<<endl;
	}

}



void main()
{
	cout<<"Scoring test..."<<endl;

	LongScoringTable table;

	table.loadFromFile("d:/gus/workgus/Cpp/GustysSerpents/Serpents/Game/data/records.txt");
	displayScoreTable(table);

	table.createScoreEntryForCurrentGame();
	table.getCurrentEntryPtr()->setName("Gustav");
	//((LongScoreValue*)table.getCurrentEntryPtr()->getScore())->setValue(333);
	table.sortList(true, false);
	//table.save(GusString("d:/gus/workgus/Cpp/GustysSerpents/Serpents/Game/data/records2.txt"));
	//displayScoreTable(table);

	table.updateCurrentScore( 400 );
	table.save(GusString("d:/gus/workgus/Cpp/GustysSerpents/Serpents/Game/data/records3.txt"));
	displayScoreTable(table);
	table.sortList(true, false);
	displayScoreTable(table);
	//ScoreEntry * myEntry = table.createScoreEntry();

	//myEntry->setAgeInMillis(12231243);
	//myEntry->setName("User1");
	//myEntry->setScore(new LongScoreValue(1233));
	//GusString gs = myEntry->toString();
	//cout<<gs<<endl;

	//ScoreEntry * myEntry2 = table.createScoreEntry();
	//myEntry2->loadFromString(gs);
	//cout<<myEntry2->toString()<<endl;

	//delete myEntry;
	cout<<"Done."<<endl;
}