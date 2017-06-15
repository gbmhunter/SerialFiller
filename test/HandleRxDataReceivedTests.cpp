#include "gtest/gtest.h"

#include "SerialFiller/SerialFiller.hpp"

using namespace mn;

namespace {

// The fixture for testing class Foo.
    class HandleRxDataReceivedTests : public ::testing::Test {
    protected:

        HandleRxDataReceivedTests() {
        }

        virtual ~HandleRxDataReceivedTests() {
        }
    };

    TEST_F(HandleRxDataReceivedTests, Test1) {
        auto packet = std::string("test-topic:hello");
        auto topic = std::string("");
        auto data = std::string("");

        SerialFiller serialFiller;

        // Connect output to input (software loopback)
        serialFiller.txDataReady_ = [&](std::string txData) -> void {
            serialFiller.HandleRxDataReceived(txData);
        };

        std::string savedData;
        serialFiller.Subscribe("test-topic", [&](std::string data) -> void {
            savedData = data;
        });

        serialFiller.Publish("test-topic", "hello");

        EXPECT_EQ("hello", savedData);
//        EXPECT_EQ("hello", data);
    }

}  // namespace