
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
        SerialFillerHelper::AddCrc(packet);

        ByteArray encodedData;
        CobsTranscoder::Encode(packet, encodedData);

        // Emit TX send event
        txDataReady_(encodedData);
    };

    void SerialFiller::Subscribe(std::string topic, std::function<void(ByteArray)> callback) {

        // Save subscription
        topicCallbacks.insert({topic, callback});

    }


    void SerialFiller::HandleRxDataReceived(ByteArray rxData) {

        std::vector<ByteArray> packets;
        SerialFillerHelper::PacketizeData(rxData, rxBuffer, packets);

        for(auto it = packets.begin(); it != packets.end(); it++) {
            std::string topic;
            ByteArray data;

            // FOR EACH PACKET:
            // 1. Remove COBS encoding
            ByteArray decodedData;
            CobsTranscoder::Decode(*it, decodedData);

            // 2. Verify CRC
            // This function will throw if CRC does not match
            SerialFillerHelper::VerifyCrc(decodedData);

            // Then split packet into topic and data
            SerialFillerHelper::SplitPacket(decodedData, topic, data);

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


}