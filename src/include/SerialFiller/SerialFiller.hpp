///
/// \file 				Fp32f.cpp
/// \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
/// \edited             n/a
/// \created			2017-06-10
/// \last-modified		2015-06-18
/// \brief 				Contains the SerialFiller class.
/// \details
///		See README.rst in root dir for more info.

#ifndef SERIAL_FILLER_SERIAL_FILLER_H_
#define SERIAL_FILLER_SERIAL_FILLER_H_



// System includes
#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <vector>

// Forward declarations
namespace mn {
    using ByteArray = std::vector<uint8_t>;
}

// User includes
#include "SerialFiller/CobsTranscoder.hpp"
#include "SerialFiller/SerialFillerHelper.hpp"
#include "SerialFiller/Exceptions/CrcCheckFailed.hpp"

namespace mn {

    ///
    /// Format, pre COBS encoded:
    /// [ 't', 'o', 'p', 'i', 'c', ':', <data 1>, <data 2>, ... , <data n>, <CRC MSB>, <CRC LSB> ]
    /// This is then COBS encoded, which frames the end-of-packet with a unique 0x00 byte,
    /// and escapes all pre-existing 0x00's present in packet.
    class SerialFiller {
    public:

        void Publish(std::string topic, ByteArray data);

        void Subscribe(std::string topic, std::function<void(ByteArray)> callback);

        /// \brief      Pass in received RX data to SerialFiller.
        /// \details    SerialFiller will add this data to it's internal RX data buffer, and then
        ///             attempt to find and extract valid packets. If SerialFiller finds valid packets,
        ///             it will then call all callbacks associated with that topic.
        /// \throws     CrcCheckFailed
        void GiveRxData(ByteArray rxData);

        std::function<void(ByteArray)> txDataReady_;

    private:

        ByteArray rxBuffer;

        typedef std::multimap<std::string, std::function<void(ByteArray)>> TopicCallback;
        typedef std::pair<TopicCallback::iterator, TopicCallback::iterator> RangeType;
        TopicCallback topicCallbacks;

    };

}

#endif // #ifndef SERIAL_FILLER_SERIAL_FILLER_H_