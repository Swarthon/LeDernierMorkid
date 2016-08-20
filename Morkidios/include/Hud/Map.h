#ifndef MAP_H
#define MAP_H

// CEGUI includes
#include <CEGUI/CEGUI.h>

// C++ includes
#include <iostream>

// My includes
#include "Utils.h"

namespace Morkidios {

	class Map {
	public:
		// Construction methodes
		Map();
		~Map();
		void addPoint(Ogre::Vector2 pos, std::string name, std::string imageFileName);
		void init(std::string fileName, Ogre::Vector2 terrainSize);

		// Various methodes
		void show(bool b);
		void updatePoint(Ogre::Vector2 newPos, std::string name);
	private:
		// CEGUI
		CEGUI::Window* mMap;
		CEGUI::Window* mImage;
		std::map<std::string, CEGUI::Window*> mPoints;

		Ogre::Vector2 mTerrainSize;

		bool mFullScreen;

		// Private methodes
		bool showFullScreen(const CEGUI::EventArgs& e);
	};
}

#endif // MAP_H
