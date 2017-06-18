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

    TEST_F(CrcTests, StandardCheckTest) {
        uint16_t crcVal2 = Crc16CCitt1021::Calc("123456789");
        EXPECT_EQ(0x29B1, crcVal2);
    }

    TEST_F(CrcTests, EmptyTest) {
        uint16_t crcVal2 = Crc16CCitt1021::Calc("");
        EXPECT_EQ(0xFFFF, crcVal2);
    }

    TEST_F(CrcTests, ZeroTest) {
        uint16_t crcVal2 = Crc16CCitt1021::Calc({'\x00'});
        EXPECT_EQ(0xE1F0, crcVal2);
    }

    TEST_F(CrcTests, LargeTest) {

        std::string data;
        // Populate data with 300 characters
        for(int i = 0; i < 30; i++) {
            data += "0123456789";
        }

        uint16_t crcVal2 = Crc16CCitt1021::Calc(data);
        EXPECT_EQ(0xC347, crcVal2);
    }

}  // namespace