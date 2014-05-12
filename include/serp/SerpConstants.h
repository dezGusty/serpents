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
//    Constants to be used within the game.

//definitions for overlay emenents (OE)
#define OE_MENU_LOGO "pLogoContainer"
#define OE_MENU_MUSIC_NOTICE "taMusicNotice"

// These must correspond to the menu  elements names, (tipically loaded form an XML file).
#define ME_MENU_RENDERING_ENGINE "renderengine"
#define ME_MENU_RESOLUTION "resolution"
#define ME_MENU_FULLSCREEN "fullscr"
#define ME_MENU_VIDEOMODE "vidmode"
#define ME_MENU_SOUNDMASTER "soundmaster"
#define ME_MENU_SOUNDMUSIC "soundmusic"
#define ME_MENU_SOUNDVOICE "soundvoice"
#define ME_MENU_SOUNDFX "soundfx"
#define ME_MENU_MESHQUAL "meshqual"
#define ME_MENU_SHADOWS "shadows"
#define ME_MENU_POSTPROCESSING "postpr"
#define ME_MENU_CPUSLEEP "cpusleep"

#define VIDEO_MODE_FULLSCREEN_ON "on"
#define VIDEO_MODE_FULLSCREEN_OFF "off"

#define GUS_SOUND_MENU_SELECTION_CHANGED "data/sounds/MenuSelectionChange.wav"
#define GUS_SOUND_MENU_SELECTION_CLICKED "data/sounds/MenuSelectionClicked.wav"
#define GUS_SOUND_MENU_OPTION_CHANGED "data/sounds/MenuOptionChange.wav"

#define GUS_SOUND_BONUS_PICKED_UP "data/sounds/Mulchsound.wav"
#define GUS_SOUND_BONUS_POPPED_UP "data/sounds/Huiitysound.wav"
#define GUS_SOUND_BONUS_CLICK "data/sounds/Clickysound.wav"

#define GUS_MENU_PLAYLIST "menu"
#define GUS_GAME_PLAYLIST "game"

#define GUS_CORRECTION_VECTOR_PARAMS 0.0, 10.0, 0.0
// True constants.

static const int c_distanceForRaySceneQueries = 1000;

static const std::string c_mainMenuOverlay = "oMainMenu";
static const std::string c_gameOverlay = "oGame";
static const std::string c_inGameMenuOverlay = "oInGameMenu";

//static const std::string c_songArtistItem = "taSongArtist";
//static const std::string c_songTitleItem = "taSongTitle";
//static const std::string c_songInfoPanel = "pSongInfo";


static const std::string c_webSiteAddressItem = "taWebSiteAddress";
static const std::string c_mainMenuPanel = "pMainMenu";//GS/MainMenuPanel
static const std::string c_optionsPanel = "GS/OptionsPanel";
static const std::string c_mapSelectionPanel = "GS/MapSelectionPanel";
static const std::string c_soundOptionsPanel = "GS/OptionsSoundPanel";
static const std::string c_graphicsOptionsPanel = "GS/OptionsGrafxPanel";
static const std::string c_videoModeOptionsPanel = "GS/OptionsVidModePanel";

