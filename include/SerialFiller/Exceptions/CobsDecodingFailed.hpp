///
/// \file 				CobsDecodingFailed.hpp
/// \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
/// \edited             n/a
/// \created			2017-06-20
/// \last-modified		2018-01-25
/// \brief 				Contains the CobsDecodingFailed exception.
/// \details
///		See README.rst in root dir for more info.

#ifndef MN_SERIAL_FILLER_COBS_DECODING_FAILED_H_
#define MN_SERIAL_FILLER_COBS_DECODING_FAILED_H_

// System includes
#include <cstdint>
#include <iomanip>
#include <sstream>

// Local includes
#include "SerialFiller/Exceptions/SerialFillerException.hpp"

namespace mn {
    namespace SerialFiller {

        class CobsDecodingFailed : public SerialFillerException {
        public:

            CobsDecodingFailed(ByteArray packet) :
                    SerialFillerException("COBS decoding failed for packet.") {
                packet_ = packet;
            };

            ByteArray packet_;
        };
    } // namespace SerialFiller
} // namespace mn

#endif // #ifndef MN_SERIAL_FILLER_COBS_DECODING_FAILED_H_
