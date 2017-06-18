#include "gtest/gtest.h"

#include "SerialFiller/SerialFiller.hpp"
#include "SerialFiller/CobsTranscoder.hpp"

using namespace mn;

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
        ByteArray rawData = ByteArray({0x01, 0x02, 0x03});
        ByteArray encodedData;
        CobsTranscoder::Encode(rawData, encodedData);
        EXPECT_EQ(ByteArray({0x04, 0x01, 0x02, 0x03, 0x00}), encodedData);

        ByteArray decodedData;
        CobsTranscoder::DecodeStatus decodeStatus = CobsTranscoder::Decode(encodedData, decodedData);
        EXPECT_EQ(CobsTranscoder::DecodeStatus::SUCCESS, decodeStatus);
        EXPECT_EQ(rawData, decodedData);
    }

    TEST_F(CobsEncodeDecodeTest, ZeroInDataTest) {
        ByteArray rawData = ByteArray({0xAA, 0x00, 0xAB});
        ByteArray encodedData;
        CobsTranscoder::Encode(rawData, encodedData);
        EXPECT_EQ(ByteArray({ 0x02, 0xAA, 0x02, 0xAB, 0x00 }), encodedData);

        ByteArray decodedData;
        CobsTranscoder::DecodeStatus decodeStatus = CobsTranscoder::Decode(encodedData, decodedData);
        EXPECT_EQ(CobsTranscoder::DecodeStatus::SUCCESS, decodeStatus);
        EXPECT_EQ(rawData, decodedData);
    }

    TEST_F(CobsEncodeDecodeTest, AllZerosTest) {
        ByteArray rawData = ByteArray({0x00, 0x00, 0x00});
        ByteArray encodedData;
        CobsTranscoder::Encode(rawData, encodedData);
        EXPECT_EQ(ByteArray({0x01, 0x01, 0x01, 0x01, 0x00}), encodedData);
        ByteArray decodedData;
        CobsTranscoder::DecodeStatus decodeStatus = CobsTranscoder::Decode(encodedData, decodedData);
        EXPECT_EQ(CobsTranscoder::DecodeStatus::SUCCESS, decodeStatus);
        EXPECT_EQ(rawData, decodedData);
    }

    TEST_F(CobsEncodeDecodeTest, ManyBytesTest) {
        ByteArray rawData = ByteArray({0x00, 0xAA, 0xAB, 0xAC, 0x00, 0x00, 0xAD});
        ByteArray encodedData;
        CobsTranscoder::Encode(rawData, encodedData);
        EXPECT_EQ(ByteArray({0x01, 0x04, 0xAA, 0xAB, 0xAC, 0x01, 0x02, 0xAD, 0x00}), encodedData);
        ByteArray decodedData;
        CobsTranscoder::DecodeStatus decodeStatus = CobsTranscoder::Decode(encodedData, decodedData);
        EXPECT_EQ(rawData, decodedData);
    }

    TEST_F(CobsEncodeDecodeTest, IncorrectDecodeTest) {
        // Encoded data is incorrectly formed, 0x02 states there should be 1 byte of valid data following,
        // but next byte is 0x00 (end of frame)
        ByteArray decodedData;
        CobsTranscoder::DecodeStatus decodeStatus = CobsTranscoder::Decode(ByteArray({0x02, 0x00, 0x00}), decodedData);
        EXPECT_EQ(CobsTranscoder::DecodeStatus::ERROR_ZERO_BYTE_NOT_EXPECTED, decodeStatus);
        EXPECT_EQ(ByteArray(), decodedData);
    }

    TEST_F(CobsEncodeDecodeTest, IncorrectDecodeTest2) {
        // Error here is the 0x00 after 0x03
        ByteArray decodedData;
        CobsTranscoder::DecodeStatus decodeStatus = CobsTranscoder::Decode(ByteArray({0x02, 0xAA, 0x03, 0x00, 0xAB}), decodedData);
        EXPECT_EQ(CobsTranscoder::DecodeStatus::ERROR_ZERO_BYTE_NOT_EXPECTED, decodeStatus);
        EXPECT_EQ(ByteArray(), decodedData);
    }

}  // namespace