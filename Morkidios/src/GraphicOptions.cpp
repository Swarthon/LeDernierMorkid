#include "GraphicOptions.h"

Morkidios::GraphicOptions* _MorkidiosExport Morkidios::GraphicOptions::mSingleton = NULL;

namespace Morkidios {
	GraphicOptions* GraphicOptions::getSingleton(){
		if(!mSingleton)
			mSingleton = new GraphicOptions();
		return mSingleton;
	}
	void GraphicOptions::destroy(){
		delete mSingleton;
	}

	// Construction methodes
	void GraphicOptions::setFileName(std::string fileName){
		mFileName = fileName;
	}
	void GraphicOptions::setRenderWindow(Ogre::RenderWindow* wnd){
		mRenderWindow = wnd;
	}
	void GraphicOptions::setCrossHair(CrossHair* ch){
		mCrossHair = ch;
		mCrossHair->setSize(mCrossHairSize);
	}

	// Various methodes
	void GraphicOptions::save(){
		std::ofstream cfg(mFileName.c_str());
		cfg << "[Graphics]" << std::endl;
		cfg << "FOV=" << mFOV << std::endl;
		cfg << "FullScreen=" << mFullScreen << std::endl;
		cfg << "[GUI]" << std::endl;
		cfg << "CrossHair=" << mCrossHairSize.d_scale << std::endl;
	}
	bool GraphicOptions::load(){
		try {
			Ogre::ConfigFile cfg;
			cfg.load(mFileName.c_str());

			mFOV = Ogre::StringConverter::parseReal(cfg.getSetting("FOV", "Graphics"));
			mFullScreen = Ogre::StringConverter::parseReal(cfg.getSetting("FullScreen", "Graphics"));
			mCrossHairSize = CEGUI::UDim(Ogre::StringConverter::parseReal(cfg.getSetting("CrossHair", "GUI")),0);

			return true;
		}
		catch (Ogre::Exception& e){
			return false;
		}
	}
	void GraphicOptions::config(){
		if(Hero::getSingleton()->getCamera())
			Hero::getSingleton()->getCamera()->setFOVy(Ogre::Radian(mFOV));
		if(mRenderWindow)
			mRenderWindow->setFullscreen(mFullScreen, mRenderWindow->getWidth(), mRenderWindow->getHeight());
		if(mCrossHair)
			mCrossHair->setSize(mCrossHairSize);
	}

	// Construction methodes
	GraphicOptions::GraphicOptions(){
		mFileName = "graphics.cfg";
		mRenderWindow = NULL;
		mFullScreen = true;
		mFOV = Ogre::Degree(30).valueRadians();
		mCrossHair = NULL;
		mCrossHairSize = CEGUI::UDim(0.1,0);
	}
	GraphicOptions::~GraphicOptions(){
	}
}
