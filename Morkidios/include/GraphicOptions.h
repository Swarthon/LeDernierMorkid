#ifndef GRAPHICOPTIONS_H
#define GRAPHICOPTIONS_H

#include <MorkidiosPrerequisites.h>

// My includes
#include "Character/Hero.h"

namespace Morkidios {
	class GraphicOptions {
	public:
		static GraphicOptions* getSingleton();
		static void destroy();

		// Construction methodes
		void setFileName(std::string fileName);
		void setRenderWindow(Ogre::RenderWindow* wnd);

		// Various methodes
		void save();
		bool load();
		void config();
	private:
		// Construction methodes
		GraphicOptions();
		~GraphicOptions();
		static GraphicOptions* mSingleton;

		std::string mFileName;
		Ogre::RenderWindow* mRenderWindow;

	public:
		// Options
		bool mFullScreen;
		double mFOV;
	};
}

#endif // GRAPHICOPTIONS_H
