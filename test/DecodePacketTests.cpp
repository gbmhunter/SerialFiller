#include "gtest/gtest.h"

#include "SerialFiller/SerialFiller.hpp"

using namespace MN;

namespace {

// The fixture for testing class Foo.
    class DecodePacketTests : public ::testing::Test {
    protected:

        DecodePacketTests() {
        }

        virtual ~DecodePacketTests() {
        }
    };

    TEST_F(DecodePacketTests, NoZerosInDataTest) {
        auto packet = std::string("test-topic:hello");
        auto topic = std::string("");
        auto data = std::string("");
        SerialFiller::DecodePacket(packet, topic, data);
        EXPECT_EQ("test-topic", topic);
        EXPECT_EQ("hello", data);
    }

}  // namespace