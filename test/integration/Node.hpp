///
/// \file 				Node.hpp
/// \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
/// \edited             n/a
/// \created			2017-09-21
/// \last-modified		2017-09-21
/// \brief 				Contains the Node class.
/// \details
///		See README.md in root dir for more info.

#ifndef MN_SERIAL_FILLER_NODE_H_
#define MN_SERIAL_FILLER_NODE_H_

// System includes
#include <atomic>
#include <string>
#include <iostream>
#include <functional>
#include <thread>

#include "SerialFiller/SerialFiller.hpp"

#include "ThreadSafeQ.hpp"

using namespace mn::SerialFiller;

namespace mn {
    namespace SerialFiller {

        class Node {

        public:

            SerialFiller serialFiller_;
            mn::CppUtils::ThreadSafeQ<uint8_t> rxQueue_;

            Node(std::string name) :
                    name_(name),
                    breakThread_(false) {
                rxThread_ = std::thread(&Node::RxThreadFn, this);

                serialFiller_.SetAckEnabled(true);
            }

            ~Node() {
                breakThread_.store(true);
                rxThread_.join();
            }

            void RxThreadFn() {
//                std::cout << __FUNCTION__ << "() called for " << name_ << std::endl;

                while (true) {
                    // Wait for data to arrive on the queue
                    uint8_t data;
                    if (rxQueue_.TryPop(data, std::chrono::milliseconds(1000))) {
//                        std::cout << name_ << " received data." << std::endl;
                        ByteQueue dataAsQ;
                        dataAsQ.push_back(data);
                        serialFiller_.GiveRxData(dataAsQ);
                    }

                    if(breakThread_.load())
                        break;
                }

            }

        private:
            std::string name_;
            std::thread rxThread_;
            std::atomic<bool> breakThread_;
        };
    } // namespace SerialFiller
} // namespace mn

#endif // #ifndef MN_SERIAL_FILLER_NODE_H_