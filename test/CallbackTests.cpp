#include "gtest/gtest.h"

#include "SerialFiller/SerialFiller.hpp"

using namespace MN;

namespace {

    class CallbackTests : public ::testing::Test {
    protected:

        CallbackTests() {
        }

        virtual ~CallbackTests() {
        }
    };

    TEST_F(CallbackTests, CallbackTest1) {


        SerialFiller serialFiller;

        std::vector<uint8_t> savedTxData;
        serialFiller.callback = [&](std::vector<uint8_t> txData) -> void {
            savedTxData = txData;
        };

        serialFiller.Publish("test-topic", "hello");

        EXPECT_NE(0, savedTxData.size());



    }

}  // namespace