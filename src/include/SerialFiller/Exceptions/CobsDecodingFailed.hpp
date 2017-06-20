///
/// \file 				CobsDecodingFailed.hpp
/// \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
/// \edited             n/a
/// \created			2017-06-20
/// \last-modified		2017-06-20
/// \brief 				Contains the CobsDecodingFailed exception.
/// \details
///		See README.rst in root dir for more info.

#ifndef MN_SERIAL_FILLER_COBS_DECODING_FAILED_H_
#define MN_SERIAL_FILLER_COBS_DECODING_FAILED_H_

// System includes
#include <cstdint>
#include <iomanip>
#include <sstream>

// Forward declaration
namespace mn {
    namespace SerialFiller {
        class CobsDecodingFailed;
    }
}

// User includes

namespace mn {
    namespace SerialFiller {

        class CobsDecodingFailed : public std::runtime_error {
        public:

            CobsDecodingFailed(ByteArray packet) :
                    runtime_error("COBS decoding failed for packet.") {
                packet_ = packet;
            };

            ByteArray packet_;
        };
    } // namespace SerialFiller
} // namespace mn

#endif // #ifndef MN_SERIAL_FILLER_COBS_DECODING_FAILED_H_
