
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

        void HandleRxDataReceived(std::vector<uint8_t> rxData);

        static void DecodePacket(const std::string packet, std::string& topic, std::string& data);

        /// \details    Splits a incoming data stream into packets, based on the end-of-frame character.
        static void PacketizeData(std::vector<uint8_t>& newRxData,
                           std::vector<uint8_t>& existingRxData, std::vector<std::vector<uint8_t>>& packets);

        /// \details    The encoding process cannot fail.
        static void CobsEncoder(
                const std::vector<uint8_t> &rawData,
                std::vector<uint8_t> &encodedData);

        /// \brief      Decode data using "Consistent Overhead Byte Stuffing" (COBS).
        /// \details    Provided encodedData is expected to be a single, valid COBS encoded packet. If not, method
        ///             will return #DecodeStatus::ERROR_ZERO_BYTE_NOT_EXPECTED.
        ///             #decodedData is emptied of any pre-existing data. If the decode fails, decodedData is left empty.
        static DecodeStatus CobsDecoder(const std::vector<uint8_t> &encodedData, std::vector<uint8_t> &decodedData);

        std::function<void(std::vector<uint8_t>)> callback;

    private:

        std::vector<uint8_t> rxBuffer;

        std::multimap<std::string, std::function<void(std::string)>> topicCallbacks;

    };

}