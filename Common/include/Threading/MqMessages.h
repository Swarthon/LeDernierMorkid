#ifndef _MQMESSAGES_H_
#define _MQMESSAGES_H_

#include <assert.h>
#include <vector>

namespace Common {
	namespace Mq {
		/**
		* @enum MessageId
   		* @brief
		*	Type of Message transmitted
   		* @ingroup Common
		* @see
		*	MessageQueueSystem, MessageQueueSystem::queueSendMessage
   		*/
		enum MessageId {
			LOGICFRAME_FINISHED,
			GAME_ENTITY_ADDED,
			GAME_ENTITY_REMOVED,
			GAME_ENTITY_SCHEDULED_FOR_REMOVAL_SLOT,
			SDL_EVENT,
			NUM_MESSAGE_IDS
		};
	}
}

#endif
