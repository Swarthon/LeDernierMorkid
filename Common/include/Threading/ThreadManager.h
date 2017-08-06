#ifndef _THREADMANAGER_H_
#define _THREADMANAGER_H_

#include <Threading/OgreBarrier.h>
#include <Threading/OgreThreads.h>

namespace Common {

	/**
	 * @typedef ThreadFunc
	 * Typedef of thread function
	 * @ingroup Common
	 * @see
	 *	ThreadManager::registerThread
	 */
	typedef unsigned long (*ThreadFunc)(Ogre::ThreadHandle*);

	/**
	 * @struct ThreadData
	 * @brief
	 *	Basic struct containing data shared between threads
	 * @ingroup Common
	 *
	 * This struct should be overloaded to contain more data, like BaseSystem
	 */
	struct ThreadData {
		/**
		 * @var barrier
		 * Barrier to synchronize threads
		 */
		Ogre::Barrier* barrier;
	};

	/**
	 * @class ThreadManager
	 * @brief
	 *	Class to manage threads (register and run them)
	 * @ingroup Common
	 */
	class ThreadManager {
	public:
		/// Simple constructor
		ThreadManager();

		/**
	 	 * Add thread to list of the threads to run
	         * @param
 		 *	thread Thread function to add
        	 * @param
 		 *	internalThread Internal thread function to add
	         * @remarks
 		 *	To make a thread, create a static function which returns an unsigned long and takes an Ogre::ThreadHandle* as parameter. Then to get the internal thread, do in the header  :
	         * @code
 		 *	extern THREAD_DEFINE(thread);
		 * @endcode
	         * Then in the source file
	         * @code
 		 *	extern THREAD_DECLARE(thread);
 		 * @endcode
	         * And call the function :
	         * @code
 		 *	threadManager->registerThread(thread, THREAD_GET(thread));
 		 * @endcode
	         */
		virtual void registerThread(ThreadFunc thread, Ogre::THREAD_ENTRY_POINT internalThread);
		/**
		 * Run all the registered threads
		 * @exception
 		 *	Asserts can be thrown if mThreadData is not defined or if there isn't any thread in mRegisteredThreads
		 * @see
 		 *	registerThread, ThreadData
		 */
		virtual void run();

		/// Set mThreadData
		virtual void setThreadData(ThreadData* threadData) { mThreadData = threadData; }
		/// Get mThreadData
		virtual ThreadData*                    getThreadData() { return mThreadData; }

	protected:
		/**
		 * @var mRegisteredThreads
		 * Registered threads
		 * @see
 		 *	registerThread
		 */
		std::vector<std::pair<ThreadFunc, Ogre::THREAD_ENTRY_POINT> > mRegisteredThreads;

		/**
                 * @var mThreadData
		 * Data shared between threads
		 * @see
 		 *	setThreadData, getThreadData
		 */
		ThreadData* mThreadData;
	};
}

#endif // _THREADMANAGER_H_
