///
/// \file 				NotEnoughBytes.hpp
/// \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
/// \edited             n/a
/// \created			2017-06-20
/// \last-modified		2018-01-25
/// \brief 				Contains the NotEnoughBytes exception.
/// \details
///		See README.rst in root dir for more info.

#ifndef MN_SERIAL_FILLER_NOT_ENOUGH_BYTES_H_
#define MN_SERIAL_FILLER_NOT_ENOUGH_BYTES_H_

// System includes
#include <cstdint>
#include <iomanip>
#include <sstream>

// Forward declaration
namespace mn {
    namespace SerialFiller {
        class NotEnoughBytes;
    }
}

// User includes
#include "SerialFiller/Constants.hpp"
#include "SerialFiller/Exceptions/SerialFillerException.hpp"

namespace mn {
    namespace SerialFiller {

        class NotEnoughBytes : public SerialFillerException {
        public:

            NotEnoughBytes(ByteArray packet) :
                    SerialFillerException(
                            "Not enough bytes in packet. Num. of bytes in packet = " + std::to_string(packet.size()) +
                            ". Min num. of byes = " + std::to_string(Constants::minNumBytesPerPacket)) {
                packet_ = packet;
            };

            ByteArray packet_;
        };
    } // namespace SerialFiller
} // namespace mn

#endif // #ifndef MN_SERIAL_FILLER_NOT_ENOUGH_BYTES_H_
