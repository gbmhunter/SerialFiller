#include "gtest/gtest.h"

#include "SerialFiller/SerialFiller.hpp"

using namespace mn::SerialFiller;

namespace {

    class NoSubscribersTests : public ::testing::Test {
    protected:

        SerialFiller serialFiller;

        NoSubscribersTests() {

            // Connect output to input (software loopback)
            serialFiller.txDataReady_ = ([&](ByteQueue txData) -> void {
                serialFiller.GiveRxData(txData);
            });
        }

        virtual ~NoSubscribersTests() {
        }
    };

    TEST_F(NoSubscribersTests, NoSubscribersEventFired) {
        auto topic = ByteArray();
        auto data = ByteArray();

        // Add listener to "no subscribers for topic" event
        bool listenerCalled = false;
        std::string savedTopic;
        ByteArray savedData;
        serialFiller.noSubscribersForTopic_.AddListener([&](std::string topic, ByteArray data) {
            listenerCalled = true;
            savedTopic = topic;
            savedData = data;
        });

        // Publish data on topic
        serialFiller.Publish("BogusTopic", { 'h', 'e', 'l', 'l', 'o'});

        // Since there was no subscribers, the "no subscribers for topic" event
        // should of fired!
        EXPECT_TRUE(listenerCalled);
        EXPECT_EQ("BogusTopic", savedTopic);
        EXPECT_EQ(ByteArray({ 'h', 'e', 'l', 'l', 'o'}), savedData);
    }

    TEST_F(NoSubscribersTests, NoSubscribersEventNotFired) {
        auto topic = ByteArray();
        auto data = ByteArray();

        // Add listener to "no subscribers for topic" event
        bool listenerCalled = false;
        std::string savedTopic;
        ByteArray savedData;
        serialFiller.noSubscribersForTopic_.AddListener([&](std::string topic, ByteArray data) {
            listenerCalled = true;
            savedTopic = topic;
            savedData = data;
        });

        // Subscribe to TestTopic, so that the "no subscribers" event
        // should not be fired
        serialFiller.Subscribe("TestTopic", [&](ByteArray data) {
            // do nothing
        });

        // Publish data on topic
        serialFiller.Publish("TestTopic", { 'h', 'e', 'l', 'l', 'o'});

        EXPECT_FALSE(listenerCalled);
    }

}  // namespace