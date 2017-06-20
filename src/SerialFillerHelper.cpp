
// User includes
#include "SerialFiller/SerialFiller.hpp"
#include "SerialFiller/Crc16CCitt1021.hpp"

namespace mn {


    void SerialFillerHelper::PacketizeData(
            const ByteArray &newRxData,
            ByteArray &existingRxData,
            std::vector<ByteArray> &packets) {

        // Clear any existing packets
        packets.clear();

        // Extract all bytes from istream
        for (auto it = newRxData.begin(); it != newRxData.end(); ++it) {

            char byteOfData = *it;
//            it = newRxData.erase(it);
            existingRxData.push_back((uint8_t) byteOfData);

            // Look for 0x00 byte in data
            if (byteOfData == 0x00) {
                // Found end-of-packet!

                // Move everything from the start to byteOfData from rxData
                // into a new packet
                ByteArray packet;
                packet.swap(existingRxData);
//                for (auto it = existingRxData.begin(); it != existingRxData.end(); it++) {
//                    packet.push_back(*it);
//                }

                // Everything from the start of the existing data to the
                // 0x00 byte has been copied into the packet, we can now
                // clear it.
                existingRxData.clear();
                packets.push_back(packet);
            }
        }
    }

    void SerialFillerHelper::AddCrc(ByteArray &packet) {

        uint16_t crcVal = Crc16CCitt1021::Calc(packet);

        // Add CRC value to end of packet, MSB of CRC
        // comes first
        packet.push_back((uint8_t)(crcVal >> 8));
        packet.push_back((uint8_t)(crcVal));
    }

    bool SerialFillerHelper::VerifyCrc(const ByteArray &packet) {

        // Create a string of the packet without the CRC
        ByteArray packetWithoutCrc(packet.begin(), packet.end() - 2);

        // Extract the sent CRC value
        ByteArray sentCrcString(packet.end() - 2, packet.end());
        uint16_t sentCrcVal = ((uint16_t)(uint8_t)sentCrcString[0] << 8) | ((uint16_t)(uint8_t)sentCrcString[1]);

        // Calculate CRC
        uint16_t calcCrcVal = Crc16CCitt1021::Calc(packetWithoutCrc);

        if(sentCrcVal != calcCrcVal) {
            throw CrcCheckFailed(calcCrcVal, sentCrcVal);
        }

        return sentCrcVal == calcCrcVal;

    }


    void SerialFillerHelper::SplitPacket(const ByteArray &packet, std::string &topic, ByteArray &data) {

        // Find ":", this indicates the end of the topic name and the
        // start of the data
        bool foundTopicToDataSeparator = false;
        for(auto it = packet.begin(); it != packet.end(); ++it) {
            // Look for the first ":"
            if(*it == ':') {
                topic = std::string(packet.begin(), it);

                // Data starts after ':' and stops 2 bytes before the
                // end (the 2 CRC bytes)
                data = ByteArray(it + 1, packet.end() - 2);
                foundTopicToDataSeparator = true;
            }
        }

        if (!foundTopicToDataSeparator) {
            throw std::runtime_error("Packet ill-formed. ':' could not be found in packet data.");
        }
    }
}