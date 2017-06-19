///
/// \file 				Fp32f.cpp
/// \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
/// \edited             n/a
/// \created			2017-06-10
/// \last-modified		2015-06-18
/// \brief 				Contains the SerialFiller class.
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


    class SerialFillerHelper {
    public:

        static void SplitPacket(const ByteArray &packet, std::string &topic, ByteArray &data);

        /// \details    Splits a incoming data stream into packets, based on the end-of-frame character.
        static void PacketizeData(
                const ByteArray& newRxData,
                ByteArray& existingRxData,
                std::vector<ByteArray>& packets);


        static void AddCrc(ByteArray& packet);

        /// \param  packet  Packet must be COBS decoded before passing into here. Expects
        ///                 last two bytes to be the CRC value of all the bytes proceeding it.
        static bool VerifyCrc(const ByteArray &packet);


    private:

    };

}

#endif // #ifndef SERIAL_FILLER_SERIAL_FILLER_H_