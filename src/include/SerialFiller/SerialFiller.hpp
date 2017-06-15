
#ifndef SERIAL_FILLER_SERIAL_FILLER_H_
#define SERIAL_FILLER_SERIAL_FILLER_H_

#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <vector>

#include "SerialFiller/CobsTranscoder.hpp"



namespace mn {


    ///
    /// Format, pre COBS encoded:
    /// [ 't', 'o', 'p', 'i', 'c'., 0x00, <data 1>, <data 2>, ... , <data n> ]
    /// This is then COBS encoded, which frames the end-of-packet with a unique 0x00 byte,
    /// and escapes all pre-existing 0x00's present in packet.
    class SerialFiller {
    public:

        void Publish(std::string topic, std::string message);

        void Subscribe(std::string topic, std::function<void(std::string)> callback);

        void HandleRxDataReceived(std::string rxData);

        static void SplitPacket(const std::string &packet, std::string &topic, std::string &data);

        /// \details    Splits a incoming data stream into packets, based on the end-of-frame character.
        static void PacketizeData(std::string& newRxData,
                           std::string& existingRxData, std::vector<std::string>& packets);

        std::function<void(std::string)> txDataReady_;

    private:

        std::string rxBuffer;

        typedef std::multimap<std::string, std::function<void(std::string)>> TopicCallback;
        typedef std::pair<TopicCallback::iterator, TopicCallback::iterator> RangeType;
        TopicCallback topicCallbacks;

    };

}

#endif // #ifndef SERIAL_FILLER_SERIAL_FILLER_H_