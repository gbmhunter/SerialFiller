///
/// \file 				SerialFillerHelper.cpp
/// \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
/// \edited             n/a
/// \created			2017-06-10
/// \last-modified		2017-06-20
/// \brief 				Contains the SerialFillerHelper class.
/// \details
///		See README.rst in root dir for more info.

// User includes
#include "SerialFiller/SerialFiller.hpp"
#include "SerialFiller/Crc16Ccitt1021.hpp"
#include "SerialFiller/Exceptions/LengthOfTopicTooLong.hpp"

namespace mn {
    namespace SerialFiller {

        void SerialFillerHelper::MoveRxDataInBuffer(
                ByteQueue &newRxData,
                ByteQueue &rxDataBuffer,
                ByteArray &packet) {

//            std::cout << "Move RX data called." << std::endl;

            // Clear any existing data from packet
            packet.clear();

            // Pop bytes from front of queue
            while (!newRxData.empty()) {


                uint8_t byteOfData = newRxData.front();
                newRxData.pop_front();

                rxDataBuffer.push_back(byteOfData);

                // Look for 0x00 byte in data
                if (byteOfData == 0x00) {
                    // Found end-of-packet!
                    std::cout << "Found EOP." << std::endl;

                    // Move everything from the start to byteOfData from rxData
                    // into a new packet
                    for (auto it = rxDataBuffer.begin(); it != rxDataBuffer.end(); ++it) {
                        packet.push_back(*it);
                    }

                    rxDataBuffer.clear();

//                    std::cout << "Move RX data returning." << std::endl;
                    return;
                }
            }
//            std::cout << "Move RX data returning." << std::endl;
        }

        void SerialFillerHelper::AddCrc(ByteArray &packet) {

            uint16_t crcVal = Crc16Ccitt1021::Calc(packet);

            // Add CRC value to end of packet, MSB of CRC
            // comes first
            packet.push_back((uint8_t) (crcVal >> 8));
            packet.push_back((uint8_t) (crcVal));
        }

        bool SerialFillerHelper::VerifyCrc(const ByteArray &packet) {

            if(packet.size() < 3)
                throw std::runtime_error("Cannot verify CRC with less than 3 bytes in packet.");

            // Create a string of the packet without the CRC
            ByteArray packetWithoutCrc(packet.begin(), packet.end() - 2);

            // Extract the sent CRC value
            ByteArray sentCrcString(packet.end() - 2, packet.end());
            uint16_t sentCrcVal =
                    ((uint16_t) (uint8_t) sentCrcString[0] << 8) | ((uint16_t) (uint8_t) sentCrcString[1]);

            // Calculate CRC
            uint16_t calcCrcVal = Crc16Ccitt1021::Calc(packetWithoutCrc);

            if (sentCrcVal != calcCrcVal) {
                throw CrcCheckFailed(calcCrcVal, sentCrcVal);
            }

            return sentCrcVal == calcCrcVal;

        }


        void SerialFillerHelper::SplitPacket(const ByteArray &packet, uint32_t startAt, std::string &topic, ByteArray &data) {

            // Get length of topic
            int lengthOfTopic = packet.at(startAt);

            int32_t availableBytes = packet.size() - 2 - startAt;
            // Verify that length of topic is not longer than total length of packet - 2 bytes for CRC - start position
            if(lengthOfTopic > availableBytes) {
                throw LengthOfTopicTooLong(packet, lengthOfTopic, availableBytes);
            }

            topic = std::string(packet.begin() + 1 + startAt, packet.begin() + 1 + + startAt + lengthOfTopic);
            data = ByteArray(packet.begin() + 1 + startAt + lengthOfTopic, packet.end() - 2);

        }
    } // namespace SerialFiller
} // namespace mn