#include "gtest/gtest.h"

#include "SerialFiller/SerialFiller.hpp"

using namespace mn;

namespace {

    class SplitPacketTests : public ::testing::Test {
    protected:

        SplitPacketTests() {
        }

        virtual ~SplitPacketTests() {
        }
    };

    TEST_F(SplitPacketTests, BasicTest) {
        std::string packetAsString = "test-topic:hello";
        auto packet = ByteArray(packetAsString.begin(), packetAsString.end());
        auto topic = std::string();
        auto data = ByteArray();
        SerialFiller::SplitPacket(packet, topic, data);
        EXPECT_EQ("test-topic", topic);
        EXPECT_EQ(ByteArray({'h', 'e', 'l', 'l', 'o'}), data);
    }

}  // namespace