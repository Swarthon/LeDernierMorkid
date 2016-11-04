#include "Inventory.h"

namespace Morkidios {
	Inventory::Inventory(){
		mObjectEntity = NULL;
		mActiveButton = NULL;
	}
	Inventory::~Inventory(){
	}

	void Inventory::enter(){
		mSceneManager = Morkidios::Framework::getSingletonPtr()->mRoot->createSceneManager(Ogre::ST_GENERIC);
		mSceneManager->setAmbientLight(Ogre::ColourValue(1.f, 1.f, 1.f));
		mCamera = mSceneManager->createCamera("InventoryCam");
		mCamera->setAspectRatio(Ogre::Real(Morkidios::Framework::getSingletonPtr()->mViewport->getActualWidth()) / Ogre::Real(Morkidios::Framework::getSingletonPtr()->mViewport->getActualHeight()));
		Morkidios::Framework::getSingletonPtr()->mViewport->setCamera(mCamera);

		createScene();
		createGUI();
	}
	void Inventory::createScene(){
	}
	void Inventory::createGUI(){
		CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(true);

		mWindow = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->createChild("DefaultWindow", "Inventory");
		mWindow->setSize(CEGUI::USize(CEGUI::UDim(1,0),CEGUI::UDim(1,0)));
		mWindow->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
		mWindow->setAlwaysOnTop(true);

		CEGUI::FontManager::getSingleton().createFreeTypeFont ("InventoryWeaponLabelFont", 50, true, "GreatVibes-Regular.ttf");
		CEGUI::FontManager::getSingleton().createFreeTypeFont ("InventoryLeftBarButton", 40, true, "GreatVibes-Regular.ttf");
		CEGUI::FontManager::getSingleton().createFreeTypeFont ("InventoryEquiperButton", 20, true, "GreatVibes-Regular.ttf");

		mTypePane = static_cast<CEGUI::ScrollablePane*>(mWindow->createChild("TaharezLook/ScrollablePane","InventoryPane"));
		mTypePane->setWidth(CEGUI::UDim(0.2,0));
		mTypePane->setHeight(CEGUI::UDim(0.98,0));
		mTypePane->setPosition(CEGUI::UVector2(CEGUI::UDim(0.01,0), CEGUI::UDim(0.01,0)));

		mTypeBar = static_cast<CEGUI::VerticalLayoutContainer*>(CEGUI::WindowManager::getSingleton().createWindow("VerticalLayoutContainer"));
		mTypePane->addChild(mTypeBar);

		createLeftBarButton("InventoryWeaponsButton", "Weapons", Object::Weapon);
		createLeftBarButton("InventoryArmoursButton", "Armours", Object::Armour);
		createLeftBarButton("InventoryObjectsQuestButton", "Quest Objects", Object::Quest);
		createLeftBarButton("InventoryObjectsVariousButton", "Various Objects", Object::Various);
		createLeftBarButton("InventoryObjectsEatablesButton", "Eatable Objects", Object::Eatable);
		createLeftBarButton("InventoryUtilsButton", "Utils", Object::Utils);

		mObjectPane = static_cast<CEGUI::ScrollablePane*>(mWindow->createChild("TaharezLook/ScrollablePane", "InventoryWeaponWindow"));
		mObjectPane->setWidth(CEGUI::UDim(0.5,0));
		mObjectPane->setHeight(CEGUI::UDim(0.98,0));
		mObjectPane->setPosition(CEGUI::UVector2(CEGUI::UDim(0.26,0), CEGUI::UDim(0.18,0)));

		mObjectBar = static_cast<CEGUI::VerticalLayoutContainer*>(CEGUI::WindowManager::getSingleton().createWindow("VerticalLayoutContainer"));
		mObjectPane->addChild(mObjectBar);

		mObjectLabel = mWindow->createChild("Generic/Label", "InventoryWeaponLabel");
		mObjectLabel->setHeight(CEGUI::UDim(0.1666666666666666666,0));
		mObjectLabel->setWidth(CEGUI::UDim(0.5,0));
		mObjectLabel->setPosition(CEGUI::UVector2(CEGUI::UDim(0.26,0), CEGUI::UDim(0.01,0)));
		mObjectLabel->setProperty("Font", "InventoryWeaponLabelFont");
		mObjectLabel->setProperty("NormalTextColour", "FFFFFFFF");
		mObjectLabel->setProperty("DisabledTextColour", "FFFFFFFF");


		mDescBar = static_cast<CEGUI::VerticalLayoutContainer*>(CEGUI::WindowManager::getSingleton().createWindow("VerticalLayoutContainer"));
		mDescBar->setPosition(CEGUI::UVector2(CEGUI::UDim(0.78,0),CEGUI::UDim(0.01,0)));
		mDescBar->setWidth(CEGUI::UDim(0.1,0));
		mWindow->addChild(mDescBar);

		mDesc3D = mWindow->createChild("TaharezLook/StaticImage", "Inventory3D");
		mDesc3D->setWidth(CEGUI::UDim(0.2,0));
		mDesc3D->setHeight(CEGUI::UDim(0.58,0));
		mDesc3D->setPosition(CEGUI::UVector2(CEGUI::UDim(0.78,0),CEGUI::UDim(0.01,0)));

		mDescText = mWindow->createChild("TaharezLook/Listbox", "InventoryText");
		mDescText->setWidth(CEGUI::UDim(0.2,0));
		mDescText->setHeight(CEGUI::UDim(0.38,0));
		mDescText->setPosition(CEGUI::UVector2(CEGUI::UDim(0.78,0),CEGUI::UDim(0.6,0)));

		// Desc3D
		mTexture = Framework::getSingletonPtr()->mRoot->getTextureManager()->createManual("RTTInventory", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, 512, 512, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET);
		Ogre::RenderTexture *rtex = mTexture->getBuffer()->getRenderTarget();

		mObjectCamera = mSceneManager->createCamera("InventoryObjectCamera");
		mObjectCamera->setNearClipDistance(1);
		mObjectViewport = rtex->addViewport(mObjectCamera);
		mObjectViewport->setOverlaysEnabled(false);
		mObjectViewport->setClearEveryFrame(true);
		mObjectViewport->setBackgroundColour(Ogre::ColourValue::Black);

		mObjectSceneNode = mSceneManager->getRootSceneNode()->createChildSceneNode("InventoryObjectSceneNode");
		mObjectSceneNode->yaw(Ogre::Degree(90));

		mObjectCamera->setPosition(Ogre::Vector3(20,0,0));
		mObjectCamera->lookAt(Ogre::Vector3(0,0,0));

		CEGUI::Texture &guiTex = ((CEGUI::OgreRenderer*)(CEGUI::System::getSingleton().getRenderer()))->createTexture("TextureInventory", mTexture);

		const CEGUI::Rectf rect(CEGUI::Vector2f(0.0f, 0.0f), guiTex.getOriginalDataSize());
		CEGUI::BasicImage* image = (CEGUI::BasicImage*)( &CEGUI::ImageManager::getSingleton().create("BasicImage", "InventoryObjectImage"));
		image->setTexture(&guiTex);
		image->setArea(rect);
		image->setAutoScaled(CEGUI::ASM_Both);

		mDesc3D->setProperty("Image", "InventoryObjectImage");
		// --------------------------------------------------------
	}
	void Inventory::exit(){
		Morkidios::Framework::getSingletonPtr()->mRoot->destroySceneManager(mSceneManager);
		Framework::getSingletonPtr()->mRoot->getTextureManager()->remove("RTTInventory");
		CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->destroyChild("Inventory");

		CEGUI::System::getSingleton().getRenderer()->destroyTexture("TextureInventory");
		CEGUI::ImageManager::getSingleton().destroy("InventoryObjectImage");
	}
	void Inventory::resume(){
		CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(true);
		mWindow->setVisible(true);
		Morkidios::Framework::getSingletonPtr()->mViewport->setCamera(mCamera);
	}
	bool Inventory::pause(){
		CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(false);
		mWindow->setVisible(false);
		clear();

		return true;
	}

	bool Inventory::keyPressed(const OIS::KeyEvent &keyEventRef){
		CEGUI::GUIContext& context = CEGUI::System::getSingleton().getDefaultGUIContext();
		context.injectKeyDown((CEGUI::Key::Scan)keyEventRef.key);
		context.injectChar((CEGUI::Key::Scan)keyEventRef.text);
		return true;
	}
	bool Inventory::keyReleased(const OIS::KeyEvent &keyEventRef){
		CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp((CEGUI::Key::Scan)keyEventRef.key);
		if(keyEventRef.key == OIS::KC_ESCAPE)
			popState();
		return true;
	}

	bool Inventory::mouseMoved(const OIS::MouseEvent &evt){
		CEGUI::System &sys = CEGUI::System::getSingleton();
		sys.getDefaultGUIContext().injectMouseMove(evt.state.X.rel, evt.state.Y.rel);

		if (evt.state.Z.rel) // Scroll wheel.
			sys.getDefaultGUIContext().injectMouseWheelChange(evt.state.Z.rel / 120.0f);
		return true;
	}
	bool Inventory::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id){
		CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(Morkidios::Utils::convertButton(id));
		return true;
	}
	bool Inventory::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id){
		CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(Morkidios::Utils::convertButton(id));
		return true;
	}

	void Inventory::update(double timeSinceLastFrame){
		double numRoundPerMinutes = 20;
		double rotate = timeSinceLastFrame * (numRoundPerMinutes / 60.0) * 360;
		if(mObjectEntity){
			mObjectSceneNode->yaw(Ogre::Degree(rotate));
			mObjectSceneNode->pitch(Ogre::Degree(rotate));
			mObjectSceneNode->roll(Ogre::Degree(rotate));
		}
	}

	// Private methodes
	void Inventory::createLeftBarButton(std::string name, std::string text, Object::Type t){
		if(!mWindow){
			std::cout << "Error : Trying to use createLeftBarButton but mWindow is not initialized\n";
			shutdown();
		}
		CEGUI::Window* wnd = mTypeBar->createChild("AlfiskoSkin/Button", name);
		wnd->setWidth(CEGUI::UDim(1,0));
		wnd->setHeight(CEGUI::UDim(0.1666666666666666666,0));
		int* i = (int*)malloc(sizeof(int));		// It must be a pointer, else the UserData not work
		*i = t;
		wnd->setUserData(i);
		wnd->setText((CEGUI::utf8*)_(text.c_str()));
		wnd->setProperty("Font","InventoryLeftBarButton");
		wnd->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Inventory::typeBarButtonClicked, this));
	}
	void Inventory::createObjectButton(std::string name, Object* o, CEGUI::Window* parent){
		CEGUI::Window* button = parent->createChild("AlfiskoSkin/Button", std::string("Inventory") + name);
		button->setHeight(CEGUI::UDim(0.1666666666666666666,0));
		button->setWidth(CEGUI::UDim(1,0));
		button->setText((CEGUI::utf8*)_(o->getName().c_str()));
		button->setProperty("Font","InventoryLeftBarButton");
		button->setUserData(o);
		button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Inventory::objectButtonClicked, this));

		CEGUI::HorizontalLayoutContainer* layout = static_cast<CEGUI::HorizontalLayoutContainer*>(CEGUI::WindowManager::getSingleton().createWindow("HorizontalLayoutContainer"));
		layout->setVisible(false);
		button->addChild(layout);

		CEGUI::Window* equipe = layout->createChild("AlfiskoSkin/Button", std::string("InventoryEquiperGauche") + name);
		equipe->setText("Main Gauche");
		equipe->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Inventory::equipeButtonClicked, this));
		equipe->setProperty("Font","InventoryEquiperButton");
		equipe->setWidth(CEGUI::UDim(0.2,0));

		equipe = layout->createChild("AlfiskoSkin/Button", std::string("InventoryEquiperDroite") + name);
		equipe->setText("Main Droite");
		equipe->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Inventory::equipeButtonClicked, this));
		equipe->setProperty("Font","InventoryEquiperButton");
		equipe->setWidth(CEGUI::UDim(0.2,0));

		CEGUI::Window* drop = layout->createChild("AlfiskoSkin/Button", std::string("InventoryDrop") + name);
		drop->setText("Jeter");
		drop->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Inventory::drop, this));
		drop->setProperty("Font","InventoryEquiperButton");
		drop->setWidth(CEGUI::UDim(0.2,0));

		layout->setPosition(CEGUI::UVector2(CEGUI::UDim(0.4,0),CEGUI::UDim(0.8,0)));
	}
	bool Inventory::typeBarButtonClicked(const CEGUI::EventArgs& e){
		const CEGUI::WindowEventArgs& we = static_cast<const CEGUI::WindowEventArgs&>(e);
		mObjectLabel->setText(we.window->getText());
		refresh(*(Object::Type*)(we.window->getUserData()));
		return true;
	}
	bool Inventory::objectButtonClicked(const CEGUI::EventArgs& e){
		const CEGUI::WindowEventArgs& we = static_cast<const CEGUI::WindowEventArgs&>(e);
		CEGUI::Window* w = we.window;
		w->getChildAtIdx(0)->setVisible(true);

		if(mActiveButton && mActiveButton != w)
			mActiveButton->getChildAtIdx(0)->setVisible(false);
		mActiveButton = w;

		if(mObjectEntity){
			mObjectEntity->_getManager()->destroyEntity(mObjectEntity);
			mObjectEntity = NULL;
		}
		mObjectEntity = ((Object*)w->getUserData())->getEntity()->clone(((Object*)w->getUserData())->getName()+std::string("Inventory"));
		mObjectSceneNode->attachObject(mObjectEntity);

		return true;
	}
	bool Inventory::equipeButtonClicked(const CEGUI::EventArgs& e){
		const CEGUI::WindowEventArgs& we = static_cast<const CEGUI::WindowEventArgs&>(e);
		CEGUI::Window* w = we.window->getParent()->getParent();

		if(we.window->getText() == "Main Gauche")
			Hero::getSingleton()->equipeLeftHand((Object*)w->getUserData());
		if(we.window->getText() == "Main Droite")
			Hero::getSingleton()->equipeRightHand((Object*)w->getUserData());

		return true;
	}
	bool Inventory::drop(const CEGUI::EventArgs& e){
		const CEGUI::WindowEventArgs& we = static_cast<const CEGUI::WindowEventArgs&>(e);
		CEGUI::Window* w = we.window->getParent()->getParent();

		Object* o = (Object*)w->getUserData();
		Morkidios::Hero::getSingleton()->drop(o);
		Morkidios::Terrain::getActiveTerrain()->addDroppedObject(o);

		refresh(o->getType());

		return true;
	}
	void Inventory::clear(){
		while(mObjectBar->getChildCount() != 0)
			mObjectBar->getChildAtIdx(0)->destroy();
		if(mObjectEntity){
			mObjectEntity->_getManager()->destroyEntity(mObjectEntity);
			mObjectEntity = NULL;
		}
		mActiveButton = NULL;
	}
	void Inventory::refresh(Object::Type t){
		clear();
		for(int i = 0; i < Hero::getSingleton()->getObjects().size(); i++)
			if(Hero::getSingleton()->getObjects()[i]->getType() == t)
				createObjectButton(Hero::getSingleton()->getObjects()[i]->getName(), Hero::getSingleton()->getObjects()[i], mObjectBar);
	}
}
