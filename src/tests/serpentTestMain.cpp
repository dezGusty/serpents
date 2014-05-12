#include "../model/SerpentBase.h"

using namespace std;
using namespace Serpents;

class MySerp : public SerpentBase
{
	BodyPartBase * createBodyPart(int x)
	{
		return new BodyPartBase();
	}
};


void main()
{
	cout<<"begin of serpent test."<<endl;
	GSTARTTRACING("ztrace.txt", 9);
	MySerp srp;


	srp.initialize();
	srp.addParts(5);
	srp.move(100);
	srp.removeParts(1);
	srp.move(100);
	srp.addParts(5);
	srp.removeParts(4);

	srp.getBodyPart(1);
	srp.setSpawnPosition(10, 10);

	srp.move(100);
	cout<<"done."<<endl;
}