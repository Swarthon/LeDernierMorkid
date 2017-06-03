#ifndef _Mq_MessageQueueSystem_H_
#define _Mq_MessageQueueSystem_H_

#include "MqMessages.h"
#include <Threading/OgreLightweightMutex.h>
#include <OgreCommon.h>
#include <OgreFastArray.h>

#include <map>

namespace Common {
	namespace Mq {
		class MessageQueueSystem {
			static const size_t cSizeOfHeader;

			typedef Ogre::FastArray<unsigned char> MessageArray;
			typedef std::map<MessageQueueSystem*, MessageArray> PendingMessageMap;

			Ogre::LightweightMutex  mMessageQueueMutex;

			PendingMessageMap   mPendingOutgoingMessages;
			MessageArray        mIncomingMessages[2];

			template <typename T> static void storeMessageToQueue( MessageArray &queue, Mq::MessageId messageId, const T &msg ) {
				//Save the current offset.
				const size_t startOffset = queue.size();

				//Enlarge the queue. Preserve alignment.
				const size_t totalSize = Ogre::alignToNextMultiple( cSizeOfHeader + sizeof(T),
																	sizeof(size_t) );
				queue.resize( queue.size() + totalSize );

				//Write the header: the Size and the MessageId
				*reinterpret_cast<Ogre::uint32*>( queue.begin() + startOffset ) = totalSize;
				*reinterpret_cast<Ogre::uint32*>( queue.begin() + startOffset +
												  sizeof(Ogre::uint32) )        = messageId;

				//Write the actual message.
				T *dstPtr = reinterpret_cast<T*>( queue.begin() + startOffset + cSizeOfHeader );
				memcpy( dstPtr, &msg, sizeof( T ) );
			}

		public:
			virtual ~MessageQueueSystem() {}

			template <typename T>
			void queueSendMessage( MessageQueueSystem *dstSystem, Mq::MessageId messageId, const T &msg ) {
				storeMessageToQueue( mPendingOutgoingMessages[dstSystem], messageId, msg );
			}

			void flushQueuedMessages(void) {
				PendingMessageMap::iterator itMap = mPendingOutgoingMessages.begin();
				PendingMessageMap::iterator enMap = mPendingOutgoingMessages.end();

				while( itMap != enMap ) {
					MessageQueueSystem *dstSystem = itMap->first;

					dstSystem->mMessageQueueMutex.lock();

					dstSystem->mIncomingMessages[0].appendPOD(
								itMap->second.begin(),
								itMap->second.end() );

					dstSystem->mMessageQueueMutex.unlock();

					itMap->second.clear();

					++itMap;
				}
			}

			template <typename T>
			void receiveMessageImmediately( Mq::MessageId messageId, const T &msg ) {
				mMessageQueueMutex.lock();
				storeMessageToQueue( mIncomingMessages[0], messageId, msg );
				mMessageQueueMutex.unlock();
			}

		protected:
			void processIncomingMessages(void) {
				mMessageQueueMutex.lock();
				mIncomingMessages[0].swap( mIncomingMessages[1] );
				mMessageQueueMutex.unlock();

				MessageArray::const_iterator itor = mIncomingMessages[1].begin();
				MessageArray::const_iterator end  = mIncomingMessages[1].end();

				while( itor != end ) {
					Ogre::uint32 totalSize = *reinterpret_cast<const Ogre::uint32*>( itor );
					Ogre::uint32 messageId = *reinterpret_cast<const Ogre::uint32*>( itor +
																					 sizeof(Ogre::uint32) );

					assert( itor + totalSize <= end && "MessageQueue corrupted!" );
					assert( messageId <= Mq::NUM_MESSAGE_IDS &&
							"MessageQueue corrupted or invalid message!" );

					const void *data = itor + cSizeOfHeader;
					processIncomingMessage( static_cast<Mq::MessageId>( messageId ), data );
					itor += totalSize;
				}

				mIncomingMessages[1].clear();
			}

			virtual void processIncomingMessage( Mq::MessageId messageId, const void *data ) = 0;
		};
	}
}

#endif
