///
/// \file 				SerialFiller.hpp
/// \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
/// \edited             n/a
/// \created			2017-06-10
/// \last-modified		2015-08-23
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
#include "SerialFiller/SerialFillerHelper.hpp"
#include "SerialFiller/Exceptions/CobsDecodingFailed.hpp"
#include "SerialFiller/Exceptions/CrcCheckFailed.hpp"
#include "SerialFiller/Exceptions/LengthOfTopicTooLong.hpp"
#include "SerialFiller/Exceptions/NotEnoughBytes.hpp"

namespace mn {
    namespace SerialFiller {

        ///
        /// Format, pre COBS encoded:
        /// [ 't', 'o', 'p', 'i', 'c', ':', <data 1>, <data 2>, ... , <data n>, <CRC MSB>, <CRC LSB> ]
        /// This is then COBS encoded, which frames the end-of-packet with a unique 0x00 byte,
        /// and escapes all pre-existing 0x00's present in packet.
        class SerialFiller {
        public:

            /// \brief      The minimum number of bytes that a packet can contain to be valid.
            /// \details    Minimum size = 1B for COBS start byte + 1B for topic + 1B for ':' separator, 0B for data + 2B for CRC + 1B for EOF
            ///                          = 6bytes
            static constexpr uint8_t minNumBytesPerPacket = 6;

            void Publish(std::string topic, ByteArray data);

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

            /// \brief      This is called by SerialFiller whenever it has data that is ready
            ///             to be sent out of the serial port.
            std::function<void(ByteQueue)> txDataReady_;

            /// \brief      This event is fired whenever a valid message is received, but
            ///             there are no subscribers listening to it.
            Event<void(std::string topic, ByteArray data)> noSubscribersForTopic_;


        private:

            ByteQueue rxBuffer;

            typedef std::multimap<std::string, std::function<void(ByteArray)>> TopicCallback;
            typedef std::pair<TopicCallback::iterator, TopicCallback::iterator> RangeType;
            TopicCallback topicCallbacks;

        };
    } // namespace SerialFiller
} // namespace mn

#endif // #ifndef SERIAL_FILLER_SERIAL_FILLER_H_