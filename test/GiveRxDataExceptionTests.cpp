#include "gtest/gtest.h"

#include "SerialFiller/SerialFiller.hpp"

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



}  // namespace