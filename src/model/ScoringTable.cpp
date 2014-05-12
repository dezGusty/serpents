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


#include <model/ScoringTable.h>
#include <guslib/trace/trace.h>


namespace Serpents
{
	// these are the expected tags for the hiscore file.

	const std::string gsAgeStrBegin = "<age>";
	const std::string gsAgeStrEnd = "</age>";
	const std::string gsNamStrBegin = "<name>";
	const std::string gsNamStrEnd = "</name>";
	const std::string gsValStrBegin = "<score>";
	const std::string gsValStrEnd = "</score>";

	const std::string gsLatestNamBegin = "<latestname>";
	const std::string gsLatestNamEnd = "</latestname>";

	const int defaultNumericalScoreLength = 20;
	const int defaultNumberOfScores = 20;
	const int maximumNumberOfScores = 100;


	//
	//	GScoreEntry -------------------------------------------------------------------------------------------
	//

	GScoreEntry::GScoreEntry()
		: internalValue(0)
		, name("")
		, ageOfEntry(0)
	{
	}

	GScoreEntry::~GScoreEntry()
	{
	}

	bool GScoreEntry::isGreaterThan( const GScoreEntry & rhs, int actionInCaseEqual ) const
	{
		if( internalValue > rhs.getValue() )
		{
			return true;
		}
		else if( internalValue == rhs.getValue() )
		{
			switch( actionInCaseEqual )
			{
			case 1://older (greater) age counts more
				return this->ageOfEntry > rhs.getAge();
			case 2://newer age counts more
				return this->ageOfEntry < rhs.getAge();
			case 0://don't care
			default:
				return false;
			}
		}
		return false;
	}



	void GScoreEntry::loadFromString( const std::string & inputString )
	{
		using namespace std;

		ageOfEntry=0;
		int myPos;
		std::string sub1;
		myPos = (int) inputString.find(gsAgeStrBegin);
		if( myPos == std::string::npos )
			return;
		sub1 = inputString.substr( myPos + gsAgeStrBegin.length() );
		myPos = (int) sub1.find(gsAgeStrEnd);
		if( myPos == std::string::npos )
			return;
		ageOfEntry = atoi(sub1.c_str());

		myPos = (int) inputString.find(gsNamStrBegin);
		if( myPos == std::string::npos )
			return;
		sub1 = inputString.substr( myPos + gsNamStrBegin.length() );
		myPos = (int) sub1.find(gsNamStrEnd);
		if( myPos == std::string::npos )
			return;
		sub1 = sub1.substr(0, myPos);
		name = sub1;

		myPos = (int) inputString.find(gsValStrBegin);
		if( myPos == std::string::npos )
			return;
		sub1 = inputString.substr( myPos + gsValStrBegin.length() );
		myPos = (int) sub1.find(gsValStrEnd);
		if( myPos == std::string::npos )
			return;
		sub1 = sub1.substr(0, myPos);

		internalValue = atof( sub1.c_str() );
	}


	std::string GScoreEntry::toString() const
	{
		std::stringstream gss;
		gss << gsAgeStrBegin << ageOfEntry << gsAgeStrEnd
			<< gsNamStrBegin << name << gsNamStrEnd
			<< gsValStrBegin << getValue() << gsValStrEnd;
		return gss.str();
	}



	//
	//	GScoringTable.-------------------------------------------------------------------------------------------
	//


	GScoringTable::GScoringTable()
		: tableEventListener( NULL )
		, loadedFile( "" )
		, expectedNumberOfEntries( defaultNumberOfScores )
		, latestPositionOfCurrent( -1 )
		, scoresChanged( false )
	{
	}

	GScoringTable::~GScoringTable()
	{
	}


	GScoreEntry * GScoringTable::createScoreEntry()
	{
		GScoreEntry *ret = new GScoreEntry();
		return ret;
	}

	void GScoringTable::createScoreEntryForCurrentGame()
	{
		GScoreEntry *temp = createScoreEntry();

		currentEntry = GScoreEntryImpl (temp);
		latestPositionOfCurrent = -1;
		scoresChanged = false;
	}

	void GScoringTable::destroyScoreEntryForCurrentGame()
	{
		currentEntry.reset();
		latestPositionOfCurrent = -1;
		scoresChanged = false;
	}



	void GScoringTable::setListener( const ScoringTableEventListener * aListener )
	{
		tableEventListener = (ScoringTableEventListener*) aListener;
	}



	std::vector<GScoreEntryImpl> GScoringTable::getTemporaryTable()
	{
		std::vector<GScoreEntryImpl> myList;

		if( ! scoresWereChanged() )
			return getEntries();

		if( latestPositionOfCurrent != -1 )
		{
			// the player entered the high-scores. Insert his score in the list.

			std::vector<GScoreEntryImpl>::iterator it = entries.begin();
			// The current score is in the list.
			int i=0;
			for( i=0; i<latestPositionOfCurrent && it!=entries.end(); i++ )
			{
				myList.push_back( *it );
				++it;
			}
			myList.push_back( currentEntry );
			i++;
			for( ; i<expectedNumberOfEntries && it!=entries.end(); i++ )
			{
				myList.push_back( *it );
				++it;
			}
		}
		return myList;
	}


	long GScoringTable::getNumberOfEntries() const
	{
		return (long)entries.size();
	}


	void GScoringTable::loadFromFile( const std::string & aFileToLoad )
	{
		GTRACE(4, "Loading scoring table from:"<<aFileToLoad);
		loadedFile = aFileToLoad;

		int expectedNumberOfEntries = 99999;
		entries.clear();
		FILE * f = NULL;
		errno_t err = fopen_s(&f, loadedFile.c_str(), "rb" );
		if( err )
		{
			expectedNumberOfEntries = defaultNumberOfScores;
			resetToDefaults();
			return;
		}
		int numLines = 0; // the number of lines scanned so far.

		std::stringstream buffer;
		// process the entire file.
		while( !feof(f) && expectedNumberOfEntries >= numLines)
		{
			char c = 0;
			buffer.str("");

			// scan a line. TODO: should be adapted for other OSs' line termination
			while( c!= '\n' && !feof(f)) // ignore \r
			{
				fscanf_s(f, "%c", &c);
				if( c!= '\n' && c!= '\r')
				{
					buffer << c;
				}
			}

			// if non-empty line
			if( buffer.str().length() > 0 )
			{
				if( numLines == 0 ) // if this is the first line
				{
					expectedNumberOfEntries = atoi( buffer.str().c_str() );
					// Make sure the number of entries is not too large or negative.
					if( expectedNumberOfEntries < 0 )
					{
						expectedNumberOfEntries = defaultNumberOfScores;
						return;
					}
					else if( expectedNumberOfEntries > maximumNumberOfScores )
					{
						expectedNumberOfEntries = maximumNumberOfScores;
					}
				}
				else
				{
					GScoreEntry * newEntry = createScoreEntry();
					if( buffer.str().length() > 0 )
						newEntry->loadFromString(buffer.str());
					GScoreEntryImpl tempEntry( newEntry );
					entries.push_back( tempEntry );
					GTRACE(4, "created score entry:" << tempEntry.get()->getName() <<","<<tempEntry.get()->getValue());
					GTRACE(4, "check score entry:" << entries.back().get()->getName() <<","<<entries.back().get()->getValue());
				}
			}
			// increment number of lines
			numLines ++;
		}
		fclose(f);
	}

	void GScoringTable::resetToDefaults()
	{
		// reset the current score list to the default values;

		if( expectedNumberOfEntries < 0 ) expectedNumberOfEntries = 10;
		if( expectedNumberOfEntries > 100 ) expectedNumberOfEntries = 100;

		for( int i=0; i< expectedNumberOfEntries; i++ )
		{
			GScoreEntry * defEntry = createScoreEntry();
			std::stringstream gss;
			gss << "Anonymous (" << expectedNumberOfEntries - i << ")";
			defEntry->setName( gss.str().c_str() );
			defEntry->setValue( (i+1) * 10 );
			GScoreEntryImpl tempEntry( defEntry );
			entries.push_back( tempEntry );
		}
		sortList(true, false);
		scoresChanged = true;
	}


	void GScoringTable::save( const std::string & fileToSave )
	{
		if( ! scoresWereChanged() )
			return;
		std::string fileName = fileToSave;
		if( fileName == std::string("") )
		{
			GTRACE(5, "GScoringTable: no save file provided; saving to loaded file [" << loadedFile << "]" );
			fileName = loadedFile;
		}
		if( fileName == std::string("") )
		{
			// Nothing loaded
			return;
		}

		GTRACE(3, "Saving scores to [" << fileName << "]" );

		if( latestPositionOfCurrent != -1 )
		{
			// the player entered the high-scores. Insert his score in the list.

			std::vector<GScoreEntryImpl>::iterator it = entries.begin();
			// The current score is in the list.
			for( int i=0; i<latestPositionOfCurrent && it!=entries.end(); i++ )
			{
				++it;
			}
			entries.insert( it, currentEntry );
			
		}
		// do the actual saving.

		FILE * f = NULL;
		fopen_s(&f, fileName.c_str(),"wb");

		//	print number of entries;
		fprintf(f, "%d\n", expectedNumberOfEntries );
		for( int i=0; i< expectedNumberOfEntries && i<getNumberOfEntries(); i++ )
		{
			fprintf_s(f, "%s\n", entries[i].get()->toString().c_str());
		}
		fclose(f);

		// remove the last entry in the list (lowest value).

		entries.pop_back();
		currentEntry.reset();
		scoresChanged = false;
	}


	bool GScoringTable::scoresWereChanged() const
	{
		return scoresChanged;
	}


	void GScoringTable::sortList(bool inCaseEqualOlderScoreMoreImportant,bool ascending)
	{
		//	nothing to sort.
		if( entries.size() <= 1 )
			return;

		for( int i=0; i<(int)entries.size(); i++ )
		{
			int myPos = i;
			for( int j=i+1; j<(int)entries.size(); j++ )
			{
				if( ascending )
				{
					if( entries[i].get()->isGreaterThan(*entries[j].get()) )
						myPos = j;
				}
				else
				{
					if( entries[j].get()->isGreaterThan(*entries[i].get()) )
						myPos = j;
				}
			}
			if( myPos != i )
			{
				GScoreEntryImpl aux = entries[i];
				entries[i] = entries[myPos];
				entries[myPos] = aux;
			}
		}
	}


	void GScoringTable::updateCurrentScore( double newValue )
	{
		if( getCurrentEntry().get() == 0 )
		{
			return;
		}

		
		GTRACE(4, "ScoringTable updating current score, newval="<< newValue );
		// If the value didn't change since last update, just exit.
		if( getCurrentEntry().get()->getValue() == newValue )
		{
			return;
		}

		if( getCurrentEntry().get()->getValue() < newValue )
		{
			//	current position will get smaller (climbing up the ladder means "2" is better than "4"
			//	or the second place is better than the fourth

			GTRACE(5, "*** update current score, oldval="<< getCurrentEntry().get()->getValue() );
			long initialPos = latestPositionOfCurrent;
			getCurrentEntry().get()->setValue( newValue );
			long startOfFor = ( latestPositionOfCurrent == -1 )? (getNumberOfEntries()-1) : latestPositionOfCurrent;

			for( int i= startOfFor; i>=0; i-- )
			{
				GTRACE(3, "*** checking  pos..." << i);
				if( getCurrentEntry().get()->isGreaterThan( * getEntries()[i].get(), 1))
				{
					latestPositionOfCurrent = i;
				}
				else 
				{
					GTRACE(3, "*** found  pos..." << latestPositionOfCurrent);
					break;
				}
			}
			if( initialPos != latestPositionOfCurrent )
			{
				scoresChanged = true;
				// notify the listeners.
				GTRACE(7, "ScoringTable notifying listener");
				if( tableEventListener )
				{
					tableEventListener->OnScoringTablePositionChanged(latestPositionOfCurrent, initialPos);
					GTRACE(6, "ScoringTable notified listener");
				}
			}
		}
	}

}