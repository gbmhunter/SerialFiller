#include "gtest/gtest.h"

#include "SerialFiller/Crc16CCitt1021.hpp"

using namespace mn;

namespace {

// The fixture for testing class Foo.
    class CrcTests : public ::testing::Test {
    protected:

        CrcTests() {
        }

        virtual ~CrcTests() {
        }
    };

    TEST_F(CrcTests, SingleTopicTest) {

        unsigned char testData[] = "123456789";

        unsigned short crcVal = Crc16CCitt1021::Calc(testData, 9);

        EXPECT_EQ(0x29B1, crcVal);
    }

}  // namespace