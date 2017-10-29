#ifndef _GRAPHICSSYSTEM_H_
#define _GRAPHICSSYSTEM_H_

#include "BaseSystem.h"
#include "GameEntityManager.h"
#include "SdlEmulationLayer.h"
#include "System/StaticPluginLoader.h"
#include <OgreColourValue.h>
#include <OgrePrerequisites.h>

#include <Threading/OgreUniformScalableTask.h>

#include <SDL.h>

namespace Common {
	class SdlInputHandler;

	/**
	 * @class GraphicsSystem
	 * @brief
	 *	GraphicsSystem managing Graphics
	 * @ingroup Common
	 */
	class GraphicsSystem : public BaseSystem, public Ogre::UniformScalableTask {
	public:
		/// LogicSystem to which communicate
		BaseSystem* mLogicSystem;
                /// GUISystem to update
                BaseSystem* mGUISystem;

	protected:
		/// Window of the program
		SDL_Window*      mSdlWindow;
		/// Handler managing input
		SdlInputHandler* mInputHandler;

		/// Root of Ogre stuff
		Ogre::Root*                mRoot;
		/// Window in which are rendered the graphics
		Ogre::RenderWindow*        mRenderWindow;
		/// SceneManager to manage graphics
		Ogre::SceneManager*        mSceneManager;
		/// Main Camera
		Ogre::Camera*              mCamera;
		/// Main CompositorWorkspace
		Ogre::CompositorWorkspace* mWorkspace;
		/// Path where the plugins are
		Ogre::String               mPluginsPath;
		/// Path to somewhere program can write
		Ogre::String               mWriteAccessFolder;
		/// Path where the resource are
		Ogre::String               mResourcePath;

		/// StaticPluginLoader
		StaticPluginLoader mStaticPluginLoader;

		/// Total time since the last logic frame
		float                mAccumTimeSinceLastLogicFrame;
		/// Current transform Id
		Ogre::uint32         mCurrentTransformIdx;
		/// List of GameEntity
		GameEntityVec        mGameEntities[Ogre::NUM_SCENE_MEMORY_MANAGER_TYPES];
		/// List of GameEntity to update
		GameEntityVec const* mThreadGameEntityToUpdate;
		/// Number of threads running simultaneously
		float                mThreadWeight;

		/// When true, the gameloop will stop
		bool mQuit;

		/// Background colour of the scene
		Ogre::ColourValue mBackgroundColour;

		/**
		 * Apply transmitted window event
		 * @param evt
		 *	Event containing window event to apply
		 */
		void handleWindowEvent(const SDL_Event& evt);

		/**
		 * Process received messages
		 * @param messageId
 		 *	The Mq::MessageId of the message, used to know what kind of message it is
		 * @param data
 		 *	The data sent with the message, could be anything. Depending on the messageId, a conversion will be needed
		 * @see
 		 *	Common::Mq::MessageQueueSystem::processIncomingMessage
		 */
		virtual void processIncomingMessage(Mq::MessageId messageId, const void* data);

		/**
		 * Method to add a resource location to for a given resource group.
		 * @param archName
		 *	The name of the resource location; probably a directory, zip file, URL etc.
		 * @param typeName
		 *	The codename for the resource type, which must correspond to the Archive factory which is providing the implementation.
		 * @param secName
		 *	The name of the resource group for which this location is to apply. ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME is the
		 *      default group which always exists, and can be used for resources which are unlikely to be unloaded until application
		 *      shutdown. Otherwise it must be the name of a group; if it has not already been created with createResourceGroup then it is created
		 *	automatically.
		 * @remarks
		 *	This method only call Ogre::ResourceGroupManager::getSingleton().addResourceLocation
		 */
		static void addResourceLocation(const Ogre::String& archName,
		                                const Ogre::String& typeName,
		                                const Ogre::String& secName);

		/// Method to add resources location  from the config file
		virtual void                       setupResources(void);
		/// Method to register hlms resources
		virtual void                       registerHlms(void);
		/// Method to load the resources
		virtual void                       loadResources(void);
		/// Method initializing the SceneManager
		virtual void                       chooseSceneManager(void);
		/// Method to create the Camera
		virtual void                       createCamera(void);
		/**
		 * Method setting up the Compositor
		 * @returns
		 *	Created CompositorWorkspace
		 */
		virtual Ogre::CompositorWorkspace* setupCompositor(void);

		/// Method to create the resource listener
		virtual void createResourceListener(void) {}
		/**
		 * Method called when an entity is added
		 * @param createdGameEntity
		 *	The GameEntity which has been created and need to be registered
		 */
		void gameEntityAdded(const GameEntityManager::CreatedGameEntity* createdGameEntity);
		/**
		 * Method called when an entity need to be remove
		 * @param toRemove
		 *	The GameEntity which need to be removed
		 */
		void gameEntityRemoved(GameEntity* toRemove);

	public:
		/**
		 * Constructor
		 * @param State
 		 *	State to which transmit the events
		 * @param backgroundColour
		 *	Background colour of the scene
		 */
		GraphicsSystem(State*        State,
		               Ogre::ColourValue backgroundColour = Ogre::ColourValue(0.f, 0.f, 0.f));
		/**
		 * Destructor
		 * @remarks
		 *	Do not forget to call deinitialize before calling the destructor
		 */
		virtual ~GraphicsSystem();

		/**
		 * Set the LogicSystem to which send the notifications
		 * @param logicSystem
		 *	LogicSystem to set
		 */
		void _notifyLogicSystem(BaseSystem* logicSystem) { mLogicSystem = logicSystem; }
		/**
		 * Initialize the system
		 * @param windowTitle
		 *	Title of the window which will be created
		 */
		void initialize(const Ogre::String& windowTitle);
		/**
		 * Deinitialize the system
		 */
		void deinitialize(void);

		/**
		 * Update the system. To call every frame
		 * @param timeSinceLast
		 *	Time past since last frame
		 */
		void update(float timeSinceLast);

		/**
		* Updates the SceneNodes of all the game entities in the container,
            	* interpolating them according to weight, reading the transforms from
            	* mCurrentTransformIdx and mCurrentTransformIdx-1.
        	* @param gameEntities
            	*	The container with entities to update.
        	* @param weight
            	*	The interpolation weight, ideally in range [0; 1]
	    	*/
		void updateGameEntities(const GameEntityVec& gameEntities, float weight);

		/**
		 * Overload Ogre::UniformScalableTask
		 * @see updateGameEntities
		 */
		virtual void execute(size_t threadId, size_t numThreads);

		/// Get mGameEntities
		const GameEntityVec& getGameEntities(Ogre::SceneMemoryMgrTypes type) const {
			return mGameEntities[type];
		}

		/// Get mInputHandler
		SdlInputHandler* getInputHandler(void) { return mInputHandler; }
		/// Set mQuit
		void             setQuit(void) { mQuit = true; }
		/// Get mQuit
		bool             getQuit(void) const { return mQuit; }
		/// Get mAccumTimeSinceLastLogicFrame
		float            getAccumTimeSinceLastLogicFrame(void) const {
			return mAccumTimeSinceLastLogicFrame;
		}

		/// Get mRoot
		Ogre::Root*                getRoot(void) const { return mRoot; }
		/// Get mRenderWindow
		Ogre::RenderWindow*        getRenderWindow(void) const { return mRenderWindow; }
		/// Get mSceneManager
		Ogre::SceneManager*        getSceneManager(void) const { return mSceneManager; }
		/// Get mCamera
		Ogre::Camera*              getCamera(void) const { return mCamera; }
		/// Get mWorkspace
		Ogre::CompositorWorkspace* getCompositorWorkspace(void) const { return mWorkspace; }
		/// Stop the Compositor
		virtual void               stopCompositor(void);
		/// Restart the Compositor
		virtual void               restartCompositor(void);
	};
}

#endif // _GRAPHICSSYSTEM_H_
