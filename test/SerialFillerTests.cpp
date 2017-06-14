#include "gtest/gtest.h"

#include "SerialFiller/SerialFiller.hpp"

using namespace MN;

namespace {

// The fixture for testing class Foo.
    class FooTest : public ::testing::Test {
    protected:
        // You can remove any or all of the following functions if its body
        // is empty.

        FooTest() {
            // You can do set-up work for each test here.
        }

        virtual ~FooTest() {
            // You can do clean-up work that doesn't throw exceptions here.
        }

        // If the constructor and destructor are not enough for setting up
        // and cleaning up each test, you can define the following methods:

        virtual void SetUp() {
            // Code here will be called immediately after the constructor (right
            // before each test).
        }

        virtual void TearDown() {
            // Code here will be called immediately after each test (right
            // before the destructor).
        }

        // Objects declared here can be used by all tests in the test case for Foo.
    };


    TEST_F(FooTest, NoZerosInDataTest) {
        std::vector<uint8_t> rawData = std::vector<uint8_t>({0x01, 0x02, 0x03});
        std::vector<uint8_t> encodedData;
        SerialFiller::CobsEncoder(rawData, encodedData);
        EXPECT_EQ(std::vector<uint8_t>({0x04, 0x01, 0x02, 0x03, 0x00}), encodedData);

        std::vector<uint8_t> decodedData;
        SerialFiller::DecodeStatus decodeStatus = SerialFiller::CobsDecoder(encodedData, decodedData);
        EXPECT_EQ(SerialFiller::DecodeStatus::SUCCESS, decodeStatus);
        EXPECT_EQ(rawData, decodedData);
    }

    TEST_F(FooTest, ZeroInDataTest) {
        std::vector<uint8_t> rawData = std::vector<uint8_t>({0xAA, 0x00, 0xAB});
        std::vector<uint8_t> encodedData;
        SerialFiller::CobsEncoder(rawData, encodedData);
        EXPECT_EQ(std::vector<uint8_t>({0x02, 0xAA, 0x02, 0xAB, 0x00}), encodedData);

        std::vector<uint8_t> decodedData;
        SerialFiller::DecodeStatus decodeStatus = SerialFiller::CobsDecoder(encodedData, decodedData);
        EXPECT_EQ(SerialFiller::DecodeStatus::SUCCESS, decodeStatus);
        EXPECT_EQ(rawData, decodedData);
    }

    TEST_F(FooTest, AllZerosTest) {
        std::vector<uint8_t> rawData = std::vector<uint8_t>({0x00, 0x00, 0x00});
        std::vector<uint8_t> encodedData;
        SerialFiller::CobsEncoder(rawData, encodedData);
        EXPECT_EQ(std::vector<uint8_t>({0x01, 0x01, 0x01, 0x01, 0x00}), encodedData);
        std::vector<uint8_t> decodedData;
        SerialFiller::DecodeStatus decodeStatus = SerialFiller::CobsDecoder(encodedData, decodedData);
        EXPECT_EQ(SerialFiller::DecodeStatus::SUCCESS, decodeStatus);
        EXPECT_EQ(rawData, decodedData);
    }

    TEST_F(FooTest, ManyBytesTest) {
        std::vector<uint8_t> rawData = std::vector<uint8_t>({0x00, 0xAA, 0xAB, 0xAC, 0x00, 0x00, 0xAD});
        std::vector<uint8_t> encodedData;
        SerialFiller::CobsEncoder(rawData, encodedData);
        EXPECT_EQ(std::vector<uint8_t>({0x01, 0x04, 0xAA, 0xAB, 0xAC, 0x01, 0x02, 0xAD, 0x00}), encodedData);
        std::vector<uint8_t> decodedData;
        SerialFiller::DecodeStatus decodeStatus = SerialFiller::CobsDecoder(encodedData, decodedData);
        EXPECT_EQ(rawData, decodedData);
    }

    TEST_F(FooTest, IncorrectDecodeTest) {
        // Encoded data is incorrectly formed, 0x02 states there should be 1 byte of valid data following,
        // but next byte is 0x00 (end of frame)
        std::vector<uint8_t> decodedData;
        SerialFiller::DecodeStatus decodeStatus = SerialFiller::CobsDecoder(std::vector<uint8_t>({0x02, 0x00, 0x00}), decodedData);
        EXPECT_EQ(SerialFiller::DecodeStatus::ERROR_ZERO_BYTE_NOT_EXPECTED, decodeStatus);
        EXPECT_EQ(std::vector<uint8_t>(), decodedData);
    }

    TEST_F(FooTest, IncorrectDecodeTest2) {
        // Error here is the 0x00 after 0x03
        std::vector<uint8_t> decodedData;
        SerialFiller::DecodeStatus decodeStatus = SerialFiller::CobsDecoder(std::vector<uint8_t>({0x02, 0xAA, 0x03, 0x00, 0xAB}), decodedData);
        EXPECT_EQ(SerialFiller::DecodeStatus::ERROR_ZERO_BYTE_NOT_EXPECTED, decodeStatus);
        EXPECT_EQ(std::vector<uint8_t>(), decodedData);
    }

}  // namespace