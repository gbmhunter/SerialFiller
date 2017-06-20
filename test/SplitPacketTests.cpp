#include "gtest/gtest.h"

#include "SerialFiller/SerialFiller.hpp"

using namespace mn::SerialFiller;

namespace {

    class SplitPacketTests : public ::testing::Test {
    protected:

        SplitPacketTests() {
        }

        virtual ~SplitPacketTests() {
        }
    };

    TEST_F(SplitPacketTests, BasicTest) {
        // The "12" at the end of the string is a fake (and incorrect) CRC
        // value, but this doesn't matter as SplitPacket does not validate the CRC
        std::string packetAsString = "test-topic:hello12";
        auto packet = ByteArray(packetAsString.begin(), packetAsString.end());
        auto topic = std::string();
        auto data = ByteArray();
        SerialFillerHelper::SplitPacket(packet, topic, data);
        EXPECT_EQ("test-topic", topic);
        EXPECT_EQ(ByteArray({'h', 'e', 'l', 'l', 'o'}), data);
    }

}  // namespace