///
/// \file 				CrcCheckFailed.hpp
/// \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
/// \edited             n/a
/// \created			2017-06-20
/// \last-modified		2018-01-25
/// \brief 				Contains the CrcCheckFailed class.
/// \details
///		See README.rst in root dir for more info.

#ifndef MN_SERIAL_FILLER_CRC_CHECK_FAILED_H_
#define MN_SERIAL_FILLER_CRC_CHECK_FAILED_H_

// System includes
#include <cstdint>
#include <iomanip>
#include <sstream>

// Local includes
#include "SerialFiller/Exceptions/SerialFillerException.hpp"

namespace mn {
    namespace SerialFiller {

        class CrcCheckFailed : public SerialFillerException {
        public:

            template<typename T>
            std::string IntToHex(T value) {
                std::stringstream stream;
                stream << "0x"
                       << std::setfill('0') << std::setw(sizeof(T) * 2)
                       << std::hex << value;
                return stream.str();
            }

            CrcCheckFailed(uint16_t calculatedCrcVal, uint16_t sentCrcVal) :
                    SerialFillerException("CRC check failed. Calculated CRC value = " + IntToHex(calculatedCrcVal) +
                                  ", sent CRC value = " + IntToHex(sentCrcVal) + ".") {
                calculatedCrcVal_ = calculatedCrcVal;
                sentCrcVal_ = sentCrcVal;
            };

            uint16_t calculatedCrcVal_;
            uint16_t sentCrcVal_;
        };
    } // namespace SerialFiller
} // namespace mn

#endif // #ifndef MN_SERIAL_FILLER_CRC_CHECK_FAILED_H_
