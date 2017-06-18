#include "gtest/gtest.h"

#include "SerialFiller/SerialFiller.hpp"

using namespace mn;

namespace {

// The fixture for testing class Foo.
    class LoopBackTests : public ::testing::Test {
    protected:

        SerialFiller serialFiller;

        LoopBackTests() {

            // Connect output to input (software loopback)
            serialFiller.txDataReady_ = [&](ByteArray txData) -> void {
                serialFiller.HandleRxDataReceived(txData);
            };
        }

        virtual ~LoopBackTests() {
        }
    };

    TEST_F(LoopBackTests, SingleTopicTest) {
        auto topic = ByteArray();
        auto data = ByteArray();

        // Subscribe to a test topic
        ByteArray savedData;
        serialFiller.Subscribe("test-topic", [&](ByteArray data) -> void {
            savedData = data;
        });

        // Publish data on topic
        serialFiller.Publish("test-topic", { 'h', 'e', 'l', 'l', 'o'});

        EXPECT_EQ(ByteArray({ 'h', 'e', 'l', 'l', 'o'}), savedData);
    }

    TEST_F(LoopBackTests, DataWithZerosTest) {
        auto topic = std::string();
        auto data = ByteArray();

        // Subscribe to a test topic
        ByteArray savedData;
        serialFiller.Subscribe("test-topic", [&](ByteArray data) -> void {
            savedData = data;
        });

        // Publish data on topic
        serialFiller.Publish("test-topic", ByteArray({ '\x00', '\x00' }));

        EXPECT_EQ(ByteArray({ '\x00', '\x00' }), savedData);
    }

    TEST_F(LoopBackTests, MultiTopicTest) {
        auto topic = std::string();
        auto data = ByteArray();

        // Subscribe to some topics (sharing the data object)
        ByteArray savedData;
        serialFiller.Subscribe("topic1", [&](ByteArray data) -> void {
            savedData = data;
        });
        serialFiller.Subscribe("topic2", [&](ByteArray data) -> void {
            savedData = data;
        });

        // Publish data on topic
        serialFiller.Publish("topic1", { 'h' ,'e', 'l', 'l', 'o'});
        EXPECT_EQ(ByteArray({ 'h' ,'e', 'l', 'l', 'o'}), savedData);
        savedData.clear();

        serialFiller.Publish("topic2", { 'w', 'o', 'r', 'l', 'd'});
        EXPECT_EQ(ByteArray({ 'w', 'o', 'r', 'l', 'd'}), savedData);
        savedData.clear();

        // Publish on topic we haven't registered on
        serialFiller.Publish("topic3", {});
        EXPECT_EQ(ByteArray{}, savedData);
        savedData.clear();
    }

}  // namespace