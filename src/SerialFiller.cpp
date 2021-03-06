///
/// \file 				SerialFiller.cpp
/// \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
/// \edited             n/a
/// \created			2017-06-10
/// \last-modified		2018-01-30
/// \brief 				Contains the SerialFiller class.
/// \details
///		See README.md in root dir for more info.

// System includes
#include <functional>
#include <stdexcept>
#include <string>
#include <SerialFiller/String.hpp>

// User includes
#include "SerialFiller/CobsTranscoder.hpp"
#include "SerialFiller/Crc16Ccitt1021.hpp"
#include "SerialFiller/SerialFillerHelper.hpp"
#include "SerialFiller/Exceptions/NotEnoughBytes.hpp"

// Module include
#include "SerialFiller/SerialFiller.hpp"

namespace mn {
    namespace SerialFiller {

        SerialFiller::SerialFiller(std::shared_ptr<Logger> logger) :
                logger_(logger),
                nextPacketId_(0),
                ackEnabled_(false),
                threadSafetyEnabled_(true),
                nextFreeSubsriberId_(0) {
            if(!logger_)
                // Create "dead" logger
                logger_ = std::shared_ptr<Logger>(new Logger("", Logger::Severity::NONE, Logger::Color::NONE, std::function<void(std::string)>()));
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
            LOG((*logger_), DEBUG, std::string() + "Method called.");

            std::unique_lock<std::mutex> lock(classMutex_, std::defer_lock);
            if(threadSafetyEnabled_)
                lock.lock();

            if(!ackEnabled_)
                LOG((*logger_), WARNING, std::string() + "PublishWait() called but auto-ACK was not enabled.");

            // Create cv and bool
            auto event = std::make_shared<EventType>();

            auto packetId = nextPacketId_;
            ackEvents_[packetId] = event;

            // Call the standard publish
            PublishInternal(topic, data);

            bool gotAck;
            if(ackEnabled_) {
                gotAck = ackEvents_[packetId]->first.wait_for(lock, timeout, [this, packetId]() {
                    auto it = ackEvents_.find(packetId);
                    if (it == ackEvents_.end())
                        throw std::runtime_error("Could not find entry in map.");

                    return it->second->second;
                });

                // Remove event from map
                ackEvents_.erase(packetId);
            } else {
                gotAck = true;
            }
            LOG((*logger_), DEBUG, "Method returning...");
            return gotAck;
        }


        uint32_t SerialFiller::Subscribe(std::string topic, std::function<void(ByteArray)> callback) {
            LOG((*logger_), DEBUG, std::string() + "Method called.");
            std::unique_lock<std::mutex> lock(classMutex_, std::defer_lock);
            if(threadSafetyEnabled_)
                lock.lock();

            // Assign ID and update free IDs
            auto id = nextFreeSubsriberId_;
            nextFreeSubsriberId_++;

            // Save subscription
            Subscriber subscriber;
            subscriber.id_ = id;
            subscriber.callback_ = callback;
            subscribers_.insert({topic, subscriber});

            
            return id;
        }

        void SerialFiller::Unsubscribe(uint32_t subscriberId) {

            std::unique_lock<std::mutex> lock(classMutex_, std::defer_lock);
            if(threadSafetyEnabled_)
                lock.lock();

            // Search for subscriber with provided ID
            for(auto it = subscribers_.begin(); it != subscribers_.end(); ++it) {
                if((*it).second.id_ == subscriberId) {
                    subscribers_.erase(it);
                    return;
                }
            }

            // If we reach here, no subscriber was found!
            throw SerialFillerException(std::string() + __PRETTY_FUNCTION__ + " called but subscriber ID of " + std::to_string(subscriberId) + " was not found.");
        }

        void SerialFiller::UnsubscribeAll() noexcept {

            std::unique_lock<std::mutex> lock(classMutex_, std::defer_lock);
            if(threadSafetyEnabled_)
                lock.lock();

            subscribers_.clear();
        }


        void SerialFiller::GiveRxData(ByteQueue &rxData) {
            LOG((*logger_), DEBUG, std::string() + "Method called with rxData = " + mn::CppUtils::String::ToHex(rxData));
            std::unique_lock<std::mutex> lock(classMutex_, std::defer_lock);

            if(threadSafetyEnabled_) {
                LOG((*logger_), DEBUG, "Locking mutex...");
                lock.lock();
                LOG((*logger_), DEBUG, "Mutex locked.");
            }

            LOG((*logger_), DEBUG, "Before extracting packets, rxBuffer_ = " + CppUtils::String::ToHex(rxBuffer_));
            LOG((*logger_), DEBUG, "Before extracting packets, rxData = " + CppUtils::String::ToHex(rxData));

            ByteArray packet;
            SerialFillerHelper::MoveRxDataInBuffer(rxData, rxBuffer_, packet);
            while (!packet.empty()) {

                LOG((*logger_), DEBUG, "Found packet. Data (COBS encoded) = " + CppUtils::String::ToHex(packet));
                LOG((*logger_), DEBUG, "rxBuffer_ now = " + CppUtils::String::ToHex(rxBuffer_));
                LOG((*logger_), DEBUG, "rxData now = " + CppUtils::String::ToHex(rxData));
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

                    // WARNING: Make sure to send ack BEFORE invoking topic callbacks, as they may cause other messages
                    // to be sent, and we always want the ACK to be the first thing sent back to the sender.
                    if(ackEnabled_)
                        SendAck(packetId);

                    // 5. Call every callback associated with this topic
                    RangeType range = subscribers_.equal_range(topic);

                    // If no subscribers are listening to this topic,
                    // fire the "no subscribers for topic" event (user can
                    // listen to this)
                    if(range.first == range.second) {
                        if(threadSafetyEnabled_) lock.unlock();
                        noSubscribersForTopic_.Fire(topic, data);
                        if(threadSafetyEnabled_) lock.lock();
                    }

                    for (Subscribers::iterator rangeIt = range.first; rangeIt != range.second; ++rangeIt) {
                        if(threadSafetyEnabled_) lock.unlock();
//                        std::cout << "Calling listener..." << std::endl;
                        rangeIt->second.callback_(data);
//                        std::cout << "Listener finished, relocking..." << std::endl;
                        if(threadSafetyEnabled_) lock.lock();
//                        std::cout << "Relocked." << std::endl;
                    }


                } else if (packetType == PacketType::ACK) {
                    LOG((*logger_), DEBUG, "Received ACK packet.");
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

                SerialFillerHelper::MoveRxDataInBuffer(rxData, rxBuffer_, packet);
            }
            LOG((*logger_), DEBUG, std::string() + "Method finished.");
        }

        void SerialFiller::SetAckEnabled(bool value) {
            ackEnabled_ = value;
        }

        void SerialFiller::SendAck(uint16_t packetId) {
            LOG((*logger_), DEBUG, "Method called with packetId = " + std::to_string(packetId));

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

//            std::cout << "SendAck() finished." << std::endl;
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