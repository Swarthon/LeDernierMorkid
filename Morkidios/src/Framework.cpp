#include "Framework.h"

using namespace Ogre;

template<> Morkidios::Framework* Ogre::Singleton<Morkidios::Framework>::msSingleton = 0;

namespace Morkidios {

	Framework::Framework()
	{
		mRoot			= 0;
		mRenderWindow		= 0;
		mViewport		= 0;
		mLog			= 0;
		mTimer			= 0;
	}
	Framework::~Framework()
	{
		Framework::getSingletonPtr()->mLog->logMessage("Shutdown OGRE...");
		Input::destroy();
		if(mRoot)			delete mRoot;
	}

	bool Framework::initOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener, OIS::MouseListener *pMouseListener)
	{
		Ogre::LogManager* logMgr = new Ogre::LogManager();

		mLog = Ogre::LogManager::getSingleton().createLog("OgreLogfile.log", true, true, false);
		mLog->setDebugOutputEnabled(true);

		// Root
		mRoot = new Ogre::Root();

		int h, v;
		if (!mRoot->restoreConfig()){
			Ogre::RenderSystem *rs = mRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
			mRoot->setRenderSystem(rs);
			rs->setConfigOption("Full Screen", "Yes");
#ifdef _WIN32
			Utils::winGetScreenSize(h, v);
			rs->setConfigOption("Video Mode", (Utils::convertIntToString(h) + std::string(" x ") + Utils::convertIntToString(v) + std::string(" @ 32-bit colour")).c_str());
#else
			h = 1920;
			v = 1080;
			rs->setConfigOption("Video Mode", "1920 x 1080 @ 32-bit colour");
#endif
			rs->setConfigOption("VSync", "Yes");
		}
		mRenderWindow = mRoot->initialise(true, wndTitle);
		mRenderWindow->resize(h, v);
		mRenderWindow->setFullscreen(true, mRenderWindow->getWidth(), mRenderWindow->getHeight());
		// -------------------------------

		// Viewport
		mViewport = mRenderWindow->addViewport(0);
		mViewport->setBackgroundColour(Ogre::ColourValue(0.6f,0.6f,0.6f,1.f));

		mViewport->setCamera(0);
		// -------------------------------

		// Input
		size_t hWnd = 0;
		OIS::ParamList paramList;
		mRenderWindow->getCustomAttribute("WINDOW", &hWnd);

		paramList.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hWnd)));

		mInput = Input::getSingleton();

		mInput->mInputManager = OIS::InputManager::createInputSystem(paramList);

		mInput->mKeyboard = static_cast<OIS::Keyboard*>(mInput->mInputManager->createInputObject(OIS::OISKeyboard, true));
		mInput->mMouse = static_cast<OIS::Mouse*>(mInput->mInputManager->createInputObject(OIS::OISMouse, true));

		mInput->mMouse->getMouseState().height = mRenderWindow->getHeight();
		mInput->mMouse->getMouseState().width  = mRenderWindow->getWidth();

		if(pKeyListener == 0)
			mInput->mKeyboard->setEventCallback(this);
		else
			mInput->mKeyboard->setEventCallback(pKeyListener);

		if(pMouseListener == 0)
			mInput->mMouse->setEventCallback(this);
		else
			mInput->mMouse->setEventCallback(pMouseListener);
		// -------------------------------

		// Input Loading
		if(!mInput->load())
			mInput->save();
		// -------------------------------

		// Ressources
		Ogre::String secName, typeName, archName;
		Ogre::ConfigFile cf;
		cf.load("resources.cfg");

		Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
		while (seci.hasMoreElements())
		{
			secName = seci.peekNextKey();
			Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
			Ogre::ConfigFile::SettingsMultiMap::iterator i;
			for (i = settings->begin(); i != settings->end(); ++i)
			{
				typeName = i->first;
				archName = i->second;
				Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
			}
		}
		Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
		Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
		// -------------------------------

		mTimer = new Ogre::Timer();
		mTimer->reset();

		mRenderWindow->setActive(true);

		// CEGUI
		mLog->logMessage("Initializing CEGUI");
		CEGUI::OgreRenderer::bootstrapSystem();
		CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");
		CEGUI::Font::setDefaultResourceGroup("Fonts");
		CEGUI::Scheme::setDefaultResourceGroup("Schemes");
		CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
		CEGUI::WindowManager::setDefaultResourceGroup("Layouts");

		CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "RootWindow"));
		mLog->logMessage("CEGUI initialized");
		// -------------------------------

		// Language
		Language::getSingleton()->load();
		// -------------------------------

		return true;
	}

	bool Framework::keyPressed(const OIS::KeyEvent &keyEventRef)
	{
		return true;
	}
	bool Framework::keyReleased(const OIS::KeyEvent &keyEventRef)
	{
		return true;
	}
	bool Framework::mouseMoved(const OIS::MouseEvent &evt)
	{
		return true;
	}
	bool Framework::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
	{
		return true;
	}
	bool Framework::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
	{
		return true;
	}

	void Framework::updateOgre(double timeSinceLastFrame)
	{
		CEGUI::System::getSingleton().injectTimePulse(timeSinceLastFrame);
		CEGUI::System::getSingleton().getDefaultGUIContext().injectTimePulse(timeSinceLastFrame);
	}

}
