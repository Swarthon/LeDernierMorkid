#ifndef QUIT_MENU_H
#define QUIT_MENU_H

// OGRE includes
#include <OGRE/Ogre.h>

// CEGUI includes
#include <CEGUI/CEGUI.h>

// Morkidios includes
#include <Morkidios.h>

class QuitMenu {
public:
	QuitMenu(CEGUI::Window* parent = 0);
	~QuitMenu();

	// Various methodes
	void show(bool b);

	// Return value methodes
	CEGUI::Window* getWindow();
	CEGUI::Window* getCancel();
	CEGUI::Window* getMainMenu();
	CEGUI::Window* getDesktop();
private:
	CEGUI::Window* mWindow;
	CEGUI::Window* mCancel;
	CEGUI::Window* mMainMenu;
	CEGUI::Window* mDesktop;
	CEGUI::Window* mGreyWindow;
};

#endif // QUIT_MENU_H