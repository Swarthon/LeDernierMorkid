#include "QuitMenu.h"

QuitMenu::QuitMenu(CEGUI::Window* parent){
	mGreyWindow = parent->createChild("Generic/Image", "GreyWindow");
	mGreyWindow->setSize(CEGUI::USize(CEGUI::UDim(1,0),CEGUI::UDim(1,0)));
	mGreyWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
	mGreyWindow->setAlpha(0.5);
	CEGUI::Texture& tex = CEGUI::System::getSingleton().getRenderer()->createTexture("GreyWindowTexture", "grey.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Morkidios::Utils::addImageToImageset(tex, "GreyWindowImage");
	mGreyWindow->setProperty("Image","GreyWindowImage");
	mGreyWindow->setVisible(false);
	mGreyWindow->setAlwaysOnTop(true);
	mGreyWindow->setRiseOnClickEnabled(false);

	CEGUI::FontManager::getSingleton().createFreeTypeFont ("QuitWindowButtonFont", 30, true, "GreatVibes-Regular.ttf");

	mWindow = parent->createChild("AlfiskoSkin/FrameWindow","QuitWindow");
	mWindow->setHorizontalAlignment(CEGUI::HA_CENTRE);
	mWindow->setVerticalAlignment(CEGUI::VA_CENTRE);
	mWindow->setAlwaysOnTop(true);
	mWindow->setVisible(false);
	mWindow->setWidth(CEGUI::UDim(0.4,0));
	mWindow->setHeight(CEGUI::UDim(0.3,0));

	mCancel = mWindow->createChild("AlfiskoSkin/Button","QuitCancel");
	mCancel->setWidth(CEGUI::UDim(0.5,0));
	mCancel->setHeight(CEGUI::UDim(0.2,0));
	mCancel->setHorizontalAlignment(CEGUI::HA_CENTRE);
	mCancel->setYPosition(CEGUI::UDim(0.1,0));
	mCancel->setText((CEGUI::utf8*)_("Cancel"));
	mCancel->setProperty("Font","QuitWindowButtonFont");

	mMainMenu = mWindow->createChild("AlfiskoSkin/Button","QuitMenu");
	mMainMenu->setWidth(CEGUI::UDim(0.5,0));
	mMainMenu->setHeight(CEGUI::UDim(0.2,0));
	mMainMenu->setHorizontalAlignment(CEGUI::HA_CENTRE);
	mMainMenu->setYPosition(CEGUI::UDim(0.4,0));
	mMainMenu->setText((CEGUI::utf8*)_("Back to main menu"));
	mMainMenu->setProperty("Font","QuitWindowButtonFont");

	mDesktop = mWindow->createChild("AlfiskoSkin/Button","QuitDesktop");
	mDesktop->setWidth(CEGUI::UDim(0.5,0));
	mDesktop->setHeight(CEGUI::UDim(0.2,0));
	mDesktop->setHorizontalAlignment(CEGUI::HA_CENTRE);
	mDesktop->setYPosition(CEGUI::UDim(0.7,0));
	mDesktop->setText((CEGUI::utf8*)_("Back to desktop"));
	mDesktop->setProperty("Font","QuitWindowButtonFont");
}
QuitMenu::~QuitMenu(){
	if(CEGUI::WindowManager::getSingleton().isAlive(mWindow))
		mWindow->destroy();
	if(CEGUI::WindowManager::getSingleton().isAlive(mGreyWindow))
		mGreyWindow->destroy();
	CEGUI::System::getSingleton().getRenderer()->destroyTexture("GreyWindowTexture");
	CEGUI::ImageManager::getSingleton().destroy("GreyWindowImage");
}

// Various methodes
void QuitMenu::show(bool b){
	mWindow->setVisible(b);
	mGreyWindow->setVisible(b);
}

// Return value methodes
CEGUI::Window* QuitMenu::getWindow(){
	return mWindow;
}
CEGUI::Window* QuitMenu::getCancel(){
	return mCancel;
}
CEGUI::Window* QuitMenu::getMainMenu(){
	return mMainMenu;
}
CEGUI::Window* QuitMenu::getDesktop(){
	return mDesktop;
}
