///
/// \file 				CobsTranscoder.hpp
/// \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
/// \edited             n/a
/// \created			2017-06-20
/// \last-modified		2018-01-25
/// \brief 				Contains the CobsTranscoder class.
/// \details
///		See README.rst in root dir for more info.

#ifndef MN_SERIAL_FILLER_COBS_TRANSCODER_H_
#define MN_SERIAL_FILLER_COBS_TRANSCODER_H_

// System includes
// none

// Local includes
#include "SerialFiller/Definitions.hpp"

namespace mn {
    namespace SerialFiller {

        class CobsTranscoder {

        public:

            enum class DecodeStatus {
                SUCCESS,
                ERROR_ZERO_BYTE_NOT_EXPECTED
            };

            /// \details    The encoding process cannot fail.
            static void Encode(
                    const ByteArray &rawData,
                    ByteArray &encodedData);

            /// \brief      Decode data using "Consistent Overhead Byte Stuffing" (COBS).
            /// \details    Provided encodedData is expected to be a single, valid COBS encoded packet. If not, method
            ///             will return #DecodeStatus::ERROR_ZERO_BYTE_NOT_EXPECTED.
            ///             #decodedData is emptied of any pre-existing data. If the decode fails, decodedData is left empty.
            static void Decode(const ByteArray &encodedData, ByteArray &decodedData);

        };
    } // namespace SerialFiller
} // namespace mn

#endif // #ifndef MN_SERIAL_FILLER_COBS_TRANSCODER_H_