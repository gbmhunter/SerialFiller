
#include <cstdint>
#include <iostream>
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

        void Publish(std::string topic, std::string message) {

            std::string packet = "";
            packet += topic;

            std::cout << "packet = " << packet << std::endl;
        };

        void Subscribe(std::string topic);


        /// \details    The encoding process cannot fail.
        static std::vector<uint8_t> CobsEncoder(
                const std::vector<uint8_t> &rawData,
                std::vector<uint8_t> &encodedData);

        /// \brief      Decode data using "Consistent Overhead Byte Stuffing" (COBS).
        /// \details    Provided encodedData is expected to be a single, valid COBS encoded packet. If not, method
        ///             will return #DecodeStatus::ERROR_ZERO_BYTE_NOT_EXPECTED.
        ///             #decodedData is emptied of any pre-existing data. If the decode fails, decodedData is left empty.
        static DecodeStatus CobsDecoder(const std::vector<uint8_t> &encodedData, std::vector<uint8_t> &decodedData);

    private:


    };

}