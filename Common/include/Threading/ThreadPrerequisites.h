#ifndef _THREADPREREQUISITES_H_
#define _THREADPREREQUISITES_H_

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define THREAD_DEFINE(threadFunction) \
	unsigned long OGRE_THREAD_CALL_CONVENTION threadFunction##_internal(void* argName);
#else
#define THREAD_DEFINE(threadFunction) \
	void* OGRE_THREAD_CALL_CONVENTION threadFunction##_internal(void* argName);
#endif

#endif // _THREADPREREQUISITES_H_
