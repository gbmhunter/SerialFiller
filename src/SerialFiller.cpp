///
/// \file 				SerialFiller.cpp
/// \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
/// \edited             n/a
/// \created			2017-06-10
/// \last-modified		2015-06-20
/// \brief 				Contains the SerialFiller class.
/// \details
///		See README.md in root dir for more info.


// User includes
#include "SerialFiller/SerialFiller.hpp"
#include "SerialFiller/Crc16Ccitt1021.hpp"
#include "SerialFiller/Exceptions/NotEnoughBytes.hpp"

namespace mn {
    namespace SerialFiller {

        void SerialFiller::Publish(std::string topic, ByteArray data) {

            ByteArray packet;
            std::copy(topic.begin(), topic.end(), std::back_inserter(packet));

            packet.push_back(':');
            std::copy(data.begin(), data.end(), std::back_inserter(packet));

            // Add CRC
            SerialFillerHelper::AddCrc(packet);

            ByteArray encodedData;
            CobsTranscoder::Encode(packet, encodedData);

            ByteQueue txData(encodedData.begin(), encodedData.end());

            // Emit TX send event
            txDataReady_.Fire(txData);
        };


        void SerialFiller::Subscribe(std::string topic, std::function<void(ByteArray)> callback) {

            // Save subscription
            topicCallbacks.insert({topic, callback});
        }


        void SerialFiller::GiveRxData(ByteQueue &rxData) {

            ByteArray packet;
            while (SerialFillerHelper::MoveRxDataInBuffer(rxData, rxBuffer, packet), !packet.empty()) {

                std::string topic;
                ByteArray data;

                //==============================//
                //======= FOR EACH PACKET ======//
                //==============================//

                // 1. Check it meets the minimum size requirements for a packet
                if (packet.size() < minNumBytesPerPacket)
                    throw NotEnoughBytes(packet);

                // 2. Remove COBS encoding
                //    This method will throw if the packet does not have a valid COBS
                //    encoding.
                ByteArray decodedData;
                CobsTranscoder::Decode(packet, decodedData);

                // 3. Verify CRC
                //    This function will throw if CRC does not match. We don't
                //    want to prevent the processing of further packets, so catch error,
                //    save it, and throw once packet processing is finished
                SerialFillerHelper::VerifyCrc(decodedData);

                // 4. Then split packet into topic and data
                //    This will throw if the ':' that splits the topic from the data cannot
                //    be found, or the topic is not a valid string
                SerialFillerHelper::SplitPacket(decodedData, topic, data);

                // 5. Call every callback associated with this topic
                RangeType range = topicCallbacks.equal_range(topic);
                for (TopicCallback::iterator rangeIt = range.first; rangeIt != range.second; ++rangeIt) {
                    rangeIt->second(data);
                }

                SerialFillerHelper::MoveRxDataInBuffer(rxData, rxBuffer, packet);
            }
        }
    } // namespace SerialFiller
} // namespace mn