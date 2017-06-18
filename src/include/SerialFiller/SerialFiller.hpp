
#ifndef SERIAL_FILLER_SERIAL_FILLER_H_
#define SERIAL_FILLER_SERIAL_FILLER_H_



// System includes
#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <vector>

// Forward declarations
namespace mn {
    using ByteArray = std::vector<uint8_t>;
}

#include "SerialFiller/CobsTranscoder.hpp"



namespace mn {



    ///
    /// Format, pre COBS encoded:
    /// [ 't', 'o', 'p', 'i', 'c'., 0x00, <data 1>, <data 2>, ... , <data n> ]
    /// This is then COBS encoded, which frames the end-of-packet with a unique 0x00 byte,
    /// and escapes all pre-existing 0x00's present in packet.
    class SerialFiller {
    public:

        void Publish(std::string topic, ByteArray message);

        void Subscribe(std::string topic, std::function<void(ByteArray)> callback);

        void HandleRxDataReceived(ByteArray rxData);

        static void SplitPacket(const ByteArray &packet, std::string &topic, ByteArray &data);

        /// \details    Splits a incoming data stream into packets, based on the end-of-frame character.
        static void PacketizeData(ByteArray& newRxData,
                           ByteArray& existingRxData, std::vector<ByteArray>& packets);


        static void AddCrc(ByteArray& packet);

        /// \param  packet  Packet must be COBS decoded before passing into here. Expects
        ///                 last two bytes to be the CRC value of all the bytes proceeding it.
        static bool VerifyCrc(const ByteArray& packet);

        std::function<void(ByteArray)> txDataReady_;

    private:

        ByteArray rxBuffer;

        typedef std::multimap<std::string, std::function<void(ByteArray)>> TopicCallback;
        typedef std::pair<TopicCallback::iterator, TopicCallback::iterator> RangeType;
        TopicCallback topicCallbacks;

    };

}

#endif // #ifndef SERIAL_FILLER_SERIAL_FILLER_H_