#ifndef _LOGICTHREADS_H_
#define _LOGICTHREADS_H_

#include <Threading/OgreBarrier.h>
#include <Threading/OgreThreads.h>

#include "Threading/ThreadPrerequisites.h"

/**
 * @ingroup Thread
 * Thread running the logic loop
 */
extern unsigned long logicThread(Ogre::ThreadHandle* threadHandle);
extern unsigned long logicThreadApp(Ogre::ThreadHandle* threadHandle);

extern THREAD_DEFINE(logicThread);

#endif // _LOGICTHREADS_H_
