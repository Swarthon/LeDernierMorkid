#ifndef _THREADMANAGER_H_
#define _THREADMANAGER_H_

#include <Threading/OgreBarrier.h>
#include <Threading/OgreThreads.h>

typedef unsigned long                     (*ThreadFunc)         (Ogre::ThreadHandle*);

struct ThreadData {
        Ogre::Barrier*  barrier;
};

class ThreadManager {
public:
        ThreadManager();

        virtual void registerThread(ThreadFunc thread, Ogre::THREAD_ENTRY_POINT internalThread);
        virtual void run();

        virtual void        setThreadData(ThreadData* threadData) { mThreadData = threadData; }
        virtual ThreadData* getThreadData()                       { return mThreadData; }

protected:
        std::vector<std::pair<ThreadFunc, Ogre::THREAD_ENTRY_POINT> > mRegisteredThreads;

        ThreadData* mThreadData;
};

#endif // _THREADMANAGER_H_
