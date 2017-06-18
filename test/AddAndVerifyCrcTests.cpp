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
        std::string packet({ '\x01', '\x02', '\x03', '\xAD', '\xAD' });
        EXPECT_EQ(true, SerialFiller::VerifyCrc(packet));
    }

}  // namespace