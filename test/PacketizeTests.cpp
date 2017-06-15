#include "gtest/gtest.h"

#include "SerialFiller/SerialFiller.hpp"

using namespace MN;

namespace {

    class PacketizeTest : public ::testing::Test {
    protected:

        PacketizeTest() {
        }

        virtual ~PacketizeTest() {
        }
    };

    TEST_F(PacketizeTest, PacketizeData) {

        auto newRxData = std::vector<uint8_t>({ 0x01, 0x02, 0x00 });
        auto existingRxData = std::vector<uint8_t>();
        std::vector<std::vector<uint8_t>> packets;
        SerialFiller serialFiller;
        serialFiller.PacketizeData(newRxData, existingRxData, packets);

        EXPECT_EQ(1, packets.size());
        EXPECT_EQ(std::vector<uint8_t>({ 0x01, 0x02, 0x00 }), packets[0]);
    }

    TEST_F(PacketizeTest, PacketizeData2) {

        auto newRxData = std::vector<uint8_t>({ 0x01, 0x02, 0x00, 0x01, 0x00 });
        auto existingRxData = std::vector<uint8_t>();
        std::vector<std::vector<uint8_t>> packets;
        SerialFiller serialFiller;
        serialFiller.PacketizeData(newRxData, existingRxData, packets);

        EXPECT_EQ(2, packets.size());
        EXPECT_EQ(std::vector<uint8_t>({ 0x01, 0x02, 0x00 }), packets[0]);
        EXPECT_EQ(std::vector<uint8_t>({ 0x01, 0x00 }), packets[1]);
    }

}  // namespace