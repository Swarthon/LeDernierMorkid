#ifndef _GRAPHICSSYSTEM_H_
#define _GRAPHICSSYSTEM_H_

#include "BaseSystem.h"
#include "GameEntityManager.h"
#include "System/StaticPluginLoader.h"
#include "SdlEmulationLayer.h"
#include <OgrePrerequisites.h>
#include <OgreColourValue.h>

#include <Threading/OgreUniformScalableTask.h>

#include <SDL.h>

namespace Common {
	class SdlInputHandler;

	class GraphicsSystem : public BaseSystem, public Ogre::UniformScalableTask {
	protected:
		BaseSystem          *mLogicSystem;

		SDL_Window          *mSdlWindow;
		SdlInputHandler     *mInputHandler;

		Ogre::Root                  *mRoot;
		Ogre::RenderWindow          *mRenderWindow;
		Ogre::SceneManager          *mSceneManager;
		Ogre::Camera                *mCamera;
		Ogre::CompositorWorkspace   *mWorkspace;
		Ogre::String                mPluginsPath;
		Ogre::String                mWriteAccessFolder;
		Ogre::String                mResourcePath;

		StaticPluginLoader          mStaticPluginLoader;

		float               mAccumTimeSinceLastLogicFrame;
		Ogre::uint32        mCurrentTransformIdx;
		GameEntityVec       mGameEntities[Ogre::NUM_SCENE_MEMORY_MANAGER_TYPES];
		GameEntityVec const *mThreadGameEntityToUpdate;
		float               mThreadWeight;

		bool                mQuit;

		Ogre::ColourValue   mBackgroundColour;

		void handleWindowEvent( const SDL_Event& evt );

		virtual void processIncomingMessage( Mq::MessageId messageId, const void *data );

		static void addResourceLocation( const Ogre::String &archName, const Ogre::String &typeName,
										 const Ogre::String &secName );
		virtual void setupResources(void);
		virtual void registerHlms(void);
		virtual void loadResources(void);
		virtual void chooseSceneManager(void);
		virtual void createCamera(void);
		virtual Ogre::CompositorWorkspace* setupCompositor(void);

		virtual void createResourceListener(void) {}

		void gameEntityAdded( const GameEntityManager::CreatedGameEntity *createdGameEntity );
		void gameEntityRemoved( GameEntity *toRemove );
	public:
		GraphicsSystem( GameState *gameState, Ogre::ColourValue backgroundColour = Ogre::ColourValue( 0.f, 0.f, 0.f ) );
		virtual ~GraphicsSystem();

		void _notifyLogicSystem( BaseSystem *logicSystem )      { mLogicSystem = logicSystem; }

		void initialize( const Ogre::String &windowTitle );
		void deinitialize(void);

		void update( float timeSinceLast );

		void updateGameEntities( const GameEntityVec &gameEntities, float weight );

		virtual void execute( size_t threadId, size_t numThreads );

		const GameEntityVec& getGameEntities( Ogre::SceneMemoryMgrTypes type ) const { return mGameEntities[type]; }

		SdlInputHandler* getInputHandler(void)                  { return mInputHandler; }

		void setQuit(void)                                      { mQuit = true; }
		bool getQuit(void) const                                { return mQuit; }

		float getAccumTimeSinceLastLogicFrame(void) const       { return mAccumTimeSinceLastLogicFrame; }

		Ogre::Root* getRoot(void) const                         { return mRoot; }
		Ogre::RenderWindow* getRenderWindow(void) const         { return mRenderWindow; }
		Ogre::SceneManager* getSceneManager(void) const         { return mSceneManager; }
		Ogre::Camera* getCamera(void) const                     { return mCamera; }
		Ogre::CompositorWorkspace* getCompositorWorkspace(void) const { return mWorkspace; }

		virtual void stopCompositor(void);
		virtual void restartCompositor(void);
	};
}

#endif // _GRAPHICSSYSTEM_H_
