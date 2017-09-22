///
/// \file 				SerialFiller.hpp
/// \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
/// \edited             n/a
/// \created			2017-06-10
/// \last-modified		2017-09-21
/// \brief 				Contains the SerialFiller class.
/// \details
///		See README.md in root dir for more info.

#ifndef SERIAL_FILLER_SERIAL_FILLER_H_
#define SERIAL_FILLER_SERIAL_FILLER_H_

// System includes
#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <queue>
#include <vector>
#include <chrono>

// Forward declarations
namespace mn {
    namespace SerialFiller {
        class SerialFiller;
        using ByteArray = std::vector<uint8_t>;
        using ByteQueue = std::deque<uint8_t>;
    }
}

// User includes
#include "SerialFiller/Event.hpp"
#include "SerialFiller/CobsTranscoder.hpp"
#include "SerialFiller/Semaphore.hpp"
#include "SerialFiller/SerialFillerHelper.hpp"
#include "SerialFiller/Exceptions/CobsDecodingFailed.hpp"
#include "SerialFiller/Exceptions/CrcCheckFailed.hpp"
#include "SerialFiller/Exceptions/LengthOfTopicTooLong.hpp"
#include "SerialFiller/Exceptions/NotEnoughBytes.hpp"

namespace mn {
    namespace SerialFiller {

        using EventType = std::pair<std::condition_variable, bool>;

        ///
        /// Format, pre COBS encoded:
        /// [ <packet type>, <packet type dependent data...> ]
        /// Packet Type: 1 = publish, 2 = ack
        /// Publish packet structure:
        /// [ 0x01, <packet ID MSB>, <packet ID LSB>, <length of topic id>, <topic ID 1>, ..., <topic ID n>, <data 1>, ... , <data n>, <CRC MSB>, <CRC LSB> ]
        /// Ack Packet Structure:
        /// [ 0x02, <packet MSB>, <packet ID LSB>, <CRC MSB>, <CRC LSB> ]
        /// This is then COBS encoded, which frames the end-of-packet with a unique 0x00 byte,
        /// and escapes all pre-existing 0x00's present in packet.
        class SerialFiller {
        public:

            /// \brief      Enumerates the available SerialFiller packet types.
            enum class PacketType : uint8_t {
                PUBLISH = 0x01,
                ACK = 0x02
            };

            SerialFiller();

            /// \brief      Publishes data on a topic, and then immediately returns. Does not block (see PublishWait()).
            /// \throws     std::runtime_error
            void Publish(std::string topic, ByteArray data);

            /// \brief      Publishes data on a topic, and then blocks the calling thread until either an acknowledge
            ///             is received, or a timeout occurs.
            /// \returns    True if an acknowledge was received before the timeout occurred, otherwise false.
            /// \warning    Ack must enabled via SetAckEnabled() before calling this method.
            bool PublishWait(const std::string &topic, const ByteArray &data, std::chrono::milliseconds timeout);

            void Subscribe(std::string topic, std::function<void(ByteArray)> callback);

            /// \brief      Pass in received RX data to SerialFiller.
            /// \details    SerialFiller will add this data to it's internal RX data buffer, and then
            ///             attempt to find and extract valid packets. If SerialFiller finds valid packets,
            ///             it will then call all callbacks associated with that topic.
            /// \throws     NotEnoughBytes
            /// \throws     CobsDecodingFailed
            /// \throws     CrcCheckFailed
            /// \throws     NoTopicDataSeparator
            void GiveRxData(ByteQueue &rxData);

            /// \brief      Use to enable/disable the sending to ACK (acknowledgement) packets when a publish packet
            ///             is received. Must be enabled if PublishWait() is going to be used.
            void SetAckEnabled(bool value);

            /// \brief      Use to enable/disable thread safety. Enabling thread safety makes all SerialFiller API
            ///             methods take out a lock on enter, and release on exit. PublishWait() releases lock when it blocks (so
            ///             PublishWait() can be called multiple times from different threads).
            void SetThreadSafetyEnabled(bool value);

            /// \brief      Call to find out how many threads are currently waiting on ACKs for this node.
            uint32_t NumThreadsWaiting();

            /// \brief      This is called by SerialFiller whenever it has data that is ready
            ///             to be sent out of the serial port.
            std::function<void(ByteQueue)> txDataReady_;

            /// \brief      This event is fired whenever a valid message is received, but
            ///             there are no subscribers listening to it.
            Event<void(std::string topic, ByteArray data)> noSubscribersForTopic_;

            std::mutex classMutex_;


        private:

            ByteQueue rxBuffer;

            typedef std::multimap<std::string, std::function<void(ByteArray)>> TopicCallback;
            typedef std::pair<TopicCallback::iterator, TopicCallback::iterator> RangeType;
            TopicCallback topicCallbacks;

            /// \brief      Stores what the next sent packet ID should be.
            uint16_t nextPacketId_;

            /// \brief      Is true if acknowledge (ACK) functionality is enabled.
            bool ackEnabled_;

            bool threadSafetyEnabled_;

            std::map<uint16_t, std::shared_ptr<EventType>> ackEvents_;

            void PublishInternal(const std::string& topic, const ByteArray& data);

            /// \brief      Sends an ACK packet.
            void SendAck(uint16_t packetId);

        };
    } // namespace SerialFiller
} // namespace mn

#endif // #ifndef SERIAL_FILLER_SERIAL_FILLER_H_