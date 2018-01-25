///
/// \file 				LengthOfTopicTooLong.hpp
/// \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
/// \edited             n/a
/// \created			2017-07-07
/// \last-modified		2018-01-25
/// \brief 				Contains the LengthOfTopicTooLong exception.
/// \details
///		See README.rst in root dir for more info.

#ifndef MN_SERIAL_FILLER_LENGTH_OF_TOPIC_TOO_LONG_H_
#define MN_SERIAL_FILLER_LENGTH_OF_TOPIC_TOO_LONG_H_

// System includes


// Forward declaration
namespace mn {
    namespace SerialFiller {
        class NoTopicDataSeparator;
    }
}

// Local includes
#include "SerialFiller/Exceptions/SerialFillerException.hpp"

namespace mn {
    namespace SerialFiller {

        class LengthOfTopicTooLong : public SerialFillerException {
        public:

            LengthOfTopicTooLong(ByteArray packet, uint32_t statedLengthInBytes, ByteArray::size_type availableBytes) :
                    SerialFillerException("Stated length of topic was too long. Stated length = " +
                                          std::to_string(statedLengthInBytes) +
                    ", available num. bytes for topic = " + std::to_string(availableBytes) + ".") {
                packet_ = packet;
                statedLengthInBytes_ = statedLengthInBytes;
                availableBytes_ = availableBytes;
            };

            ByteArray packet_;
            uint32_t statedLengthInBytes_;
            ByteArray::size_type availableBytes_;
        };
    } // namespace SerialFiller
} // namespace mn

#endif // #ifndef MN_SERIAL_FILLER_LENGTH_OF_TOPIC_TOO_LONG_H_
