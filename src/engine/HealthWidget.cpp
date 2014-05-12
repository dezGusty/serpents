#include "HealthWidget.h"
#include <Ogre.h>
#include <OgreTextAreaOverlayElement.h>
#include <OgreOverlayManager.h>
#include "GBaseSetting.h"

#include <guslib/trace/trace.h>

namespace Serpents
{
	HealthWidget::HealthWidget( const std::string & textElem, const std::string & graphElem )
		: textOverlayElement_(textElem)
		, graphicalOverlayElement_(graphElem)
		, textValue_(100)
	{
	}

	HealthWidget::~HealthWidget()
	{
	}


	void HealthWidget::render()
	{
		if( !messageAccessMutex.try_lock() )
		{
			return;
		}
		if( textOverlayElement_ != "" )
		{
			Ogre::OverlayElement * tempOverlayItem = Ogre::OverlayManager::getSingleton().getOverlayElement(textOverlayElement_);
			std::stringstream ss;
			ss<<"Health: "<< textValue_ <<" %";
			tempOverlayItem->setCaption(ss.str());
		}

		if(graphicalOverlayElement_ != "" )
		{
			Ogre::OverlayElement * tempOverlayItem = Ogre::OverlayManager::getSingletonPtr()->getOverlayElement(graphicalOverlayElement_);
			Ogre::ParameterList myList = tempOverlayItem->getMaterial()->getParameters();
			Ogre::Technique::PassIterator myIt = tempOverlayItem->getTechnique()->getPassIterator();

			while( myIt.hasMoreElements() )
			{
				Ogre::Pass::TextureUnitStateIterator secIt = myIt.getNext()->getTextureUnitStateIterator();
				while( secIt.hasMoreElements() )
				{
					double divisor = GBase::GBaseAppSettings::getPtr ()->model.healthIconDivision;
					if( divisor == 0 ) divisor=1;
					Ogre::Real v = GBase::GBaseAppSettings::getPtr ()->model.healthIconOffset 
						+ textValue_*100 / divisor;

					if( secIt.peekNext()->getName()=="tu_Tex" )
					{
						secIt.peekNext()->setTextureScroll(0, v);
					}

					secIt.moveNext();
				}
			}//endwhile
		}//endif
		messageAccessMutex.unlock();
	}
	void HealthWidget::setValue(double textValue)
	{
		guslib::GScopedLock myLock(messageAccessMutex);
		textValue_ = textValue;
		GTRACE(4, "HealthWidget::set"<<textValue_);
	}
}