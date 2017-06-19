#include "gtest/gtest.h"

#include "SerialFiller/SerialFiller.hpp"

using namespace mn;

namespace {

    class PacketizeTest : public ::testing::Test {
    protected:

        PacketizeTest() {
        }

        virtual ~PacketizeTest() {
        }
    };

    TEST_F(PacketizeTest, PacketizeData) {

        auto newRxData = ByteArray({ 0x01, 0x02, 0x00 });
        auto existingRxData = ByteArray();
        std::vector<ByteArray> packets;
        SerialFiller serialFiller;
        serialFiller.PacketizeData(newRxData, existingRxData, packets);

        EXPECT_EQ(1, packets.size());
        EXPECT_EQ(ByteArray({ 0x01, 0x02, 0x00 }), packets[0]);
    }

    TEST_F(PacketizeTest, PacketizeData2) {

        auto newRxData = ByteArray({ 0x01, 0x02, 0x00, 0x01, 0x00 });
        auto existingRxData = ByteArray();
        std::vector<ByteArray> packets;
        SerialFiller serialFiller;
        serialFiller.PacketizeData(newRxData, existingRxData, packets);

        EXPECT_EQ(2, packets.size());
        EXPECT_EQ(ByteArray({ 0x01, 0x02, 0x00 }), packets[0]);
        EXPECT_EQ(ByteArray({ 0x01, 0x00 }), packets[1]);
    }

    TEST_F(PacketizeTest, EmptyTest) {
        auto existingRxData = ByteArray();
        std::vector<ByteArray> packets;
        SerialFiller serialFiller;
        // Pass in an empty array for newRxData
        serialFiller.PacketizeData({}, existingRxData, packets);

        EXPECT_EQ(0, packets.size());
    }

    TEST_F(PacketizeTest, SegmenteddataTest) {
        auto existingRxData = ByteArray();
        std::vector<ByteArray> packets;
        SerialFiller serialFiller;
        serialFiller.PacketizeData({ 0x01, 0x02, 0x03 }, existingRxData, packets);

        EXPECT_EQ(0, packets.size());
        EXPECT_EQ(ByteArray({ 0x01, 0x02, 0x03 }), existingRxData);

        // Now add a EOF byte + start of next packet (which should complete the
        // packet partially received above)
        serialFiller.PacketizeData({ 0x00, 0xAA, 0xAB }, existingRxData, packets);

        EXPECT_EQ(1, packets.size());
        EXPECT_EQ(ByteArray({ 0x01, 0x02, 0x03, 0x00 }), packets[0]);
        EXPECT_EQ(ByteArray({ 0xAA, 0xAB }), existingRxData);

        serialFiller.PacketizeData({ 0x00 }, existingRxData, packets);

        EXPECT_EQ(1, packets.size());
        EXPECT_EQ(ByteArray({ 0xAA, 0xAB, 0x00 }), packets[0]);
        EXPECT_EQ(ByteArray({ }), existingRxData);
    }

}  // namespace