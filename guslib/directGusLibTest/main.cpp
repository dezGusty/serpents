#include <guslib/trace/trace.h>

#include <guslib/system/timer.h>

#include <guslib/util/consolemenu.h>

#include <guslib/common/smartptr.h>

#include <guslib/system/commandline.h>

#include <guslib\util\Configuration.h>

using namespace std;

#include <windows.h>
#include "menutesting.h"
#include <guslib\util\StringUtil.h>
#include <guslib\util\Config.h>


class C1
{
public:
  virtual void name () { cout << "C1" << endl; }
};

class C2:
  public C1
{
public:
  virtual void name () { cout << "C2" << endl; }
};



void getArrayOfStrings (std::string ** vec, int &itemCount)
{
  GTRACE (5, "Getting list of program arguments");
  std::vector <std::string> tempVector;

  tempVector.push_back ("ana");
  tempVector.push_back ("are");
  tempVector.push_back ("mere");

  itemCount = tempVector.size ();
  *vec = new std::string [itemCount];
  for (int i = 0; i < itemCount; ++ i)
  {
    (*vec)[i] = tempVector[i];
  }
}


void stringUtilSpeedTest ()
{
  guslib::Timer myTimer;
  myTimer.reset ();
  int nOpCount = 1000000;

  cout << "Making int to string parse ops : " << nOpCount << endl;
  for (int i = 0 ; i <= nOpCount; ++ i)
  {
    std::string sTemp = stringutil::IntToString (i);
  }
  cout << "Elapsed time: " << myTimer.renew () << endl;
}



void main ()
{

  guslib::AppRestarter::getPtr ()->setRestartParameter ("--restart");

  if( guslib::AppRestarter::getPtr()->restartProcessStarted() )
  {
    guslib::AppRestarter::getPtr()->waitForPreviousProcessToFinish();
  }

  // Program description.
  cout << "Simple example of using the guslib code directly. " << endl;
  cout << "The project does not require the \".lib\" file to compile" << endl;
  cout << "The project does not require the \".dll\" file to run" << endl;
  cout << endl;

  // Start tracing.
  GSTARTTRACING ("zdirecttrace.txt", 10);

  // Test the argument parsing. Please note that the arguments are supplied in this case directly (hardcoded), so it's not the actual command line.
  std::vector <std::string> myList;
  myList = guslib::CommandLine::getListOfArguments ("\"d:/gus/workgus/Cpp/GustysSerpents/Serpents/Game/SerpentsVS2008_Debug.exe\" --test \"blabla1 1233\" -ff special");

  for (std::vector <std::string>::iterator it = myList.begin ();
    it != myList.end (); ++ it)
  {
    cout << "[" << *it << "]" << endl;
  }
  cout << "----------------" << endl;

  myList = guslib::CommandLine::getListOfArguments ("\"d:/gus/workgus/Cpp/GustysSerpents/Serpents/Game/SerpentsVS2008_Debug.exe\"");

  for (std::vector <std::string>::iterator it = myList.begin ();
    it != myList.end (); ++ it)
  {
    cout << "[" << *it << "]" << endl;
  }
  cout << "----------------" << endl;
  
  myList = guslib::CommandLine::getListOfArguments ("\"SerpentsVS2008_Debug.exe\"  --test \"blabla1 1233\" -ff special");

  for (std::vector <std::string>::iterator it = myList.begin ();
    it != myList.end (); ++ it)
  {
    cout << "[" << *it << "]" << endl;
  }
  cout << "----------------" << endl;

  std::string * vec = 0;
  int miniCount = 0;
  getArrayOfStrings (&vec, miniCount);
  //guslib::CommandLine::getArrayOfProgramArguments (vec, miniCount);
  cout << "miniCount=" << miniCount << endl;
  for (int i = 0; i < miniCount; ++ i)
  {
    cout << vec[i] << endl;
  }
  
  cout << "Done with argument list." << endl;

  guslib::Timer myTimer;
  myTimer.reset ();
  cout << "Elapsed time: " << myTimer.renew () << endl;

  int nCount = 50; // Feel free to increase this to 50'000
  cout << "Writing " << nCount << " entries to the trace file" << endl;
  for (int i = 0; i < nCount; ++ i)
  {
    GTRACE (1, "This is test number " << i << " out of " << nCount << ".");
  }

  cout << "Elapsed time: " << myTimer.renew () << endl;

  try
  {
    GSharedPtr <C1> myPtr (new C2 ());
    myPtr->name ();
    GSharedPtr <C2> subitem;
    subitem = boost::static_pointer_cast <C2, C1> (myPtr);


    GSmartPtr <C1> y1 (new C2 ());
    myPtr->name ();
    GSmartPtr <C2> y2;
    y2 = static_cast <GSmartPtr <C2>> (y1);
    y2->name ();
  }
  catch (std::exception & e)
  {
    GTRACE (1, "ex: " << e.what ());
  }

  {
    cout << "Getting a configuration from [../guslib/directGusLibTest/gus.ini] if possible" << endl;
    Config::init ();
    Configuration config;

    config.load("../guslib/directGusLibTest/gus.ini");
    // alternatively: config.load (IniConfigLoader("gus.ini"));
    int userCount = config["general"]["userCount"].getAsInt();
    cout << "- Got general->userCount as " << userCount << endl;

    vector <string> plugins = config["video"]["pluginlist"].getAsStringList();
    int nPluginCount (1);
    for (vector<string>::iterator it = plugins.begin(); it != plugins.end(); ++ it)
    {
      cout << "plugin " << nPluginCount ++ << " is [" << *it << "]" << endl;
    }

    config["general"]["userCount"].setAsInt(userCount + 1);

    plugins.push_back ("Catalyst Optimized Profile");
    config["video"]["pluginlist"].setAsVector(plugins);
    config.saveAs ("../guslib/directGusLibTest/gus2.ini");
  }
  GTRACE (7, "ending");
  cout << /*"time:" << */ guslib::SysClock::get () << "; temp" << endl;
  GSTOPTRACING ();

  guslib::AppRestarter::getPtr()->finishRestart();


//  stringUtilSpeedTest ();
}