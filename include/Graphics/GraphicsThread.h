#ifndef _GRAPHICSTHREADS_H_
#define _GRAPHICSTHREADS_H_

#include <Threading/OgreBarrier.h>
#include <Threading/OgreThreads.h>

#include "Threading/ThreadPrerequisites.h"

extern unsigned long renderThread(Ogre::ThreadHandle* threadHandle);
extern unsigned long renderThreadApp(Ogre::ThreadHandle* threadHandle);

extern THREAD_DEFINE(renderThread);

#endif // _GRAPHICSTHREADS_H_
