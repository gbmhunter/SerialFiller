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

        auto newRxData = std::string({ 0x01, 0x02, 0x00 });
        auto existingRxData = std::string();
        std::vector<std::string> packets;
        SerialFiller serialFiller;
        serialFiller.PacketizeData(newRxData, existingRxData, packets);

        EXPECT_EQ(1, packets.size());
        EXPECT_EQ(std::string({ 0x01, 0x02, 0x00 }), packets[0]);
    }

    TEST_F(PacketizeTest, PacketizeData2) {

        auto newRxData = std::string({ 0x01, 0x02, 0x00, 0x01, 0x00 });
        auto existingRxData = std::string();
        std::vector<std::string> packets;
        SerialFiller serialFiller;
        serialFiller.PacketizeData(newRxData, existingRxData, packets);

        EXPECT_EQ(2, packets.size());
        EXPECT_EQ(std::string({ 0x01, 0x02, 0x00 }), packets[0]);
        EXPECT_EQ(std::string({ 0x01, 0x00 }), packets[1]);
    }

}  // namespace