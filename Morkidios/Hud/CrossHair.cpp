#include "CrossHair.h"

namespace Morkidios {

	// Construction methodes
	CrossHair::CrossHair(){
		mCrossHair = NULL;
	}
	CrossHair::~CrossHair(){
		CEGUI::System::getSingleton().getRenderer()->destroyTexture("CrossHairTexture");
		CEGUI::ImageManager::getSingleton().destroy("CrossHairImage");
		CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->destroyChild("CrossHair");
	}
	void CrossHair::init(std::string fileName){
		CEGUI::Texture& tex = CEGUI::System::getSingleton().getRenderer()->createTexture("CrossHairTexture", fileName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		Utils::addImageToImageset(tex, "CrossHairImage");

		mCrossHair = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->createChild("Generic/Image","CrossHair");
		mCrossHair->setProperty("Image","CrossHairImage");
		mCrossHair->setHorizontalAlignment(CEGUI::HA_CENTRE);
		mCrossHair->setVerticalAlignment(CEGUI::VA_CENTRE);
		mCrossHair->setWidth(CEGUI::UDim(0.1,0));
		mCrossHair->setAspectMode(CEGUI::AM_SHRINK);
	}

	// Various methodes
	void CrossHair::show(bool b){
		mCrossHair->setVisible(b);
	}
}