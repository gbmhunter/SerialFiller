
// 3rd party includes
#include "gtest/gtest.h"

// User includes
#include "SerialFiller/SerialFiller.hpp"
#include "SerialFiller/Exceptions/LengthOfTopicTooLong.hpp"

using namespace mn::SerialFiller;

namespace {

    class GiveRxDataExceptionTests : public ::testing::Test {
    protected:

        SerialFiller serialFiller;

        GiveRxDataExceptionTests() {

        }

        virtual ~GiveRxDataExceptionTests() {
        }
    };

    TEST_F(GiveRxDataExceptionTests, CrcCheckFailedTest) {
        // Data has a bad CRC!
        auto rxData = ByteQueue({ 0x05, 0x02, 0x03, 0x04, 0x05, 0x00 });
        EXPECT_THROW(serialFiller.GiveRxData(rxData), CrcCheckFailed);
        EXPECT_TRUE(rxData.empty());
    }

    TEST_F(GiveRxDataExceptionTests, NotEnoughBytesTest) {
        auto rxData = ByteQueue({ 0x01, 0x00 });
        EXPECT_THROW(serialFiller.GiveRxData(rxData), NotEnoughBytes);
        EXPECT_TRUE(rxData.empty());
    }

    TEST_F(GiveRxDataExceptionTests, NoTopicDataSeparator) {
        // Give a bogus long value for topic length (0x74).
        // CRC and COBS encoding is correct
        auto rxData = ByteQueue({ 0x07, 0x74, 0x01, 0x02, 0x03, 0x6D, 0x75, 0x00 });
        EXPECT_THROW(serialFiller.GiveRxData(rxData), LengthOfTopicTooLong);
        EXPECT_TRUE(rxData.empty());
    }
}  // namespace