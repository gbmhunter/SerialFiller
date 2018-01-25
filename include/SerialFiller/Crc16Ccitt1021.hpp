///
/// \file 				Crc16Ccitt1021.hpp
/// \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
/// \edited             n/a
/// \created			2017-06-10
/// \last-modified		2018-01-25
/// \brief 				Contains the Crc16Ccitt1021 class.
/// \details
///		See README.rst in root dir for more info.

#ifndef MN_SERIAL_FILLER_CRC16_CCITT_1021_H_
#define MN_SERIAL_FILLER_CRC16_CCITT_1021_H_

// Local includes
#include "SerialFiller/SerialFiller.hpp"

namespace mn {
    namespace SerialFiller {

        class Crc16Ccitt1021 {

        public:
            static uint16_t Calc(ByteArray data);

        };
    } // namespace SerialFiller
} // namespace mn

#endif // #ifndef MN_SERIAL_FILLER_CRC16_CCITT_1021_H_