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
        auto packet = std::string("test-topic:hello");
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

}  // namespace