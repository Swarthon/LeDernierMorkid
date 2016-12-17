#ifndef GRAPHICOPTIONS_H
#define GRAPHICOPTIONS_H

#include <MorkidiosPrerequisites.h>

// My includes
#include "Hud/CrossHair.h"
#include "Character/Hero.h"

namespace Morkidios {
	class _MorkidiosExport GraphicOptions {
	public:
		static GraphicOptions* getSingleton();
		static void destroy();

		// Construction methodes
		void setFileName(std::string fileName);
		void setRenderWindow(Ogre::RenderWindow* wnd);
		void setCrossHair(CrossHair* ch);

		// Various methodes
		void save();
		bool load();
		void config();

		// Options
		bool mFullScreen;
		double mFOV;
		CEGUI::UDim mCrossHairSize;
	private:
		// Construction methodes
		GraphicOptions();
		~GraphicOptions();
		static GraphicOptions* mSingleton;

		std::string mFileName;
		Ogre::RenderWindow* mRenderWindow;
		CrossHair* mCrossHair;
	};
}

#endif // GRAPHICOPTIONS_H
