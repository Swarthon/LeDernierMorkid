#include "LeDernierMorkid.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
int main()
#endif
{
        LeDernierMorkid* lederniermorkid = new LeDernierMorkid();
	lederniermorkid->run();
	return 0;
}
