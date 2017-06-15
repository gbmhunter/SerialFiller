
#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <vector>

namespace MN {


    ///
    /// Format, pre COBS encoded:
    /// [ 't', 'o', 'p', 'i', 'c'., 0x00, <data 1>, <data 2>, ... , <data n> ]
    /// This is then COBS encoded, which frames the end-of-packet with a unique 0x00 byte,
    /// and escapes all pre-existing 0x00's present in packet.
    class SerialFiller {
    public:

        enum class DecodeStatus {
            SUCCESS,
            ERROR_ZERO_BYTE_NOT_EXPECTED
        };

        void Publish(std::string topic, std::string message);

        void Subscribe(std::string topic, std::function<void(std::string)> callback);

        void HandleRxDataReceived(std::string rxData);

        static void SplitPacket(const std::string &packet, std::string &topic, std::string &data);

        /// \details    Splits a incoming data stream into packets, based on the end-of-frame character.
        static void PacketizeData(std::string& newRxData,
                           std::string& existingRxData, std::vector<std::string>& packets);

        /// \details    The encoding process cannot fail.
        static void CobsEncoder(
                const std::string &rawData,
                std::string &encodedData);

        /// \brief      Decode data using "Consistent Overhead Byte Stuffing" (COBS).
        /// \details    Provided encodedData is expected to be a single, valid COBS encoded packet. If not, method
        ///             will return #DecodeStatus::ERROR_ZERO_BYTE_NOT_EXPECTED.
        ///             #decodedData is emptied of any pre-existing data. If the decode fails, decodedData is left empty.
        static DecodeStatus CobsDecoder(const std::string &encodedData, std::string &decodedData);

        std::function<void(std::string)> txDataReady_;

    private:

        std::string rxBuffer;

        typedef std::multimap<std::string, std::function<void(std::string)>> TopicCallback;
        typedef std::pair<TopicCallback::iterator, TopicCallback::iterator> RangeType;
        TopicCallback topicCallbacks;

    };

}