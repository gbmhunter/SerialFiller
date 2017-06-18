
#include "SerialFiller/SerialFiller.hpp"
#include "SerialFiller/Crc16CCitt1021.hpp"

namespace mn {

    void SerialFiller::Publish(std::string topic, std::string message) {

        std::string packet = "";
        packet += topic;
        packet += ":";
        packet += message;

        std::cout << "packet = " << packet << std::endl;

        // Convert to raw packet
        std::string rawData;
        for (int i = 0; i < packet.size(); i++) {
            rawData.push_back((uint8_t) packet[i]);
        }

        std::string encodedData;
        CobsTranscoder::Encode(rawData, encodedData);

        // Emit TX send event
        txDataReady_(encodedData);
    };

    void SerialFiller::Subscribe(std::string topic, std::function<void(std::string)> callback) {

        // Save subscription
        topicCallbacks.insert({topic, callback});

    }


    void SerialFiller::PacketizeData(
            std::string &newRxData,
            std::string &existingRxData,
            std::vector<std::string> &packets) {

        // Extract all bytes from istream
        for (auto it = newRxData.begin(); it != newRxData.end(); it++) {

            char byteOfData = *it;
            existingRxData.push_back((uint8_t) byteOfData);

            // Look for 0x00 byte in data
            if (byteOfData == 0x00) {
                // Found end-of-packet!

                // Move everything from the start to byteOfData from rxData
                // into a new packet
                std::string packet;
                for (auto it = existingRxData.begin(); it != existingRxData.end(); it++) {
                    packet.push_back(*it);
                }
                existingRxData.clear();
                packets.push_back(packet);
            }
        }
    }

    bool SerialFiller::VerifyCrc(const std::string& packet) {

        // Create a string of the packet without the CRC
        std::string packetWithoutCrc = packet.substr(0, packet.size() - 2);

        // Extract the sent CRC value
        std::string sentCrcString = packet.substr(packet.size() - 2, packet.size());
        uint16_t sentCrcVal = ((uint16_t)(uint8_t)sentCrcString[0] << 8) | ((uint16_t)(uint8_t)sentCrcString[1]);

        // Calculate CRC
        uint16_t calcCrcVal = Crc16CCitt1021::Calc(packetWithoutCrc);

        return sentCrcVal == calcCrcVal;

    }

    void SerialFiller::HandleRxDataReceived(std::string rxData) {

        std::vector<std::string> packets;
        SerialFiller::PacketizeData(rxData, rxBuffer, packets);

        for(auto it = packets.begin(); it != packets.end(); it++) {
            std::string topic;
            std::string data;

            // FOR EACH PACKET:
            // 1. Remove COBS encoding
            std::string decodedData;
            CobsTranscoder::Decode(*it, decodedData);

            // 2. Verify CRC


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

    void SerialFiller::SplitPacket(const std::string &packet, std::string &topic, std::string &data) {

        // Find ":", this indicates the end of the topic name and the
        // start of the data
        std::size_t colonPos = packet.find(":");
        if (colonPos == std::string::npos) {
            throw std::runtime_error("Packet ill-formed. ':' could not be found in packet data.");
        } else {
            topic = packet.substr(0, colonPos);
            std::cout << "topic = " << topic << std::endl;

            data = packet.substr(colonPos + 1, packet.size());
            std::cout << "data = " << data << std::endl;
        }

    }

}