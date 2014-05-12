#pragma once

//    This file is part of Gusty's Serpents
//    Copyright (C) 2009  Augustin Preda (thegusty999@gmail.com)
//
//    Gusty's Serpents is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    The game algorithm's main class.

// Includes

#include "Bonus.h"
#include "Map.h"
#include "SerpentBase.h"
#include <guslib/system/timer.h>
#include "ScoringTable.h"
#include "GBaseEngine.h"

namespace Serpents
{
	struct GameSessionSettings
	{
		int numPlayers;
	};


	class GGameSession
	{
	private:
		GameSessionSettings settings;
	public:
		GGameSession();
		GameSessionSettings getSettings();
		void setSettings( GameSessionSettings someSettings );
	};


	class GGameAlgoStateListener
	{
	public:
		virtual void OnGameFinished() = 0;
		virtual void OnBonusSpawned( BonusBase * bns ) = 0;
		virtual void OnPlayerScoreChange( long score ) = 0;
		virtual void OnPlayerHealthChange( double percentage ) = 0;
		virtual void OnPlayerHungerChange( double percentage ) = 0;
		virtual void OnPlayerLengthChange( double newLength ) = 0;
		virtual void OnBonusPicked( BonusBase * bns ) = 0;
		virtual void OnPlayerSpeedChange( double speed ) = 0;
		//virtual void OnPlayerPositionChange( double x, double y ) = 0;
	};


	class GGameAlgo
	{
	private:
		guslib::Timer timer;
		GGameSession session;
		BonusManagerBase * bonusManager;
		SerpentBase * serpent;
		GGameAlgoStateListener * listener;
		Map * gameMap;
		GScoringTable * scoringTable;

		GBase::GBaseEngine * engine;
		bool started;
		bool paused;

	protected:

		double standardHungerGrowthRate;
		double standardHungerQuench; // when a bonus is picked, hunger drops by this amount.
		double standardHealthDropRate; // when hunger reaches the maximum level, health begins to by this amount of points
		double standardHealthDropInterval; // when hunger reaches the maximum level, health drops once every interval .
		double standardHealthDropTimer; // internal counter.
		double standardHungerDamagingLimit; // the level at which the hunger must be for damage to begin.


		// Do some action; the time in millis is the one elapsed from the last similar operation.
		virtual void actionLoop( guslib::TimeUnits elapsedTimeInMillis );

		virtual bool isPaused(){ return paused; }

		virtual void reactToInput( guslib::TimeUnits elapsedTimeInMillis );

	public:
		GGameAlgo();
		virtual ~GGameAlgo();

		void pause();
		void resume();

		bool start();

		virtual void setBonusManager( BonusManagerBase * mgr );
		virtual void setSerpent( SerpentBase * srp );
		virtual void setMap( Map * aMap );
		virtual void setEngine( GBase::GBaseEngine * anEngine );
		virtual void setScoringTable( GScoringTable * aTable );


		virtual void setStateListener( GGameAlgoStateListener * lis );

		// non-virtual; begin a logical game;
		void startGameSession(GGameSession aSession);

		//
		void lapseTime();

		// stop a logical game;
		virtual void stopGameSession();

		virtual void verifyBonusCapture();

		virtual void verifyCollisions();
	};
}