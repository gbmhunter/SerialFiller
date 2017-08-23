///
/// \file 				Crc16Ccitt1021.hpp
/// \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
/// \edited             n/a
/// \created			2017-06-10
/// \last-modified		2017-06-20
/// \brief 				Contains the Crc16Ccitt1021 class.
/// \details
///		See README.rst in root dir for more info.

#include "SerialFiller/SerialFiller.hpp"

namespace mn {
    namespace SerialFiller {

        class Crc16Ccitt1021 {

        public:
            static uint16_t Calc(ByteArray data);

        };
    } // namespace SerialFiller
} // namespace mn