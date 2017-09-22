///
/// \file 				SerialFillerHelper.hpp
/// \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
/// \edited             n/a
/// \created			2017-06-10
/// \last-modified		2017-09-21
/// \brief 				Contains the SerialFillerHelper class.
/// \details
///		See README.rst in root dir for more info.

#ifndef SERIAL_FILLER_SERIAL_FILLER_INTERNAL_H_
#define SERIAL_FILLER_SERIAL_FILLER_INTERNAL_H_



// System includes
#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <vector>


#include "SerialFiller/CobsTranscoder.hpp"



namespace mn {
    namespace SerialFiller {


        class SerialFillerHelper {
        public:

            static void SplitPacket(const ByteArray &packet, uint32_t startAt, std::string &topic, ByteArray &data);

            /// \details    Moves new RX data into the RX buffer, while looking for the
            ///             end-of-frame character. If EOF is found, packet is populated
            ///             and this method returns.
            static void MoveRxDataInBuffer(
                    ByteQueue &newRxData,
                    ByteQueue &rxDataBuffer,
                    ByteArray &packet);


            static void AddCrc(ByteArray &packet);

            /// \param  packet  Packet must be COBS decoded before passing into here. Expects
            ///                 last two bytes to be the CRC value of all the bytes proceeding it.
            static bool VerifyCrc(const ByteArray &packet);


        private:

        };
    }
}

#endif // #ifndef SERIAL_FILLER_SERIAL_FILLER_H_