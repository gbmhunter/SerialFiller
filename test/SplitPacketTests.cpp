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
        std::string packetAsString = { 0x01, 0x00, 0x01, 0x04, 't', 'e', 's', 't', 'h', 'e', 'l', 'l', 'o', 0x01, 0x01};
        auto packet = ByteArray(packetAsString.begin(), packetAsString.end());
        auto topic = std::string();
        auto data = ByteArray();
        SerialFillerHelper::SplitPacket(packet, 3, topic, data);
        EXPECT_EQ("test", topic);
        EXPECT_EQ(ByteArray({'h', 'e', 'l', 'l', 'o'}), data);
    }

    TEST_F(SplitPacketTests, BogusTopicLength) {
        auto packet = ByteArray({ 0x01, 0x00, 0x01, 0x06, 0x02, 0x03 });
        auto topic = std::string();
        auto data = ByteArray();
        EXPECT_THROW(SerialFillerHelper::SplitPacket(packet, 3, topic, data), LengthOfTopicTooLong);
        EXPECT_EQ("", topic);
        EXPECT_TRUE(data.empty());

    }

}  // namespace