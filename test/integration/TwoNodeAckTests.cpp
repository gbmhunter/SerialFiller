///
/// \file 				TwoNodeTests.hpp
/// \author 			Geoffrey Hunter <gbmhunter@gmail.com> (www.mbedded.ninja)
/// \edited             n/a
/// \created			2017-09-21
/// \last-modified		2017-09-21
/// \brief 				Contains tests for the SerialFiller class that involve two SerialFiller nodes.
/// \details
///		See README.md in root dir for more info.

#include <thread>

#include "gtest/gtest.h"

#include "SerialFiller/SerialFiller.hpp"

#include "ThreadSafeQ.hpp"
#include "Node.hpp"

using namespace mn::SerialFiller;

namespace {

// The fixture for testing class Foo.
    class TwoNodeAckTests : public ::testing::Test {
    protected:


        Node node1_;
        Node node2_;

        TwoNodeAckTests() : node1_("node1"), node2_("node2") {

            // Connect node 1 output to node 2 input and vise versa
            node1_.serialFiller_.txDataReady_ = ([&](ByteQueue txData) -> void {
                while(txData.size()) {
                    node2_.rxQueue_.Push(txData.front());
                    txData.pop_front();
                }
            });
            node2_.serialFiller_.txDataReady_ = ([&](ByteQueue txData) -> void {
                while(txData.size()) {
                    node1_.rxQueue_.Push(txData.front());
                    txData.pop_front();
                }
            });
        }

        virtual ~TwoNodeAckTests() {
        }
    };

    TEST_F(TwoNodeAckTests, SingleTopic) {
        auto topic = ByteArray();
        auto data = ByteArray();

        // Subscribe to a test topic
        ByteArray savedData;
        node2_.serialFiller_.Subscribe("test-topic", [&](ByteArray data) -> void {
            savedData = data;
        });

        auto dataToSend = ByteArray({ 0x01, 0x02, 0x03, 0x04 });

        // Call PublishWait
        auto gotAck = node1_.serialFiller_.PublishWait("test-topic", dataToSend, std::chrono::milliseconds(1000));

        EXPECT_TRUE(gotAck);
        EXPECT_EQ(dataToSend, savedData);
        EXPECT_EQ(0, node1_.serialFiller_.NumThreadsWaiting());
    }

    TEST_F(TwoNodeAckTests, NoResponse) {
        auto topic = ByteArray();
        auto data = ByteArray();

        // Subscribe to a test topic
        ByteArray savedData;
        node2_.serialFiller_.Subscribe("test-topic", [&](ByteArray data) -> void {
            savedData = data;
        });

        node2_.serialFiller_.SetAckEnabled(false);

        // Publish data on topic
        auto dataToSend = ByteArray({ 0x01, 0x02, 0x03, 0x04 });
        auto gotAck = node1_.serialFiller_.PublishWait("test-topic", dataToSend, std::chrono::milliseconds(1000));
        EXPECT_FALSE(gotAck);
        EXPECT_EQ(dataToSend, savedData);
    }

    TEST_F(TwoNodeAckTests, TwoWayTest) {
        auto topic = ByteArray();
        auto data = ByteArray();

        // Subscribe to a test topic
        ByteArray savedNode1Data;
        node1_.serialFiller_.Subscribe("test-topic", [&](ByteArray data) -> void {
            savedNode1Data = data;
        });
        ByteArray savedNode2Data;
        node2_.serialFiller_.Subscribe("test-topic", [&](ByteArray data) -> void {
            savedNode2Data = data;
        });

        // Publish data on topic
        auto node1DataToSend = ByteArray({ 0x01, 0x02, 0x03, 0x04 });
        auto node2DataToSend = ByteArray({ 0x0A, 0x0B, 0x0C, 0x0D });

        bool node1GotAck;
        std::thread t1([&]() {
            node1GotAck = node1_.serialFiller_.PublishWait("test-topic", node1DataToSend, std::chrono::milliseconds(1000));
        });
        bool node2GotAck;
        std::thread t2([&]() {
            node2GotAck = node2_.serialFiller_.PublishWait("test-topic", node2DataToSend, std::chrono::milliseconds(1000));
        });

        t1.join();
        t2.join();

        EXPECT_TRUE(node1GotAck);
        EXPECT_TRUE(node2GotAck);
        EXPECT_EQ(node1DataToSend, savedNode2Data);
        EXPECT_EQ(node2DataToSend, savedNode1Data);
    }

    TEST_F(TwoNodeAckTests, TwoPublishWaitCalls) {
        auto topic = ByteArray();
        auto data = ByteArray();

        // Subscribe to two topics
        ByteArray savedMsg1Data;
        node2_.serialFiller_.Subscribe("topic1", [&](ByteArray data) -> void {
            savedMsg1Data = data;
        });
        ByteArray savedMsg2Data;
        node2_.serialFiller_.Subscribe("topic2", [&](ByteArray data) -> void {
            savedMsg2Data = data;
        });


        // Publish data on both topics
        auto msg1Data = ByteArray({ 0x01, 0x02, 0x03, 0x04 });
        auto msg2Data = ByteArray({ 0x0A, 0x0B, 0x0C, 0x0D });

        bool msg1GotAck;
        std::thread t1([&]() {
            msg1GotAck = node1_.serialFiller_.PublishWait("topic1", msg1Data, std::chrono::milliseconds(1000));
        });
        bool msg2GotAck;
        std::thread t2([&]() {
            msg2GotAck = node1_.serialFiller_.PublishWait("topic2", msg2Data, std::chrono::milliseconds(1000));
        });

        t1.join();
        t2.join();

        EXPECT_TRUE(msg1GotAck);
        EXPECT_TRUE(msg2GotAck);
        EXPECT_EQ(msg1Data, savedMsg1Data);
        EXPECT_EQ(msg2Data, savedMsg2Data);
    }

    TEST_F(TwoNodeAckTests, SubscribeCallsPublish) {
        auto topic = ByteArray();
        auto data = ByteArray();

        // Subscribe to a test topic
        ByteArray savedNode1Data;
        node1_.serialFiller_.Subscribe("response", [&](ByteArray data) -> void {
            savedNode1Data = data;
        });

        ByteArray savedNode2Data;
        node2_.serialFiller_.Subscribe("request", [&](ByteArray data) -> void {
            // This should test that the node2 mutex is unlocked before calling any subscribed callbacks
            // otherwise we would get into deadlock on this call
            node2_.serialFiller_.Publish("response", { 0x02 });
            savedNode2Data = data;
        });

        bool node1GotAck = node1_.serialFiller_.PublishWait("request", { 0x01 }, std::chrono::milliseconds(5000));

        node2_.Join();

        EXPECT_TRUE(node1GotAck);
        EXPECT_EQ(ByteArray({ 0x02 }), savedNode1Data);
        EXPECT_EQ(ByteArray({ 0x01 }), savedNode2Data);
    }

}  // namespace