///
/// \file 				SerialFiller.cpp
/// \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
/// \edited             n/a
/// \created			2017-06-10
/// \last-modified		2017-09-22
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

        SerialFiller::SerialFiller() :
                nextPacketId_(0),
                ackEnabled_(false),
                threadSafetyEnabled_(true) {
            // nothing
        }

        void SerialFiller::Publish(std::string topic, ByteArray data) {
            std::unique_lock<std::mutex> lock(classMutex_, std::defer_lock);
            if(threadSafetyEnabled_)
                lock.lock();

            PublishInternal(topic, data);
        };

        bool SerialFiller::PublishWait(
                const std::string &topic,
                const ByteArray &data,
                std::chrono::milliseconds timeout) {

            std::unique_lock<std::mutex> lock(classMutex_, std::defer_lock);
            if(threadSafetyEnabled_)
                lock.lock();

            if(!ackEnabled_)
                throw std::runtime_error("PublishWait() called but auto-ACK was not enabled.");

            // Create cv and bool
            auto event = std::make_shared<EventType>();

            auto packetId = nextPacketId_;
            ackEvents_[packetId] = event;

            // Call the standard publish
            PublishInternal(topic, data);

            bool gotAck = ackEvents_[packetId]->first.wait_for(lock, timeout, [this, packetId]() {
                auto it = ackEvents_.find(packetId);
                if(it == ackEvents_.end())
                    throw std::runtime_error("Could not find entry in map.");

                return it->second->second;
            });
//            std::cout << "wait_for() returned gotAck = ." << gotAck << std::endl;

            // Remove event from map
            ackEvents_.erase(packetId);

            return gotAck;
        }


        void SerialFiller::Subscribe(std::string topic, std::function<void(ByteArray)> callback) {
//            std::cout << "Subscribe() called." << std::endl;
            std::unique_lock<std::mutex> lock(classMutex_, std::defer_lock);
            if(threadSafetyEnabled_)
                lock.lock();

            // Save subscription
            topicCallbacks.insert({topic, callback});

        }


        void SerialFiller::GiveRxData(ByteQueue &rxData) {

//            std::cout << "GiveRxData() called" << std::endl;
            std::unique_lock<std::mutex> lock(classMutex_, std::defer_lock);

            if(threadSafetyEnabled_)
                lock.lock();

            ByteArray packet;
            while (SerialFillerHelper::MoveRxDataInBuffer(rxData, rxBuffer, packet), !packet.empty()) {

                std::string topic;
                ByteArray data;

                //==============================//
                //======= FOR EACH PACKET ======//
                //==============================//


                // Remove COBS encoding
                //    This method will throw if the packet does not have a valid COBS
                //    encoding.
                ByteArray decodedData;
                CobsTranscoder::Decode(packet, decodedData);

                // Verify CRC
                //    This function will throw if CRC does not match.
                SerialFillerHelper::VerifyCrc(decodedData);

                // Look at packet type
                auto packetType = static_cast<PacketType>(decodedData[0]);
                if(packetType == PacketType::PUBLISH) {
//                    std::cout << "Received PUBLISH packet." << std::endl;

                    uint16_t packetId = (static_cast<uint16_t>(decodedData.at(1)) << 8) | (static_cast<uint16_t>(decodedData.at(2)) << 0);
                    // 4. Then split packet into topic and data
                    //    This will throw if topic length is invalid
                    SerialFillerHelper::SplitPacket(decodedData, 3, topic, data);

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

                    if(ackEnabled_)
                        SendAck(packetId);


                } else if (packetType == PacketType::ACK) {
//                    std::cout << "Received ACK packet." << std::endl;
                    if(!ackEnabled_)
                        throw std::runtime_error("SerialFiller node received ACK packet but auto-ACK was not enabled.");

                    // Extract packet ID
                    uint16_t packetId = (static_cast<uint16_t>(decodedData.at(1)) << 8) | (static_cast<uint16_t>(decodedData.at(2)) << 0);

//                    std::cout << "Looking for packetId = " << packetId << std::endl;
                    auto it = ackEvents_.find(packetId);
                    if(it == ackEvents_.end()) {
//                        std::cout << "No threads waiting on ACK." << std::endl;
                    } else {
                        it->second->second = true;
                        it->second->first.notify_all();
                    }

                } else {
                    throw std::runtime_error("Received packet type not recognized.");
                }

                SerialFillerHelper::MoveRxDataInBuffer(rxData, rxBuffer, packet);
            }
        }

        void SerialFiller::SetAckEnabled(bool value) {
            ackEnabled_ = value;
        }

        void SerialFiller::SendAck(uint16_t packetId) {
//            std::cout << "SendAck() called with packetId = " << packetId << std::endl;

            ByteArray packet;

            // 1st byte is packet type, in this case it's ACK
            packet.push_back(static_cast<uint8_t>(PacketType::ACK));

            // 2nd and 3rd bytes are packet ID (MSB then LSB)
            packet.push_back(static_cast<uint8_t>(packetId >> 8));
            packet.push_back(static_cast<uint8_t>(packetId >> 0));

            // Add CRC
            SerialFillerHelper::AddCrc(packet);

            // Encode data using COBS
            ByteArray encodedData;
            CobsTranscoder::Encode(packet, encodedData);

            ByteQueue txData(encodedData.begin(), encodedData.end());

            // Emit TX send event
            if(txDataReady_) {
                txDataReady_(txData);
            } else
                throw std::runtime_error(std::string() + __PRETTY_FUNCTION__ + " was called but txDataReady_ function has no valid function object.");

        }

        uint32_t SerialFiller::NumThreadsWaiting() {

            std::unique_lock<std::mutex> lock(classMutex_, std::defer_lock);
            if(threadSafetyEnabled_)
                lock.lock();

            return static_cast<uint32_t>(ackEvents_.size());
        }

        void SerialFiller::PublishInternal(const std::string &topic, const ByteArray &data) {
            ByteArray packet;

            // 1st byte is packet type, in this case it's PUBLISH
            packet.push_back(static_cast<uint8_t>(PacketType::PUBLISH));

            // 2nd and 3rd bytes are packet ID (MSB then LSB)
            packet.push_back(static_cast<uint8_t>(nextPacketId_ >> 8));
            packet.push_back(static_cast<uint8_t>(nextPacketId_ >> 0));

            // 4th byte (pre-COBS encoded) is num. of bytes for topic
            packet.push_back((unsigned char)topic.size());

            std::copy(topic.begin(), topic.end(), std::back_inserter(packet));

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

            // If everything was successful, increment packet ID
            nextPacketId_ += 1;
        }

        void SerialFiller::SetThreadSafetyEnabled(bool value) {
            threadSafetyEnabled_ = value;
        }


    } // namespace SerialFiller
} // namespace mn