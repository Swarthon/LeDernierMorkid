#ifndef _THREADMANAGER_H_
#define _THREADMANAGER_H_

#include <Threading/OgreBarrier.h>
#include <Threading/OgreThreads.h>

typedef unsigned long                     (*ThreadFunc)         (Ogre::ThreadHandle*);
typedef void* OGRE_THREAD_CALL_CONVENTION (*InternalThreadFunc) (void *);

struct ThreadData {
        Ogre::Barrier*  barrier;
};

class ThreadManager {
public:
        ThreadManager();

        virtual void registerThread(ThreadFunc thread, InternalThreadFunc internalThread);
        virtual void run();

        virtual void        setThreadData(ThreadData* threadData) { mThreadData = threadData; }
        virtual ThreadData* getThreadData()                       { return mThreadData; }

protected:
        std::vector<std::pair<ThreadFunc, InternalThreadFunc>> mRegisteredThreads;

        ThreadData* mThreadData;
};

#endif // _THREADMANAGER_H_
