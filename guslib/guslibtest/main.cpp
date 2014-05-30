#include <guslib/trace/trace.h>

#include <guslib/system/timer.h>
#include <guslib/system/random.h>

#include <guslib/util/filehelper.h>

#include "SuperClass.h"

#include <iostream>
using namespace std;

#include <windows.h>

void main ()
{
  try
  {
	  GSTARTTRACING ("ztraceguslibtest.txt", 10);

	  cout << "This project is linked against the generated library guslib/guslib_D.lib" << endl;
	  cout << "This project is linked against the generated library guslibTestDLL/guslibTestDLL_D.lib" << endl;
	
	  cout << "It will require the presence of the guslibTestDLL/guslibTestDLL_D.dll" << endl;
	  cout << endl;
	  cout << "It shows how you can use the same (tracing) singleton in the DLL and in the executable" << endl;
	  cout << "Check the ztrace.txt for the exact output" << endl;
	  cout << endl;
	  cout << endl;

	  guslib::Timer myTimer;
	  myTimer.reset ();
	  cout << "Elapsed time: " << myTimer.renew () << endl;


	  GTRACE (1, "hello");
	  GTRACE (2, "hello2");
	  GTRACE (3, "hello3");

	  cout << "Random values: " << endl;
	  cout << guslib::Random::getPtr()->getValueUpTo (100) << endl;
	  cout << guslib::Random::getPtr()->getValueUpTo (100) << endl;
	  cout << guslib::Random::getPtr()->getValueUpTo (100) << endl;
	  cout << guslib::Random::getPtr()->getValueUpTo (100) << endl;
	  cout << guslib::Random::getPtr()->getValueBetween (11, 50) << endl;
	  cout << "(end)" << endl;

	  gustest::SuperClass myClass ("john");
	  GTRACE (7, "sub-1");
	  {
		  gustest::SuperClass myClass ("george");
	  }
	  GTRACE (7, "sub-2");

	  GTRACE (3, "Unit Tests for file helper ----------------------------- begin");
	  std::string tempString ("d:\\work\\GustysSerpents\\Serpents\\Game\\data\\");
	  GTRACE (3, "Src = [" << tempString << "], dest (1,1)=[" << guslib::filehelper::EnsureTrailingSeparator (tempString, true, true) << "]");
	  GTRACE (3, "Src = [" << tempString << "], dest (1,0)=[" << guslib::filehelper::EnsureTrailingSeparator (tempString, true, false) << "]");
	  GTRACE (3, "Src = [" << tempString << "], dest (0,1)=[" << guslib::filehelper::EnsureTrailingSeparator (tempString, false, true) << "]");
	  GTRACE (3, "Src = [" << tempString << "], dest (0,0)=[" << guslib::filehelper::EnsureTrailingSeparator (tempString, false, false) << "]");
	  tempString = "d:/work/GustysSerpents/Serpents/Game/data/";
	  GTRACE (3, "Src = [" << tempString << "], dest (1,1)=[" << guslib::filehelper::EnsureTrailingSeparator (tempString, true, true) << "]");
	  GTRACE (3, "Src = [" << tempString << "], dest (1,0)=[" << guslib::filehelper::EnsureTrailingSeparator (tempString, true, false) << "]");
	  GTRACE (3, "Src = [" << tempString << "], dest (0,1)=[" << guslib::filehelper::EnsureTrailingSeparator (tempString, false, true) << "]");
	  GTRACE (3, "Src = [" << tempString << "], dest (0,0)=[" << guslib::filehelper::EnsureTrailingSeparator (tempString, false, false) << "]");
	  tempString = "d:\\work\\GustysSerpents\\Serpents\\Game\\data";
	  GTRACE (3, "Src = [" << tempString << "], dest (1,1)=[" << guslib::filehelper::EnsureTrailingSeparator (tempString, true, true) << "]");
	  GTRACE (3, "Src = [" << tempString << "], dest (1,0)=[" << guslib::filehelper::EnsureTrailingSeparator (tempString, true, false) << "]");
	  GTRACE (3, "Src = [" << tempString << "], dest (0,1)=[" << guslib::filehelper::EnsureTrailingSeparator (tempString, false, true) << "]");
	  GTRACE (3, "Src = [" << tempString << "], dest (0,0)=[" << guslib::filehelper::EnsureTrailingSeparator (tempString, false, false) << "]");
	  tempString = "d:/work/GustysSerpents/Serpents/Game/data";
	  GTRACE (3, "Src = [" << tempString << "], dest (1,1)=[" << guslib::filehelper::EnsureTrailingSeparator (tempString, true, true) << "]");
	  GTRACE (3, "Src = [" << tempString << "], dest (1,0)=[" << guslib::filehelper::EnsureTrailingSeparator (tempString, true, false) << "]");
	  GTRACE (3, "Src = [" << tempString << "], dest (0,1)=[" << guslib::filehelper::EnsureTrailingSeparator (tempString, false, true) << "]");
	  GTRACE (3, "Src = [" << tempString << "], dest (0,0)=[" << guslib::filehelper::EnsureTrailingSeparator (tempString, false, false) << "]");
		
	  GTRACE (3, "Unit Tests for file helper ----------------------------- end");


	  cout << "Elapsed time: " << myTimer.renew () << endl;
	  GTRACE (7, "ending");
  }
  catch (const std::exception& e)
  {
    std::cout << "Caught exception: " << e.what() << std::endl;
  }
}