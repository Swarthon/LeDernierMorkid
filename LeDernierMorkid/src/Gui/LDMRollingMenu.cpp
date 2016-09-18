#include "LDMRollingMenu.h"

LDMRollingMenu::LDMRollingMenu(int numFaces, double faceWidth, double faceHeight){
	// CEGUI in Ogre
	CEGUI::Window* root = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "RollingMenuRoot");
	for(int i = 0; i < numFaces; i++){
		CEGUI::Window* w = root->createChild("AlfiskoSkin/Button",std::string("RollingMenuFace") + Morkidios::Utils::convertIntToString(i));
		w->setSize(CEGUI::USize(CEGUI::UDim(1,0), CEGUI::UDim(1.0/double(numFaces),0)));
		w->setYPosition(CEGUI::UDim(1.0/(double)(numFaces)*(double)(i),0));
		w->setText(Morkidios::Utils::convertIntToString(i));
		mFaces.push_back(w);
	}
	 
	CEGUI::System& ceguiSystem = CEGUI::System::getSingleton();
	mRenderer = static_cast<CEGUI::OgreRenderer *>(ceguiSystem.getRenderer());
	CEGUI::Sizef size(static_cast<float>(faceWidth*5), static_cast<float>(faceHeight*5*numFaces));
	mTextureTarget = mRenderer->createTextureTarget();
	mTextureTarget->declareRenderSize(size);
	 
	mContext = &ceguiSystem.createGUIContext(static_cast<CEGUI::RenderTarget&>(*mTextureTarget) );
	mContext->setRootWindow(root);

	CEGUI::Texture& renderTargetTexture = mTextureTarget->getTexture();
	Ogre::TexturePtr ogreTexture = static_cast<CEGUI::OgreTexture&>(renderTargetTexture).getOgreTexture();

	Ogre::MaterialPtr matPtr = Ogre::MaterialManager::getSingleton().create("RollingMenuTexture",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Ogre::Technique* technique = matPtr->getTechnique(0);
	Ogre::Pass* pass = technique->getPass(0);
	Ogre::TextureUnitState* textureUnit = pass->createTextureUnitState();
	textureUnit->setTextureName(ogreTexture->getName());

	// Ogre in CEGUI
	mSceneManager = Morkidios::Framework::getSingletonPtr()->mRoot->createSceneManager("OctreeSceneManager");
	mSceneManager->setAmbientLight(Ogre::ColourValue(1.f, 1.f, 1.f));

	mTexture = Morkidios::Framework::getSingletonPtr()->mRoot->getTextureManager()->createManual("RTTRollingMenu", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, 2000, 2000, 0, Ogre::PF_R8G8B8A8, Ogre::TU_RENDERTARGET);
	Ogre::RenderTexture *rtex = mTexture->getBuffer()->getRenderTarget();

	mCamera = mSceneManager->createCamera("RollingMenuCamera");
	mCamera->setNearClipDistance(1);
	mCamera->lookAt(1,0,0);
	mViewport = rtex->addViewport(mCamera);
	mViewport->setBackgroundColour(Ogre::ColourValue(0,0,0,0));
	mViewport->setClearEveryFrame(true);

	CEGUI::Texture &guiTex = ((CEGUI::OgreRenderer*)(CEGUI::System::getSingleton().getRenderer()))->createTexture("RTTRollingMenu", mTexture);

	const CEGUI::Rectf rect(CEGUI::Vector2f(0.0f, 0.0f), guiTex.getOriginalDataSize());
	CEGUI::BasicImage* image = (CEGUI::BasicImage*)( &CEGUI::ImageManager::getSingleton().create("BasicImage", "RollingMenuImage"));
	image->setTexture(&guiTex);
	image->setArea(rect);
	image->setAutoScaled(CEGUI::ASM_Both);

	mWindow = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->createChild("TaharezLook/StaticImage", "RollingMenu");
	mWindow->setProperty("Image", "RollingMenuImage");
	mWindow->setSize(CEGUI::USize(CEGUI::UDim(0.5,0),CEGUI::UDim(0.5,0)));
	mWindow->setProperty("BackgroundEnabled", "False");
	mWindow->setProperty("FrameEnabled", "False");
	mWindow->setVerticalAlignment(CEGUI::VA_CENTRE);
	mWindow->setXPosition(CEGUI::UDim(0.06,0));

	mClickArea = mWindow->createChild("DefaultWindow", "RollingMenuClickArea");
	mClickArea->setHorizontalAlignment(CEGUI::HA_CENTRE);
	mClickArea->setVerticalAlignment(CEGUI::VA_CENTRE);
	mClickArea->setSize(CEGUI::USize(CEGUI::UDim(0.75,0),CEGUI::UDim(0.5,0)));
	mClickArea->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&LDMRollingMenu::activeClicked, this));

	CEGUI::Texture& tex = CEGUI::System::getSingleton().getRenderer()->createTexture("DownArrowTexture", "DownArrow.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Morkidios::Utils::addImageToImageset(tex, "DownArrow");
	CEGUI::Texture& tex2 = CEGUI::System::getSingleton().getRenderer()->createTexture("UpArrowTexture", "UpArrow.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Morkidios::Utils::addImageToImageset(tex2, "UpArrow");
	
	mDownButtonLeft = mWindow->createChild("TaharezLook/ImageButton", "DownArrow");
	mDownButtonLeft->setHorizontalAlignment(CEGUI::HA_LEFT);
	mDownButtonLeft->setVerticalAlignment(CEGUI::VA_BOTTOM);
	mDownButtonLeft->setProperty("NormalImage","DownArrow");
	mDownButtonLeft->setProperty("HoverImage","DownArrow");
	mDownButtonLeft->setProperty("DisabledImage","DownArrow");
	mDownButtonLeft->setProperty("PushedImage","DownArrow");
	mDownButtonLeft->setSize(CEGUI::USize(CEGUI::UDim(0.1,0),CEGUI::UDim(0.1,0)));
	mDownButtonLeft->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LDMRollingMenu::down, this));

	mUpButtonLeft = mWindow->createChild("TaharezLook/ImageButton", "UpArrow");
	mUpButtonLeft->setHorizontalAlignment(CEGUI::HA_LEFT);
	mUpButtonLeft->setVerticalAlignment(CEGUI::VA_TOP);
	mUpButtonLeft->setProperty("NormalImage","UpArrow");
	mUpButtonLeft->setProperty("HoverImage","UpArrow");
	mUpButtonLeft->setProperty("DisabledImage","UpArrow");
	mUpButtonLeft->setProperty("PushedImage","UpArrow");
	mUpButtonLeft->setSize(CEGUI::USize(CEGUI::UDim(0.1,0),CEGUI::UDim(0.1,0)));
	mUpButtonLeft->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LDMRollingMenu::up, this));

	createMenuShape(numFaces, faceWidth, faceHeight);

	mNumFaces = numFaces;
	mLastClick = 0;
	mToRotate = Ogre::Degree(0);

	mActive = mFaces[0];

	update(0);
}
LDMRollingMenu::~LDMRollingMenu(){
	CEGUI::System::getSingleton().destroyGUIContext(*mContext);
	CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow()->destroyChild("RollingMenu");
	Morkidios::Framework::getSingletonPtr()->mRoot->destroySceneManager(mSceneManager);
	CEGUI::System::getSingleton().getRenderer()->destroyTexture("RTTRollingMenu");
	CEGUI::ImageManager::getSingleton().destroy("RollingMenuImage");
	Morkidios::Framework::getSingletonPtr()->mRoot->getTextureManager()->remove("RTTRollingMenu");
	Morkidios::Framework::getSingletonPtr()->mRoot->getTextureManager()->remove("RollingMenuTexture");
	mRenderer->destroyTextureTarget(mTextureTarget);
	CEGUI::System::getSingleton().getRenderer()->destroyTexture("DownArrowTexture");
	CEGUI::System::getSingleton().getRenderer()->destroyTexture("UpArrowTexture");
	CEGUI::ImageManager::getSingleton().destroy("DownArrow");
	CEGUI::ImageManager::getSingleton().destroy("UpArrow");
}

// Input methodes
bool LDMRollingMenu::keyPressed(const OIS::KeyEvent &keyEventRef){
	return true;
}
bool LDMRollingMenu::keyReleased(const OIS::KeyEvent &keyEventRef){
	if(keyEventRef.key == OIS::KC_RETURN)
		activeClicked();
	return true;
}

bool LDMRollingMenu::mouseMoved(const OIS::MouseEvent &evt){	
	if(mLastClick == 0){
		if(evt.state.Z.rel < 0)
			down();
		if(evt.state.Z.rel > 0)
			up();
	}
	return true;
}
bool LDMRollingMenu::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id){
	return true;
}
bool LDMRollingMenu::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id){
	return true;
}

// Various methodes
void LDMRollingMenu::update(double timeSinceLastFrame){
	mRenderer->beginRendering();
	mTextureTarget->clear();
	mContext->draw();
	mRenderer->endRendering();

	mLastClick -= timeSinceLastFrame;
	if(mLastClick < 0)
		mLastClick = 0;
	if(mLastClick == 0){
		if(Morkidios::Framework::getSingletonPtr()->mInput->mKeyboard->isKeyDown(OIS::KC_UP))
			up();
		if(Morkidios::Framework::getSingletonPtr()->mInput->mKeyboard->isKeyDown(OIS::KC_DOWN))
			down();
	}

	if(mToRotate != Ogre::Degree(0)){
		Ogre::Degree d;
		if(mToRotate > Ogre::Degree((720*timeSinceLastFrame)/1000) || mToRotate < -Ogre::Degree((720*timeSinceLastFrame)/1000))
			d = Ogre::Degree((720*timeSinceLastFrame)/1000);
		else
			d = Ogre::Math::Abs(mToRotate);
		d = (mToRotate < Ogre::Degree(0)) ? d * -1 : d * 1;
		mSceneNode->pitch(d);
		mToRotate -= d;
	}
}
void LDMRollingMenu::show(bool b){
	mWindow->setVisible(b);
}

// Return value methodes
std::vector<CEGUI::Window*> LDMRollingMenu::getFaces(){
	return mFaces;
}

// Private methodes
void LDMRollingMenu::createMenuShape(int numFaces, double faceWidth, double faceHeight){
	double x[numFaces];
	double y[numFaces];
	double r = faceHeight / (2 * sin(3.141592/numFaces));
	double XPosition = r - 50 / (2 * sin(3.141592/8));
	double YPosition = 0;
	for(int i = 0; i < numFaces; i++){
		x[i] = r * cos(2*3.141592*i/numFaces);
		y[i] = r * sin(2*3.141592*i/numFaces);
	}

	Ogre::ManualObject* mo = new Ogre::ManualObject("RollingMenuObject");
	mo->begin("RollingMenuTexture",  Ogre::RenderOperation::OT_TRIANGLE_LIST);

	for(int i = 0; i < numFaces; i++){
		mo->position(-faceWidth/2,x[i],y[i]);		mo->textureCoord(0,1.0/double(numFaces)*(double)(i));
		mo->position(faceWidth/2,x[i],y[i]);		mo->textureCoord(1,1.0/double(numFaces)*(double)(i));
		if(i != numFaces - 1){
			mo->position(-faceWidth/2,x[i+1],y[i+1]);	mo->textureCoord(0,1.0/double(numFaces)*(double)(i+1));
			mo->position(faceWidth/2,x[i+1],y[i+1]);	mo->textureCoord(1,1.0/double(numFaces)*(double)(i+1));
		}
		else {
			mo->position(-faceWidth/2,x[0],y[0]);	mo->textureCoord(0,1.0/double(numFaces)*(double)(i+1));
			mo->position(faceWidth/2,x[0],y[0]);	mo->textureCoord(1,1.0/double(numFaces)*(double)(i+1));
		}
	}
	int i1 = 0, i2 = 1, i3 = 2, i4 = 3;
	for(int i = 0; i < numFaces; i++){
		mo->triangle(i1,i3,i2);
		mo->triangle(i2,i3,i4);
		mo->triangle(i1,i2,i3);
		mo->triangle(i2,i4,i3);

		i1 += 4;
		i2 += 4;
		i3 += 4;
		i4 += 4;
	}

	mo->end();
	mo->convertToMesh(mo->getName());

	mSceneNode = mSceneManager->getRootSceneNode()->createChildSceneNode("CubeScene");
	mSceneNode->attachObject(mSceneManager->createEntity(mo->getName()));
	mSceneNode->yaw(Ogre::Degree(-90), Ogre::SceneNode::TS_PARENT);
	mSceneNode->setPosition(Ogre::Vector3(200+XPosition,0+YPosition,0));
	mSceneNode->pitch(Ogre::Degree(90-360/numFaces/2));
}
void LDMRollingMenu::down(){
	mToRotate -= Ogre::Degree(360/mNumFaces);
	mLastClick += 360/mNumFaces*1000/720;

	std::vector<CEGUI::Window*>::iterator it = std::find(mFaces.begin(), mFaces.end(), mActive);
	if(it != mFaces.end() && it+1 != mFaces.end())
		mActive = *(it+1);
	if(it+1 == mFaces.end())
		mActive = *mFaces.begin();
}

void LDMRollingMenu::up(){
	mToRotate += Ogre::Degree(360/mNumFaces);
	mLastClick += 360/mNumFaces*1000/720;

	std::vector<CEGUI::Window*>::iterator it = std::find(mFaces.begin(), mFaces.end(), mActive);
	if(it != mFaces.end() && it != mFaces.begin())
		mActive = *(it-1);
	if(it == mFaces.begin())
		mActive = *(mFaces.end()-1);
}

// 
bool LDMRollingMenu::activeClicked(const CEGUI::EventArgs& evt){
	CEGUI::EventArgs wnd = CEGUI::WindowEventArgs(mActive);
	mActive->fireEvent(CEGUI::Window::EventMouseClick, wnd);
}