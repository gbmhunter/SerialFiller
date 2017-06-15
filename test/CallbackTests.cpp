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

        std::string savedTxData;
        serialFiller.txDataReady_ = [&](std::string txData) -> void {
            savedTxData = txData;
        };

        serialFiller.Publish("test-topic", "hello");

        EXPECT_NE(0, savedTxData.size());



    }

}  // namespace