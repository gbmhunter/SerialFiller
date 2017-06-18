#include "gtest/gtest.h"

#include "SerialFiller/SerialFiller.hpp"

using namespace mn;

namespace {

    class AddAndVerifyCrcTests : public ::testing::Test {
    protected:

        AddAndVerifyCrcTests() {
        }

        virtual ~AddAndVerifyCrcTests() {
        }
    };

    TEST_F(AddAndVerifyCrcTests, StandardCheckTest) {
        EXPECT_EQ(true, SerialFiller::VerifyCrc(ByteArray({ 0x01, 0x02, 0x03, 0xAD, 0xAD })));
    }

}  // namespace