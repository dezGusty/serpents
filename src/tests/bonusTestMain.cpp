#include <stdio.h>

#include "../utils/GTrace.h"
#include <iostream>
#include <conio.h>
#include <windows.h>

#include "../model/Bonus.h"

using namespace std;
using namespace Serpents;


void displayInstructions()
{
	cout<<"Enter single chars to interact"<<endl
		<<"List of char actions:"<<endl
		<<"` : see this message"<<endl
		<<"r : create a random bonus"<<endl
		<<"x : exit the application"<<endl;
}

void displaybonuses( BonusManagerBase &manager
					)
{
	cout<<"List of bonuses display begin................."<<endl;
	for( BonusBaseList::iterator it = manager.getBonuses().begin(); it!= manager.getBonuses().end(); ++it )
	{
		BonusBase *bb = &*it;
		if( bb )
		{
			cout<<bb->getLinkToPrototype()->getInternalName() <<" at "<<bb->getLocation().x
				<<","<<bb->getLocation().y<<" has time="<<bb->getTimeToLive()<<endl ;
		}
	}
	cout<<"List of bonuses display end......................"<<endl;
}


void displayprotos( BonusPrototypeBase * proto )
{
	if( proto == NULL )
		return;
	cout<<"----------------------"<<endl;
	cout<<"Name="<<proto->getInternalName()<<endl;
	for(BonusRewardList::iterator it= proto->getRewards().begin(); it!= proto->getRewards().end(); ++it )
	{
		cout<<"Type=";
		switch( it->getType() )
		{
		case BRT_HealthModifier:
			cout<<"Health";
			break;
		case BRT_HungerModifier:
			cout<<"Hunger";
			break;
		case BRT_LengthModifier:
			cout<<"Len";
			break;
		case BRT_ScoreModifier:
			cout<<"Score";
			break;
		case BRT_UserDefined:
			cout<<"(U)";
			cout<<it->getId();
			break;
		default:
			cout<<"Unknown";
			break;
		}
		cout<<"\t\t";
		cout<<"Value="<<it->getValue()<<endl;
		
	}
	cout<<"----------------------"<<endl;
}


void main()
{
	cout<<"Bonus tests..."<<endl;

	GSTARTTRACING("xtrace.txt", 10);
	GTRACE(1, "Bonus tests begin.");
	GTRACE(1, "-----------------.");

	BonusManagerBase manager;
	cout<<"Created bonus manager"<<endl;

	cout<<"Creating bonus prototypes"<<endl;


	{
		BonusPrototypeBase * proto1;
		BonusReward r( BRT_HungerModifier, "", -0.1 );
		GTRACE(4, "creating a proto");
		proto1 = manager.createPrototype("apple");
		GTRACE(4, "adding a reward");
		proto1->addReward(r);
		r.setType( BRT_LengthModifier );
		r.setValue( 1.0 );
		GTRACE(4, "adding a reward");
		proto1->addReward(r);
		GTRACE(5, "creating custom reward");
		BonusReward customR( BRT_UserDefined, "armor", 4 );
		proto1->addReward( customR );

		GTRACE(4, "adding proto to mgr");
		manager.addBonusPrototype( *proto1 );
		cout<<"Added prototype to manager:"<<endl;
		displayprotos( proto1 );
		delete proto1;

		proto1 = manager.createPrototype("orange");
		proto1->addReward( BonusReward(BRT_HungerModifier, "", -0.2) );
		proto1->addReward( BonusReward(BRT_ScoreModifier, "", 5) );
		proto1->addReward( BonusReward(BRT_LengthModifier, "", 0.5) );
		proto1->addReward( BonusReward(BRT_HealthModifier, "", 0.01) );
		manager.addBonusPrototype( *proto1 );
		cout<<"Added prototype to manager:"<<endl;
		displayprotos( proto1 );
		delete proto1;

		proto1 = manager.createPrototype("gold coin");
		proto1->addReward( BonusReward(BRT_HealthModifier, "", -0.01) );
		proto1->addReward( BonusReward(BRT_ScoreModifier, "", 20) );
		proto1->addReward( BonusReward(BRT_UserDefined, "magnetism", 1) );
		manager.addBonusPrototype( *proto1 );
		cout<<"Added prototype to manager:"<<endl;
		displayprotos( proto1 );
		delete proto1;
	}

	bool bOver=false;
	while( ! bOver )
	{
		while( !_kbhit() )
		{
			Sleep(5);
			manager.onTimeGoesBy(5);
		}
		char c;
		c = _getch();
		if( c=='x' )
		{
			bOver = true;
		}
		else if( c=='`' )
		{
			displayInstructions();
		}
		else if( c=='r' )
		{
			BonusPrototypeBase * tempProto = manager.getRandomBonusType();
			DPOINT p;
			p.x = GRandom::getInstancePtr()->getValueBetween(0, 100);
			p.y = GRandom::getInstancePtr()->getValueBetween(0, 100);
			manager.spawnNewBonus( p, tempProto );
			cout<<"Spawned new bonus"<<endl;
			displaybonuses(manager);
		}
	}

	cout<<"Done."<<endl;
	GTRACE(4, "exiting scope");
}