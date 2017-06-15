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
        std::string rawData = std::string({0x01, 0x02, 0x03});
        std::string encodedData;
        CobsTranscoder::Encode(rawData, encodedData);
        EXPECT_EQ(std::string({0x04, 0x01, 0x02, 0x03, 0x00}), encodedData);

        std::string decodedData;
        CobsTranscoder::DecodeStatus decodeStatus = CobsTranscoder::Decode(encodedData, decodedData);
        EXPECT_EQ(CobsTranscoder::DecodeStatus::SUCCESS, decodeStatus);
        EXPECT_EQ(rawData, decodedData);
    }

    TEST_F(CobsEncodeDecodeTest, ZeroInDataTest) {
        std::string rawData = std::string({'\xAA', '\x00', '\xAB'});
        std::string encodedData;
        CobsTranscoder::Encode(rawData, encodedData);
        EXPECT_EQ(std::string({'\x02', '\xAA', '\x02', '\xAB', '\x00'}), encodedData);

        std::string decodedData;
        CobsTranscoder::DecodeStatus decodeStatus = CobsTranscoder::Decode(encodedData, decodedData);
        EXPECT_EQ(CobsTranscoder::DecodeStatus::SUCCESS, decodeStatus);
        EXPECT_EQ(rawData, decodedData);
    }

    TEST_F(CobsEncodeDecodeTest, AllZerosTest) {
        std::string rawData = std::string({0x00, 0x00, 0x00});
        std::string encodedData;
        CobsTranscoder::Encode(rawData, encodedData);
        EXPECT_EQ(std::string({0x01, 0x01, 0x01, 0x01, 0x00}), encodedData);
        std::string decodedData;
        CobsTranscoder::DecodeStatus decodeStatus = CobsTranscoder::Decode(encodedData, decodedData);
        EXPECT_EQ(CobsTranscoder::DecodeStatus::SUCCESS, decodeStatus);
        EXPECT_EQ(rawData, decodedData);
    }

    TEST_F(CobsEncodeDecodeTest, ManyBytesTest) {
        std::string rawData = std::string({0x00, '\xAA', '\xAB', '\xAC', 0x00, 0x00, '\xAD'});
        std::string encodedData;
        CobsTranscoder::Encode(rawData, encodedData);
        EXPECT_EQ(std::string({0x01, 0x04, '\xAA', '\xAB', '\xAC', 0x01, 0x02, '\xAD', 0x00}), encodedData);
        std::string decodedData;
        CobsTranscoder::DecodeStatus decodeStatus = CobsTranscoder::Decode(encodedData, decodedData);
        EXPECT_EQ(rawData, decodedData);
    }

    TEST_F(CobsEncodeDecodeTest, IncorrectDecodeTest) {
        // Encoded data is incorrectly formed, 0x02 states there should be 1 byte of valid data following,
        // but next byte is 0x00 (end of frame)
        std::string decodedData;
        CobsTranscoder::DecodeStatus decodeStatus = CobsTranscoder::Decode(std::string({0x02, 0x00, 0x00}), decodedData);
        EXPECT_EQ(CobsTranscoder::DecodeStatus::ERROR_ZERO_BYTE_NOT_EXPECTED, decodeStatus);
        EXPECT_EQ(std::string(), decodedData);
    }

    TEST_F(CobsEncodeDecodeTest, IncorrectDecodeTest2) {
        // Error here is the 0x00 after 0x03
        std::string decodedData;
        CobsTranscoder::DecodeStatus decodeStatus = CobsTranscoder::Decode(std::string({0x02, '\xAA', 0x03, 0x00, '\xAB'}), decodedData);
        EXPECT_EQ(CobsTranscoder::DecodeStatus::ERROR_ZERO_BYTE_NOT_EXPECTED, decodeStatus);
        EXPECT_EQ(std::string(), decodedData);
    }

}  // namespace