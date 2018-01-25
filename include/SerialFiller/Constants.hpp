///
/// \file 				Constants.hpp
/// \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
/// \edited             n/a
/// \created			2017-06-20
/// \last-modified		2018-01-25
/// \brief 				Contains the Constants class.
/// \details
///		See README.md in root dir for more info.

#ifndef MN_SERIAL_FILLER_CONSTANTS_H_
#define MN_SERIAL_FILLER_CONSTANTS_H_

// System includes
#include <cstdint>

// Local includes
// none

namespace mn {
    namespace SerialFiller {

        class Constants {
        public:

            /// \brief      The minimum number of bytes that a packet can contain to be valid.
            /// \details    Minimum size = 1B for COBS start byte + 1B for topic + 1B for ':' separator, 0B for data + 2B for CRC + 1B for EOF
            ///                          = 6bytes
            static constexpr uint8_t minNumBytesPerPacket = 6;

        };
    } // namespace SerialFiller
} // namespace mn

#endif // #ifndef MN_SERIAL_FILLER_CONSTANTS_H_