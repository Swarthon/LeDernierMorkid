
#ifndef _YIELDTIMER_H_
#define _YIELDTIMER_H_

#include <OgreTimer.h>

namespace Common {

	/**
	 * @class YieldTimer
	 * @brief
	 *	Timer using an external timer to wait until frameTime + startTime = timer time
	 * @ingroup Common
	 */
	class YieldTimer {
		/**
                 * @var mExternalTimer
		 * Data shared between threads
		 * @see
 		 *	setThreadData, getThreadData
		 */
		Ogre::Timer* mExternalTimer;

	public:
		/**
		 * Constructor
	         * @param
 		 *	externalTimer Timer providing the extenral time
		 */
		YieldTimer(Ogre::Timer* externalTimer)
		                : mExternalTimer(externalTimer) {}

		/**
		 * Function waiting until time provided by mExternalTimer is higher than frameTime + startTime
		 * @param
 		 *	frameTime Time of a frame
		 * @param
 		 *	startTime Time of the start of the frame
		 * @return
 		 *	end time of the frame
		 */
		unsigned long yield(double frameTime, unsigned long startTime) {
			unsigned long endTime = mExternalTimer->getMicroseconds();

			while (frameTime * 1000000.0 > (endTime - startTime)) {
				endTime = mExternalTimer->getMicroseconds();

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
				SwitchToThread();
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX || OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
				sched_yield();
#endif
			}

			return endTime;
		}
	};
}

#endif // YieldTimer
