#pragma once
using namespace guslib;

#include <guslib/util/consolemenu.h>
#include <conio.h>
#include <guslib/system/apprestart.h>
#include <iostream>

using namespace std;
void displayInstructions()
{
  cout<<"Enter single chars to interact"<<endl
    <<"List of char actions:"<<endl
    <<"` : see this message"<<endl
    <<"w : menu up"<<endl
    <<"s : menu down"<<endl
    <<"e : action (enter sub-menu, or enter/exit option edit mode)"<<endl
    <<"a : menu option previous value"<<endl
    <<"d : menu option next value"<<endl
    <<"q : go back to previous menu level"<<endl
    <<"x : exit the application"<<endl
    <<"ESC: exit the application"<<endl;
}


class MyListener : public GMenuListener
{
public:
  MyListener():GMenuListener() {bOver = false;}
  virtual ~MyListener(){}
  virtual void OnMenuAction(GMenuItem *item)
  {
    cout << "Menu action" << endl;
    if( item->getName() == "exit" )
    {
      bOver = true;
    }
    else if (item->getName () == "playlist1")
    {
      SoundManager::getPtr()->getMusicPlayer()->switchToPlaylist("game");
    }
    else if (item->getName () == "playlist2")
    {
      SoundManager::getPtr()->getMusicPlayer()->switchToPlaylist("menu");
    }
    else if (item->getName () == "pause")
    {
      SoundManager::getPtr()->getMusicPlayer()->pause ();
    }
    else if (item->getName () == "next1")
    {
      SoundManager::getPtr()->getMusicPlayer()->nextTrack ();
    }
    else if (item->getName () == "stop")
    {
      SoundManager::getPtr()->getMusicPlayer()->stop ();
    }
    else if (item->getName () == "start")
    {
      SoundManager::getPtr()->getMusicPlayer()->play ();
    }
    else if (item->getName () == "volup")
    {
      SoundManager::getPtr ()->getRepository ()->setVolumeForCategory(GSC_Master,
        SoundManager::getPtr ()->getRepository ()->getVolumeForCategory (GSC_Master) + (SoundVolume)0.1);
    }
    else if (item->getName () == "voldn")
    {
      SoundManager::getPtr ()->getRepository ()->setVolumeForCategory(GSC_Master,
        SoundManager::getPtr ()->getRepository ()->getVolumeForCategory (GSC_Master) - (SoundVolume)0.1);
    }
    else if (item->getName () == "randsnd")
    {
      std::string sTemp = SoundManager::getPtr ()->getSoundGroup("swordsman selected")->getRandomSound();
      SoundManager::getPtr ()->getRepository ()->getSound (sTemp)->play2D ();
    }
    else if (item->getName () == "randsndsingle")
    {
      std::string sTemp = SoundManager::getPtr ()->getSoundGroup("single sound selection")->getRandomSound();
      SoundManager::getPtr ()->getRepository ()->getSound (sTemp)->play2D ();
    }
    else if (item->getName () == "randsndleft")
    {
      std::string sTemp = SoundManager::getPtr ()->getSoundGroup("swordsman selected")->getRandomSound();
      SoundManager::getPtr ()->getRepository ()->getSound (sTemp)->play3D (-108.0, -1.0, 1.2);
    }
    else if (item->getName () == "randsndright")
    {
      std::string sTemp = SoundManager::getPtr ()->getSoundGroup("swordsman selected")->getRandomSound();
      SoundManager::getPtr ()->getRepository ()->getSound (sTemp)->play3D (108.0, 1.0, 1.2);
    }
    
  }
  virtual bool OnMenuBack(GMenuItem *item)
  {
    cout<<"Back button pressed:"<<item->getCaption()<<endl;
    return true;
  }
  virtual void OnMenuShouldRedraw( GMenu * menu )
  {
  }
  virtual void OnMenuSelectionChange( GMenuItem *item )
  {
    cout<<"menu selection changed for "<<item->getCaption()<<endl;
    cout<<"new position is "<<item->getSelectedChildIndex()<<endl;
  }
  virtual void OnMenuDepthChange( GMenu * menu, bool fromTop )
  {
    if( menu == NULL )
    {
      cout << "Can't change depth; top level!"<<endl;
    }
    else
    {
      cout << "Depth was changed, currently is "<< menu->getActive()->getItemDepth()
        <<" from "<<(fromTop? "top" : "bottom")<<endl;
    }
  }
  virtual void OnMenuOptionValueChange( GMenuItem *optionGroup, GMenuItem *option )
  {
    cout << "Option for " << optionGroup->getName() << " changed to " << option->getName()<<endl;
  }
  virtual void OnMenuOptionStartEdit( GMenuItem *optionGroup, GMenuItem *option )
  {cout<<"Started editing "<<optionGroup->getName()<<", "<<option->getName()<<endl;
  }
  virtual void OnMenuOptionStopEdit( GMenuItem *optionGroup, GMenuItem *option, bool bCanceled )
  {cout<<(bCanceled?"Cancelled":"Accepted")<<" option change for "<<optionGroup->getName()<<", "<<option->getName()<<endl;
  }

  bool bOver;
};


void drawMenu(GMenu *menu)
{
  GMenuItemList vect = menu->getVisible()->getChildren();
  std::system("cls");
  cout <<"--------------"<<menu->getVisible()->getCaption()<<"------------ (Press [`] for help)"<<endl;
  for(GMenuItemList::iterator it = vect.begin(); it!= vect.end(); ++it)
  {
    if( (*it)->isSelected() )
    {
      cout << "(X)" << (*it)->getCaption()<<"; type="<< (*it)->getType()<<"... lvl="<< (*it)->getItemDepth();
    }
    else
    {
      cout << "( )" << (*it)->getCaption()<<"; type="<< (*it)->getType();
    }
    if( (*it) == menu->getActive() )
    {
      cout<<"(option edit, press left-right cmd)";
    }
    cout<<endl;
  }
}


void menutest ()
{
  // Create a menu to allow the application's control.
  GMenu myMenu;

  // Prepare the actions. For simplicity, don't use any subfolders in this example. Just put everything in the main list.
  GMenuAction starter (&myMenu, "start", "Start playlist play");
  GMenuAction stp (&myMenu, "stop", "Stop playlist play");

  GMenuAction rndsnd (&myMenu, "randsnd", "Play a random loaded sound");
  GMenuAction rndsndsingle (&myMenu, "randsndsingle", "Play a single instance loaded sound");
  GMenuAction rndsndLeft (&myMenu, "randsndleft", "Play a random sound (LEFT)");
  GMenuAction rndsndRight (&myMenu, "randsndright", "Play a random sound (RIGHT)");

  GMenuAction pl1 (&myMenu, "playlist1", "Switch to playlist 1");
  GMenuAction pl2 (&myMenu, "playlist2", "Switch to playlist 2");
  GMenuAction ps (&myMenu, "pause", "Pause; use play to resume");
  GMenuAction nxt (&myMenu, "next1", "Next track in current list");
  GMenuAction volup (&myMenu, "volup", "Volume Up");
  GMenuAction voldn (&myMenu, "voldn", "Volume Down");
  
  myMenu.getRoot ()->addChild (&starter);
  myMenu.getRoot ()->addChild (&stp);

  myMenu.getRoot ()->addChild (&rndsnd);
  myMenu.getRoot ()->addChild (&rndsndsingle);
  myMenu.getRoot ()->addChild (&rndsndLeft);
  myMenu.getRoot ()->addChild (&rndsndRight);

  myMenu.getRoot ()->addChild (&pl1);
  myMenu.getRoot ()->addChild (&pl2);
  myMenu.getRoot ()->addChild (&ps);
  myMenu.getRoot ()->addChild (&nxt);
  myMenu.getRoot ()->addChild (&volup);
  myMenu.getRoot ()->addChild (&voldn);

  // Specify that pressing the "down" key while at the last item will move you to the first item.
  myMenu.getRoot ()->setSelectionWrapping(true);

  // Also define the exit command.
  GMenuAction ex (&myMenu, "exit", "Exit");
  myMenu.getRoot ()->addChild (&ex);

  // Create a listener for the events in the menu object.
  MyListener *lst = new MyListener ();
  myMenu.setListener (lst);

  // Set-up the keys that control the actions in the menu.
  myMenu.setKeyMap (cmdMenuUp, 'w');
  myMenu.setKeyMap (cmdMenuDown, 's');
  myMenu.setKeyMap (cmdActionLeft, 'a');
  myMenu.setKeyMap (cmdActionRight, 'd');
  myMenu.setKeyMap (cmdActionIn, 'e');
  myMenu.setKeyMap (cmdActionEsc, 'q');

  // Prepare the main loop.
  bool bOver = false;
  drawMenu(&myMenu);

  while (!bOver)
  {
    // Keep loop alive while no key is pressed.
    while( !_kbhit() )
    {
      // Don't increase this value too much, as the sound will become distorted and effects will be destroyed.
      ::Sleep(20);

      // Inform the sound manager of the fact that it needs to update its time. You don't need to pass anything to it.
      SoundManager::getPtr ()->elapseTime ();
    }

    // Read the pressed key.
    char c;
    c = _getch();
    if( c=='x' || c== 27) //ESC
    {
      bOver = true;
    }
    else if( c=='`' )
    {
      displayInstructions ();
    }
    else
    {
      // Push the reaction to the key to the menu object.
      myMenu.reactToKeyPress(c);

      // If the key reaction resulted in the user wanting to quit, we'll find that information in the listener.
      bOver = lst->bOver;

      // redraw/refresh
      drawMenu(&myMenu);
    }
  }

}