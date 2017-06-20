#include "gtest/gtest.h"

#include "SerialFiller/Crc16Ccitt1021.hpp"

using namespace mn::SerialFiller;

namespace {

// The fixture for testing class Foo.
    class CrcTests : public ::testing::Test {
    protected:

        CrcTests() {
        }

        virtual ~CrcTests() {
        }
    };

    TEST_F(CrcTests, StandardCheckTest) {
        uint16_t crcVal2 = Crc16Ccitt1021::Calc({ '1', '2', '3', '4', '5', '6', '7', '8', '9' });
        EXPECT_EQ(0x29B1, crcVal2);
    }

    TEST_F(CrcTests, EmptyTest) {
        uint16_t crcVal2 = Crc16Ccitt1021::Calc({ });
        EXPECT_EQ(0xFFFF, crcVal2);
    }

    TEST_F(CrcTests, ZeroTest) {
        uint16_t crcVal2 = Crc16Ccitt1021::Calc({'\x00'});
        EXPECT_EQ(0xE1F0, crcVal2);
    }

    TEST_F(CrcTests, LargeTest) {

        ByteArray data;
        ByteArray sequence({ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' });
        // Populate data with 300 characters
        for(int i = 0; i < 30; i++) {
            std::copy(sequence.begin(), sequence.end(), std::back_inserter(data));
        }

        uint16_t crcVal2 = Crc16Ccitt1021::Calc(data);
        EXPECT_EQ(0xC347, crcVal2);
    }

}  // namespace