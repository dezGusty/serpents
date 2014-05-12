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
//    Class to allow easier access to the hiscores.

#include "utils/GGeneralUtils.h"
#include <guslib/common/smartPtr.h>

///
///	The classes defined here are meant to easen the job of keeping track of the game's scores.
///

// so what is a scoring table exactly?
// Is it just the combination of name and points you would get in an arcade game?
// In my opinion it should also provide additional information such as the amount of bonuses picked for a snake
// game, the number of slain enemies for an RPG, production and kill count for an RTS, etc...
// But all that is just additional information. In order to have a normal classification, you
// should be able to have a single numerical score, so that a player would easily know if he did
// or is doing better that someone else. This will also allow an easier on-the-fly update of the score.
// For instance, you could check at each change in score if the player just set a new record.
//
// how easy should it be to set up?
// The provided interface should allow a load of the current scores, so that they may be displayed.
// The save operation should also be quite straightforward.
// To manage the progression of the player's score, a special entry is created for him and 
namespace Serpents
{

	class GScoreData
	{
	};


	// utlity typedef.
	typedef GSharedPtr<GScoreData> GScoreDataImpl;


	/// A single entry, containing all relevant information for that particular entry
	class GScoreEntry
	{
	private:
		double internalValue; // this shouldn't be accessed directly by child classes.
		std::string name;
		long ageOfEntry;
		GScoreDataImpl additionalData;

	protected:

	public:
		GScoreEntry();
		virtual ~GScoreEntry();

		///	Getter for the additional data; this will not be handled by default in any way.
		virtual GScoreDataImpl getAdditionalData(){ return additionalData; }

		///	Getter for the name of the entry.
		long getAge() const { return ageOfEntry; }

		///	Getter for the name of the entry.
		virtual const std::string& getName() const { return name; }

		///	Getter for the value of the entry. If you need additional information attached consider
		/// using the additionalData entry.
		virtual double getValue() const { return internalValue; }

		/**		
			See if the currentEntry fares better than another one. 
			@param rhs The score entry to compare to.
			@param actionInCaseEqual = 0, 1, 2
			(0=don't care; 1=older entries are considered greater; 2=new entries are considered greater
		*/
		virtual bool isGreaterThan( const GScoreEntry & rhs, int resultInCaseEqual =1 ) const;

		/// Load the data from a string;
		virtual void loadFromString( const std::string & inputString );

		///	Setter for the additional data; this will not be handled by default in any way.
		void setAdditionalData(GScoreDataImpl data){ additionalData = data; }

		/// Set the age of the entry.
		void setAge( long newAge ){ ageOfEntry = newAge; }

		///	Set the name associated with the entry.
		virtual void setName( const std::string & newName ){ name = newName; }

		///	Set the value of the entry.
		virtual void setValue( double newScore ){ internalValue = newScore; }

		// This is actually what will be written to the file.
		virtual std::string toString() const;

	};

	// utlity typedef.
	typedef GSharedPtr<GScoreEntry> GScoreEntryImpl;


	///
	///	Callback interface for the changing of the position in the list.
	///	classes implementing this class will be informed when the current position in the table has changed its position.
	///
	class ScoringTableEventListener
	{
	public:
		///	Callback function called when the score of the current user climbs up or falls down the ladder.
		virtual void OnScoringTablePositionChanged( long newPosition, long oldPosition ) = 0;
	};


	/// The class that will keep the entire list of entries in the score list.
	class GScoringTable
	{
	private:
		std::string loadedFile;			//	the name of the file the scores were loaded from.

	protected:
		std::vector<GScoreEntryImpl> entries;
		GScoreEntryImpl currentEntry;
		ScoringTableEventListener * tableEventListener;
		std::string lastUsedName;

		int expectedNumberOfEntries;
		bool scoresChanged;			//	specify wether the scores have changed or not, so that they won't be saved if not necessary.
		int latestPositionOfCurrent;

		/// Generate a new score entry. This will not be added automatically to the list of score entries.
		virtual GScoreEntry * createScoreEntry();

	public:
		GScoringTable();
		virtual ~GScoringTable();


		///
		///	Create a new entry for the game in progress. The entry can be retrieved afterwards via the 
		///	"getCurrentEntry()" function.
		///
		virtual void createScoreEntryForCurrentGame();

		virtual void destroyScoreEntryForCurrentGame();

		///	Get the score entry being used for the current player.
		virtual GScoreEntryImpl & getCurrentEntry() { return currentEntry; }

		// TODO: This seems to be exposing too much information. Should reconsider.
		std::vector<GScoreEntryImpl> & getEntries() { return entries; } 

		/// This will not affect the original table, but will create a copy and integrate the current user's score
		/// in the returned list.
		std::vector<GScoreEntryImpl> getTemporaryTable();

		virtual int getLatestPositionOfCurrent() const { return latestPositionOfCurrent; }

		///	Get the name that was last used. This will be useful generally when someone keeps playing and after
		///	he makes a score reaching the scoring table, he inputs his name, which is remembered by the scoring list.
		///	The next time he plays a game and enters the hiscores, his name can be automatically filled.
		virtual std::string getLatestUsedName() const { return lastUsedName; }

		/// Get the file that was loaded. It will be an empty string if no file was loaded.
		const std::string& getLoadedFile() const { return loadedFile; }

		/// Get the number of entries currently held.
		long getNumberOfEntries() const;

		///	Load the score list from a file.
		virtual void loadFromFile( const std::string &fileToLoad );

		///	Reset the list to some default hard-coded values.
		virtual void resetToDefaults();

		///	Save the hiscores to a file with a given name.
		virtual void save( const std::string &fileToSave = std::string("") );

		///	Find out if the scores have changed and saving them actually makes sense.
		virtual bool scoresWereChanged() const;

		void setListener( const ScoringTableEventListener * aListener );

		///	Do a full sorting of the list. This will generally NOT be used. 
		virtual void sortList( bool inCaseEqualOlderScoreMoreImportant = true, bool ascending = true );

		///	Update the score of the current entry, giving it a new value. This will also check the score table to see
		///	if the change affects the position of the current entry in the list. If it does, the registered listeners 
		///	will be notified of the change. 
		virtual void updateCurrentScore( double newValue );
	};

}