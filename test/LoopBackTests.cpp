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
            serialFiller.txDataReady_ = [&](std::string txData) -> void {
                serialFiller.HandleRxDataReceived(txData);
            };
        }

        virtual ~LoopBackTests() {
        }
    };

    TEST_F(LoopBackTests, SingleTopicTest) {
        auto topic = std::string("");
        auto data = std::string("");

        // Subscribe to a test topic
        std::string savedData;
        serialFiller.Subscribe("test-topic", [&](std::string data) -> void {
            savedData = data;
        });

        // Publish data on topic
        serialFiller.Publish("test-topic", "hello");

        EXPECT_EQ("hello", savedData);
    }

    TEST_F(LoopBackTests, DataWithZerosTest) {
        auto topic = std::string("");
        auto data = std::string("");

        // Subscribe to a test topic
        std::string savedData;
        serialFiller.Subscribe("test-topic", [&](std::string data) -> void {
            savedData = data;
        });

        // Publish data on topic
        serialFiller.Publish("test-topic", std::string({ '\x00', '\x00' }));

        EXPECT_EQ(std::string({ '\x00', '\x00' }), savedData);
    }

    TEST_F(LoopBackTests, MultiTopicTest) {
        auto topic = std::string("");
        auto data = std::string("");

        // Subscribe to some topics (sharing the data object)
        std::string savedData;
        serialFiller.Subscribe("topic1", [&](std::string data) -> void {
            savedData = data;
        });
        serialFiller.Subscribe("topic2", [&](std::string data) -> void {
            savedData = data;
        });

        // Publish data on topic
        serialFiller.Publish("topic1", "hello");
        EXPECT_EQ("hello", savedData);
        savedData.clear();

        serialFiller.Publish("topic2", "world");
        EXPECT_EQ("world", savedData);
        savedData.clear();

        // Publish on topic we haven't registered on
        serialFiller.Publish("topic3", "nope");
        EXPECT_EQ("", savedData);
        savedData.clear();
    }

}  // namespace