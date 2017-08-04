#include "Threading/ThreadManager.h"

ThreadManager::ThreadManager() {
	mThreadData = NULL;
}
//-----------------------------------------------------------------------------
void ThreadManager::registerThread(ThreadFunc thread, Ogre::THREAD_ENTRY_POINT internalThread) {
	mRegisteredThreads.push_back(std::make_pair(thread, internalThread));
}
//-----------------------------------------------------------------------------
void ThreadManager::run() {
	assert(mThreadData);
	assert(mRegisteredThreads.size());

	Ogre::Barrier barrier(mRegisteredThreads.size());
	mThreadData->barrier = &barrier;

	Ogre::ThreadHandlePtr threadHandles[mRegisteredThreads.size()];
	for (size_t i            = 0; i < mRegisteredThreads.size(); i++)
		threadHandles[i] = Ogre::Threads::CreateThread(mRegisteredThreads[i].second, i, mThreadData);

	Ogre::Threads::WaitForThreads(mRegisteredThreads.size(), threadHandles);
}
//-----------------------------------------------------------------------------
