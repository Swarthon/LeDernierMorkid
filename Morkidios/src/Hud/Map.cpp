#include "Map.h"

namespace Morkidios {

	// Construction methodes
	Map::Map(){
		// CEGUI
		mMap = NULL;
		mImage = NULL;

		mFullScreen = false;
	}
	Map::~Map(){
		CEGUI::System::getSingleton().getRenderer()->destroyTexture("MapTexture");
		CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->destroyChild("Map");
		CEGUI::ImageManager::getSingleton().destroy("MapImage");

		for(std::map<std::string, CEGUI::Window*>::iterator it = mPoints.begin(); it != mPoints.end(); it++){
			CEGUI::System::getSingleton().getRenderer()->destroyTexture(it->first);
			CEGUI::ImageManager::getSingleton().destroy(it->first);
		}
	}
	void Map::addPoint(Ogre::Vector2 pos, std::string name, std::string imageFileName){
		if(!mMap){
			std::cerr << "Error : Map not initialized for Map::addPoint\n";
			exit(1);
		}

		CEGUI::Texture& tex = CEGUI::System::getSingleton().getRenderer()->createTexture(name, imageFileName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		Utils::addImageToImageset(tex, name);

		mPoints[name] = mMap->createChild("Generic/Image", name);
		mPoints[name]->setSize(CEGUI::USize(CEGUI::UDim(0.05,0),CEGUI::UDim(0.05,0)));
		mPoints[name]->setProperty("Image", name);
		mPoints[name]->setInheritsAlpha(true);

		CEGUI::UVector2 size = CEGUI::UVector2(mPoints[name]->getSize().d_width, mPoints[name]->getSize().d_height) / CEGUI::UVector2(CEGUI::UDim(2,0), CEGUI::UDim(2,0));
		CEGUI::UVector2 p = CEGUI::UVector2(CEGUI::UDim((pos.x + mTerrainSize.x/2)/mTerrainSize.x,0), CEGUI::UDim((pos.y + mTerrainSize.y/2)/mTerrainSize.y,0)) - size;
		mPoints[name]->setPosition(p);
		mPoints[name]->setMousePassThroughEnabled(true);
	}
	void Map::init(std::string fileName, Ogre::Vector2 terrainSize){
		CEGUI::Texture& tex = CEGUI::System::getSingleton().getRenderer()->createTexture("MapTexture", fileName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		Utils::addImageToImageset(tex, "MapImage");

		mMap = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->createChild("TaharezLook/FrameWindow", "Map");
		mMap->setSize(CEGUI::USize(CEGUI::UDim(0.3,0),CEGUI::UDim(0.3,0)));
		mMap->setHorizontalAlignment(CEGUI::HA_RIGHT);
		mMap->setAspectMode(CEGUI::AM_SHRINK);
		mMap->setAlpha(0.25);

		mImage = mMap->createChild("Generic/Image", "MapImage");
		mImage->setProperty("Image","MapImage");
		mImage->setSize(CEGUI::USize(CEGUI::UDim(1,0),CEGUI::UDim(1,0)));
		mImage->setMousePassThroughEnabled(true);

		mMap->subscribeEvent(CEGUI::Window::EventMouseDoubleClick, CEGUI::Event::Subscriber(&Map::showFullScreen, this));

		mTerrainSize = terrainSize;
	}

	// Various methodes
	void Map::show(bool b){
		if(mMap)
			mMap->setVisible(b);
	}
	void Map::updatePoint(Ogre::Vector2 newPos, std::string name){
		CEGUI::UVector2 size = CEGUI::UVector2(mPoints[name]->getSize().d_width, mPoints[name]->getSize().d_height) / CEGUI::UVector2(CEGUI::UDim(2,0), CEGUI::UDim(2,0));
		CEGUI::UVector2 p = CEGUI::UVector2(CEGUI::UDim((newPos.x + mTerrainSize.x/2)/mTerrainSize.x,0), CEGUI::UDim((newPos.y + mTerrainSize.y/2)/mTerrainSize.y,0)) - size;
		mPoints[name]->setPosition(p);
	}

	// Private methodes
	bool Map::showFullScreen(const CEGUI::EventArgs& e){
		if(!mFullScreen){
			mMap->setSize(CEGUI::USize(CEGUI::UDim(1,0), CEGUI::UDim(1,0)));
			mMap->setAspectMode(CEGUI::AM_IGNORE);
		}
		else {
			mMap->setSize(CEGUI::USize(CEGUI::UDim(0.3,0),CEGUI::UDim(0.3,0)));
			mMap->setAspectMode(CEGUI::AM_SHRINK);
		}

		mFullScreen = !mFullScreen;

		return true;
	}

}
