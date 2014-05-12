#include <stdio.h>

#include "../engine/GusAudioEngine.h"
#include "../utils/GTrace.h"
#include <iostream>
#include <conio.h>
#include <windows.h>

using namespace std;
using namespace GUtils;
using namespace Serpents;

#define MUSIC_01 "d:/gus/workgus/Cpp/GustysSerpents/Serpents/Game/data/music/menu.mp3"
#define MUSIC_02 "D:/Downloads/Music/Heir to Madness/02-Bliss-Heir to Madness.mp3"
#define MUSIC_03 "D:/Downloads/Music/Cargo Cult/Alchemy/01-Entry-Cargo Cult.mp3"
#define SND_01 "d:/gus/workgus/Cpp/GustysSerpents/Serpents/Game/data/sounds/MenuSelectionClicked.wav"
void displayInstructions()
{
	cout<<"Enter single chars to interact with the application"<<endl
		<<"List of char actions:"<<endl
		<<"` : see this message"<<endl
		<<"1 : start menu playlist"<<endl
		<<"2 : pause menu playlist"<<endl
		<<"3 : stop menu playlist"<<endl
		<<"4 : next song in menu playlist"<<endl
		<<"9 : play sound ["<<SND_01<<"]"<<endl
		<<"0 : play sound ["<<SND_01<<"]"<<endl
		<<"o : load user sounds"<<endl
		<<"p : remove user sounds"<<endl
		<<"r : re-create the engine"<<endl
		<<"q : decrease volume"<<endl
		<<"w : increase volume"<<endl
		<<"x : exit the application"<<endl;
}

void loadUserSounds(GusAudioEngine *engine)
{
	GusAudioEngine::getInstancePtr()->addSound("snd1", SND_01, GST_UserDefined, GSC_Effect, false);
}

void recreateEngine()
{

}

void main()
{
	GSTARTTRACING("z_audiotrace.txt", 6);
	cout<<"Starting audio test"<<endl;
	displayInstructions();

	float fGeneralVolume = 0.4f;
	GusAudioEngine::getInstancePtr()->setVolumeForCategory(GSC_Master,fGeneralVolume);

	GusAudioEngine *engine = GusAudioEngine::getInstancePtr();
	GusAudioEngine::getInstancePtr()->addPlaylist("menumusic");
	//GusAudioEngine::getInstancePtr()->addPlaylist("gamemusic");


	GAdvSound * mytempsnd = new GAdvSound(*engine,"music1", MUSIC_01, GST_Internal, GSC_Music, false);
	GusAudioEngine::getInstancePtr()->addSound( mytempsnd );
	//GSoundEffect * effect = new GFadeInEffect(mytempsnd, mytempsnd->getLengthInMillis(), 3000);
	//mytempsnd->addPermanentEffect(effect);
	//effect = new GFadeOutEffect(mytempsnd, mytempsnd->getLengthInMillis(), 3000);
	//mytempsnd->addPermanentEffect(effect);
	GusAudioEngine::getInstancePtr()->getPlaylist("menumusic")->addSound(mytempsnd);
	//GusAudioEngine::getInstancePtr()->addSound( mytempsnd );

	mytempsnd = new GAdvSound(*engine,"music2", MUSIC_02, GST_Internal, GSC_Music, false);
	GusAudioEngine::getInstancePtr()->addSound( mytempsnd );
	//effect = new GFadeInEffect(mytempsnd, mytempsnd->getLengthInMillis(), 5000);
	//mytempsnd->addTemporaryEffect(effect);
	//effect = new GFadeOutEffect(mytempsnd, mytempsnd->getLengthInMillis(), 3000);
	//mytempsnd->addPermanentEffect(effect);
	GusAudioEngine::getInstancePtr()->getPlaylist("menumusic")->addSound(mytempsnd);
	//GusAudioEngine::getInstancePtr()->addSound( mytempsnd );

	mytempsnd = new GAdvSound(*engine,"music2", MUSIC_03, GST_Internal, GSC_Music, false);
	GusAudioEngine::getInstancePtr()->addSound( mytempsnd );
	GusAudioEngine::getInstancePtr()->getPlaylist("menumusic")->addSound(mytempsnd);


	//GusAudioEngine::getInstancePtr()->addSound("music1", MUSIC_01, GST_Internal, GSC_Music, false);
	//GusAudioEngine::getInstancePtr()->addSound("music2", MUSIC_02, GST_Internal, GSC_Music, false);
	
	loadUserSounds( engine );
	GusAudioEngine::getInstancePtr()->addSound("snd2", SND_01, GST_Internal, GSC_Effect, false);

	cout<<"music1 len="<<GusAudioEngine::getInstancePtr()->getSoundByName("music1")->getLengthInMillis()<<endl;

	GSoundPlaylist *mylist = GusAudioEngine::getInstancePtr()->getPlaylist("inexistent");
	if( mylist != NULL )
	{
		cout<<"getPlaylist did not return NULL as it was expected!"<<endl;
	}
	mylist = GusAudioEngine::getInstancePtr()->getPlaylist("menumusic");
	if( mylist == NULL )
	{
		cout<<"getPlaylist did return NULL unexpected!"<<endl;
	}
	//GusAudioEngine::getInstancePtr()->addSoundToPlayList( "music1", "menumusic" );
	//GusAudioEngine::getInstancePtr()->addSoundToPlayList( "music2", "menumusic" );
	GusAudioEngine::getInstancePtr()->getPlaylist("menumusic")->setRepeat(true);
	//GusAudioEngine::getInstancePtr()->getPlaylist("menumusic")->play();
	//GSOUNDPLAYLIST("menumusic");
	GSOUNDPLAYLISTWITHFADEIN("menumusic");


	//GusAudioEngine::getInstancePtr()->setMusicPlay();
	char c=0;
	bool bExit = false;
	while( !bExit )
	{
		while( !_kbhit() )
		{
			Sleep(5);
			GusAudioEngine::getInstancePtr()->elapseTime();
		}
		c = _getch();

		if( c=='x' )
			bExit = true;

		//scanf("%c", &c);
		if( c=='`' )
		{
			displayInstructions();
		}
		if( c=='1' )
		{
			//GSOUNDPLAYLIST("menumusic");
			GSOUNDPLAYLISTWITHFADEIN("menumusic");
			//GusAudioEngine::getInstancePtr()->getPlaylist("menumusic")->play();
		}
		if( c=='2' )
		{
			GSOUNDPAUSELIST("menumusic");
			//GusAudioEngine::getInstancePtr()->getPlaylist("menumusic")->pause();
		}
		if( c=='3' )
		{
			//GSOUNDSTOPLIST("menumusic");
			GSTOPLISTFADEOUT("menumusic");
			//GusAudioEngine::getInstancePtr()->getPlaylist("menumusic")->stop();
		}
		if( c=='4' )
		{
			GusAudioEngine::getInstancePtr()->getPlaylist("menumusic")->skipToNextTrack();
		}
		if( c=='0' )
		{
			GusAudioEngine::getInstancePtr()->play( "snd1" );
		}
		if( c=='9' )
		{
			GusAudioEngine::getInstancePtr()->play( "snd2" );
		}
		if( c=='p' )
		{
			GusAudioEngine::getInstancePtr()->removeUserSounds();
		}
		if( c=='o' )
		{
			loadUserSounds( engine );
		}
		if( c=='r' )
		{
			GusAudioEngine::destroyInstance();
		}
		if( c=='q' )
		{
			GusAudioEngine::getInstancePtr()->setVolumeForCategory( GSC_Master, 
				GusAudioEngine::getInstancePtr()->getVolumeForCategory(GSC_Master)-0.1f );
		}
		if( c=='w' )
		{
			GusAudioEngine::getInstancePtr()->setVolumeForCategory( GSC_Master, 
				GusAudioEngine::getInstancePtr()->getVolumeForCategory(GSC_Master)+0.1f );
		}
	}

	GusAudioEngine::destroyInstance();
	cout<<"End of audio test."<<endl;
}