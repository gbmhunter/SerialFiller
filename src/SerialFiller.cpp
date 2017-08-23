///
/// \file 				SerialFiller.cpp
/// \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
/// \edited             n/a
/// \created			2017-06-10
/// \last-modified		2017-08-23
/// \brief 				Contains the SerialFiller class.
/// \details
///		See README.md in root dir for more info.

// System includes
#include <stdexcept>

// User includes
#include "SerialFiller/SerialFiller.hpp"
#include "SerialFiller/Crc16Ccitt1021.hpp"
#include "SerialFiller/Exceptions/NotEnoughBytes.hpp"

namespace mn {
    namespace SerialFiller {

        void SerialFiller::Publish(std::string topic, ByteArray data) {

            ByteArray packet;

            // 1st byte (pre-COBS encoded) is num. of bytes for topic
            packet.push_back((unsigned char)topic.size());

            std::copy(topic.begin(), topic.end(), std::back_inserter(packet));

//            packet.push_back(':');
            std::copy(data.begin(), data.end(), std::back_inserter(packet));

            // Add CRC
            SerialFillerHelper::AddCrc(packet);

            // Encode data using COBS
            ByteArray encodedData;
            CobsTranscoder::Encode(packet, encodedData);

            ByteQueue txData(encodedData.begin(), encodedData.end());

            // Emit TX send event
            if(txDataReady_)
                txDataReady_(txData);
            else
                throw std::runtime_error(std::string() + __PRETTY_FUNCTION__ + " was called but txDataReady_ function has no valid function object.");
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

                // If no subscribers are listening to this topic,
                // fire the "no subscribers for topic" event (user can
                // listen to this)
                if(range.first == range.second) {
                    noSubscribersForTopic_.Fire(topic, data);
                }

                for (TopicCallback::iterator rangeIt = range.first; rangeIt != range.second; ++rangeIt) {
                    rangeIt->second(data);
                }

                SerialFillerHelper::MoveRxDataInBuffer(rxData, rxBuffer, packet);
            }
        }
    } // namespace SerialFiller
} // namespace mn