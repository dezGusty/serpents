#ifndef _APP_SERP_GUI_H
#define _APP_SERP_GUI_H

#ifdef CEGUI_07
#include "CEGUI.h"
#include "RendererModules/Ogre/CEGUIOgreRenderer.h"
#else
#include "cegui/CEGUI.h"
#include "CEGUI/RendererModules/Ogre/Renderer.h"
#endif


namespace app
{
	/**
		Utility class to encapsulate a 3rd party library for dealing with the GUI.
	*/
	class SerpGUI
	{
	protected:
		CEGUI::System* ceguiSysPtr_;
		std::string resourceGroupName_;

	public:
		SerpGUI (Ogre::RenderTarget* targetPtr, const std::string& logFileName, int logLevel = 2);

		virtual ~SerpGUI ();

		void addFont (const std::string& fontName);
		void setDefaultFont (const std::string& fontName);

		void setMouseCursor (const std::string& cursorName);

		virtual void initialize ();

		virtual void loadSchemes (std::vector <std::string> schemeList);

		virtual void renderGUI (float timeSinceLastRender = 1.0f);

		/**
			Commence the API Logging.
			@param fileName The name of the file to write to. It should be in the working directory.
		*/
		static void startLogging (const std::string& fileName);
	};
}

#endif // _APP_SERP_GUI_H