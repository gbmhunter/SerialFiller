
// User includes
#include "SerialFiller/SerialFiller.hpp"
#include "SerialFiller/Crc16CCitt1021.hpp"

namespace mn {

    void SerialFiller::Publish(std::string topic, ByteArray data) {

        ByteArray packet;
        std::copy(topic.begin(), topic.end(), std::back_inserter(packet));

        packet.push_back(':');
        std::copy(data.begin(), data.end(), std::back_inserter(packet));

        // Add CRC
        AddCrc(packet);

        ByteArray encodedData;
        CobsTranscoder::Encode(packet, encodedData);

        // Emit TX send event
        txDataReady_(encodedData);
    };

    void SerialFiller::Subscribe(std::string topic, std::function<void(ByteArray)> callback) {

        // Save subscription
        topicCallbacks.insert({topic, callback});

    }


    void SerialFiller::PacketizeData(
            ByteArray &newRxData,
            ByteArray &existingRxData,
            std::vector<ByteArray> &packets) {

        // Extract all bytes from istream
        for (auto it = newRxData.begin(); it != newRxData.end(); it++) {

            char byteOfData = *it;
            existingRxData.push_back((uint8_t) byteOfData);

            // Look for 0x00 byte in data
            if (byteOfData == 0x00) {
                // Found end-of-packet!

                // Move everything from the start to byteOfData from rxData
                // into a new packet
                ByteArray packet;
                for (auto it = existingRxData.begin(); it != existingRxData.end(); it++) {
                    packet.push_back(*it);
                }
                existingRxData.clear();
                packets.push_back(packet);
            }
        }
    }

    void SerialFiller::AddCrc(ByteArray &packet) {

        uint16_t crcVal = Crc16CCitt1021::Calc(packet);

        // Add CRC value to end of packet, MSB of CRC
        // comes first
        packet.push_back((uint8_t)(crcVal >> 8));
        packet.push_back((uint8_t)(crcVal));
    }

    bool SerialFiller::VerifyCrc(const ByteArray &packet) {

        // Create a string of the packet without the CRC
        ByteArray packetWithoutCrc(packet.begin(), packet.end() - 2);

        // Extract the sent CRC value
        ByteArray sentCrcString(packet.end() - 2, packet.end());
        uint16_t sentCrcVal = ((uint16_t)(uint8_t)sentCrcString[0] << 8) | ((uint16_t)(uint8_t)sentCrcString[1]);

        // Calculate CRC
        uint16_t calcCrcVal = Crc16CCitt1021::Calc(packetWithoutCrc);

        if(sentCrcVal != calcCrcVal) {
            throw std::runtime_error("CRC values did not match!");
        }

        return sentCrcVal == calcCrcVal;

    }

    void SerialFiller::HandleRxDataReceived(ByteArray rxData) {

        std::vector<ByteArray> packets;
        SerialFiller::PacketizeData(rxData, rxBuffer, packets);

        for(auto it = packets.begin(); it != packets.end(); it++) {
            std::string topic;
            ByteArray data;

            // FOR EACH PACKET:
            // 1. Remove COBS encoding
            ByteArray decodedData;
            CobsTranscoder::Decode(*it, decodedData);

            // 2. Verify CRC
            // This function will throw if CRC does not match
            VerifyCrc(decodedData);

            // Then split packet into topic and data
            SerialFiller::SplitPacket(decodedData, topic, data);

            // Call every callback associated with this topic
//            auto keyRange = topicCallbacks.equal_range(topic);
//            std::pair<TopicCallback::iterator, TopicCallback::iterator> range;
            RangeType range = topicCallbacks.equal_range(topic);
            for(TopicCallback::iterator rangeIt = range.first;  rangeIt != range.second;  ++rangeIt) {
//                std::cout << "    value = " << keyIt->second << std::endl;
                rangeIt->second(data);
            }

        }

    }

    void SerialFiller::SplitPacket(const ByteArray &packet, std::string &topic, ByteArray &data) {

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