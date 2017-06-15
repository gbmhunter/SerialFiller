#include "gtest/gtest.h"

#include "SerialFiller/SerialFiller.hpp"

using namespace MN;

namespace {

// The fixture for testing class Foo.
    class CobsEncodeDecodeTest : public ::testing::Test {
    protected:

        CobsEncodeDecodeTest() {
        }

        virtual ~CobsEncodeDecodeTest() {
        }
    };

    TEST_F(CobsEncodeDecodeTest, NoZerosInDataTest) {
        std::vector<uint8_t> rawData = std::vector<uint8_t>({0x01, 0x02, 0x03});
        std::vector<uint8_t> encodedData;
        SerialFiller::CobsEncoder(rawData, encodedData);
        EXPECT_EQ(std::vector<uint8_t>({0x04, 0x01, 0x02, 0x03, 0x00}), encodedData);

        std::vector<uint8_t> decodedData;
        SerialFiller::DecodeStatus decodeStatus = SerialFiller::CobsDecoder(encodedData, decodedData);
        EXPECT_EQ(SerialFiller::DecodeStatus::SUCCESS, decodeStatus);
        EXPECT_EQ(rawData, decodedData);
    }

    TEST_F(CobsEncodeDecodeTest, ZeroInDataTest) {
        std::vector<uint8_t> rawData = std::vector<uint8_t>({0xAA, 0x00, 0xAB});
        std::vector<uint8_t> encodedData;
        SerialFiller::CobsEncoder(rawData, encodedData);
        EXPECT_EQ(std::vector<uint8_t>({0x02, 0xAA, 0x02, 0xAB, 0x00}), encodedData);

        std::vector<uint8_t> decodedData;
        SerialFiller::DecodeStatus decodeStatus = SerialFiller::CobsDecoder(encodedData, decodedData);
        EXPECT_EQ(SerialFiller::DecodeStatus::SUCCESS, decodeStatus);
        EXPECT_EQ(rawData, decodedData);
    }

    TEST_F(CobsEncodeDecodeTest, AllZerosTest) {
        std::vector<uint8_t> rawData = std::vector<uint8_t>({0x00, 0x00, 0x00});
        std::vector<uint8_t> encodedData;
        SerialFiller::CobsEncoder(rawData, encodedData);
        EXPECT_EQ(std::vector<uint8_t>({0x01, 0x01, 0x01, 0x01, 0x00}), encodedData);
        std::vector<uint8_t> decodedData;
        SerialFiller::DecodeStatus decodeStatus = SerialFiller::CobsDecoder(encodedData, decodedData);
        EXPECT_EQ(SerialFiller::DecodeStatus::SUCCESS, decodeStatus);
        EXPECT_EQ(rawData, decodedData);
    }

    TEST_F(CobsEncodeDecodeTest, ManyBytesTest) {
        std::vector<uint8_t> rawData = std::vector<uint8_t>({0x00, 0xAA, 0xAB, 0xAC, 0x00, 0x00, 0xAD});
        std::vector<uint8_t> encodedData;
        SerialFiller::CobsEncoder(rawData, encodedData);
        EXPECT_EQ(std::vector<uint8_t>({0x01, 0x04, 0xAA, 0xAB, 0xAC, 0x01, 0x02, 0xAD, 0x00}), encodedData);
        std::vector<uint8_t> decodedData;
        SerialFiller::DecodeStatus decodeStatus = SerialFiller::CobsDecoder(encodedData, decodedData);
        EXPECT_EQ(rawData, decodedData);
    }

    TEST_F(CobsEncodeDecodeTest, IncorrectDecodeTest) {
        // Encoded data is incorrectly formed, 0x02 states there should be 1 byte of valid data following,
        // but next byte is 0x00 (end of frame)
        std::vector<uint8_t> decodedData;
        SerialFiller::DecodeStatus decodeStatus = SerialFiller::CobsDecoder(std::vector<uint8_t>({0x02, 0x00, 0x00}), decodedData);
        EXPECT_EQ(SerialFiller::DecodeStatus::ERROR_ZERO_BYTE_NOT_EXPECTED, decodeStatus);
        EXPECT_EQ(std::vector<uint8_t>(), decodedData);
    }

    TEST_F(CobsEncodeDecodeTest, IncorrectDecodeTest2) {
        // Error here is the 0x00 after 0x03
        std::vector<uint8_t> decodedData;
        SerialFiller::DecodeStatus decodeStatus = SerialFiller::CobsDecoder(std::vector<uint8_t>({0x02, 0xAA, 0x03, 0x00, 0xAB}), decodedData);
        EXPECT_EQ(SerialFiller::DecodeStatus::ERROR_ZERO_BYTE_NOT_EXPECTED, decodeStatus);
        EXPECT_EQ(std::vector<uint8_t>(), decodedData);
    }

}  // namespace